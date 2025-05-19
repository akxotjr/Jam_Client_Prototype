#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"

Session::Session(ServiceRef service, boost::asio::any_io_executor executor)
	: _service(service), _executor(executor)
{
}

Session::~Session()
{
}

TcpSession::TcpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: Session(service, executor), _socket(executor), _recvBuffer(BUFFER_SIZE)
{
	auto owner = _service.lock();
	if (!owner) return;

	_remoteEndpoint = owner->GetTcpRemoteEndpoint();
}

TcpSession::~TcpSession()
{
	_socket.close();
}

void TcpSession::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCK
		_sendQueue.push(sendBuffer); // how about change to lock-free queue?

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void TcpSession::Connect()
{
	RegisterConnect();
}

void TcpSession::Disconnect(const string cause)
{
	if (_connected.exchange(false) == false)
		return;

	RegisterDisconnect();
}


void TcpSession::RegisterConnect()
{
	_socket.open(tcp::v4());

	_socket.set_option(boost::asio::ip::tcp::no_delay(true));
	_socket.set_option(boost::asio::socket_base::keep_alive(true));

	_socket.bind(tcp::endpoint(tcp::v4(), 0));

	if (GetRemoteEndpoint().port() == 0) return;

	_socket.async_connect(GetRemoteEndpoint(),
		[this, self = shared_from_this()](const boost::system::error_code& ec)
		{
			if (!ec)
			{
				ProcessConnect();
			}
			else
			{
				std::cout << "[ERROR] Connection failed: " << ec.message() << " (code=" << ec.value() << ")" << std::endl;
			}
		});
}

void TcpSession::RegisterDisconnect()
{
	if (_socket.is_open())
	{
		boost::system::error_code ec;
		_socket.cancel(ec);
		_socket.close(ec);
		if (ec)
		{
			std::cout << "[ERROR] Socket Close Error : " << ec.message() << " (code=" << ec.value() << ")" << std::endl;
		}
		else
		{
			ProcessDisconnect();
		}
	}
}

void TcpSession::RegisterSend()
{
	if (IsConnected() == false || _socket.is_open() == false)
		return;

	vector<boost::asio::const_buffer> sendBuffers;

	_currentSendBuffers.clear();

	{
		WRITE_LOCK

		if (_sendQueue.empty())
			return;

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			if (sendBuffer->Buffer() && sendBuffer->WriteSize() > 0)
			{
				writeSize += sendBuffer->WriteSize();
				_currentSendBuffers.push_back(sendBuffer);
				sendBuffers.push_back(boost::asio::buffer(sendBuffer->Buffer(), sendBuffer->WriteSize()));
			}
			_sendQueue.pop();
		}
	}

	// Scatter-Gather
	boost::asio::async_write(_socket, sendBuffers,
		[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			if (!ec)
			{
				ProcessSend(bytes_transferred);
			}
			else
			{
				// retry send
				{
					WRITE_LOCK
					for (auto& buffer : _currentSendBuffers)
					{
						_sendQueue.push(buffer);
					}
					_currentSendBuffers.clear();
				}
			}
		});
}

void TcpSession::RegisterRecv()
{
	if (!IsConnected() || !_socket.is_open())
		return;

	_socket.async_receive(
		boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()),
		[this](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (ec)
			{
				std::cout << "[ERROR] Tcp fail to receive : " << ec.message() << " (code=" << ec.value() << ")" << std::endl;
				Disconnect(ec.message());
				return;
			}

			ProcessRecv(bytes_transferred);
		});
}

void TcpSession::ProcessConnect()
{
	_connected.store(true);
	GetService()->AddTcpSession(static_pointer_cast<TcpSession>(shared_from_this()));
	OnConnected();

	RegisterRecv();
}

void TcpSession::ProcessDisconnect()
{
	_connected.store(false);
	OnDisconnected();
	GetService()->ReleaseTcpSession(static_pointer_cast<TcpSession>(shared_from_this()));

	{
		WRITE_LOCK

		_sendQueue = {};
		_currentSendBuffers.clear();
		_sendRegistered.store(false);
	}
}

void TcpSession::ProcessSend(int32 numOfBytes)
{
	if (numOfBytes == 0)
	{
		Disconnect("Send 0 byte");
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCK
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void TcpSession::ProcessRecv(int32 numOfBytes)
{
	if (numOfBytes == 0)
	{
		Disconnect("Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect("OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = IsParsingPacket(_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect("OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();
	RegisterRecv();
}

int32 TcpSession::IsParsingPacket(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;

		if (dataSize < sizeof(TcpPacketHeader))
			break;

		TcpPacketHeader* header = reinterpret_cast<TcpPacketHeader*>(&buffer[processLen]);

		if (dataSize < header->size || header->size < sizeof(TcpPacketHeader))
			break;

		OnRecv(&buffer[processLen], header->size);

		processLen += header->size;
	}

	return processLen;
}



ReliableUdpSession::ReliableUdpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: Session(service, executor)
{
	auto owner = _service.lock();
	if (!owner) return;

	_remoteEndpoint = owner->GetUdpRemoteEndpoint();
}

ReliableUdpSession::~ReliableUdpSession()
{
}

void ReliableUdpSession::Connect()
{
	//RegisterConnect();
}

void ReliableUdpSession::Disconnect(const string cause)
{
	std::cout << "[UDP] Disconnect : " << cause << std::endl;

	if (_connected.exchange(false) == false)
		return;

	//RegisterDisconnect();
}

void ReliableUdpSession::Send(SendBufferRef sendBuffer)
{
	//bool registerSend = false;

	//{
	//	WRITE_LOCK
	//	_sendQueue.push(sendBuffer);

	//	if (_sendRegistered.exchange(true) == false)
	//		registerSend = true;
	//}

	//if (registerSend)
	RegisterSend(sendBuffer);
}

void ReliableUdpSession::SendReliable(SendBufferRef sendBuffer, double timestamp)
{
	uint16 seq = _sendSeq++;

	UdpPacketHeader* header = reinterpret_cast<UdpPacketHeader*>(sendBuffer->Buffer());
	header->sequence = seq;

	PendingPacket pkt = { .buffer = sendBuffer, .sequence = seq, .timestamp = timestamp, .retryCount = 0 };

	{
		WRITE_LOCK
		_pendingAckMap[seq] = pkt;
	}

	Send(sendBuffer);
}

void ReliableUdpSession::HandleAck(uint16 latestSeq, uint32 bitfield)
{
	WRITE_LOCK

	for (int i = 0; i <= 32; ++i)
	{
		uint16 ackSeq = latestSeq - i;

		if (i == 0 || (bitfield & (1 << (i - 1))))
		{
			auto it = _pendingAckMap.find(ackSeq);
			if (it != _pendingAckMap.end())
			{
				_pendingAckMap.erase(it);
			}
		}
	}
}

bool ReliableUdpSession::CheckAndRecordReceiveHistory(uint16 seq)
{
	if (!IsSeqGreater(seq, _latestSeq - 1024))
		return false;

	if (_receiveHistory.test(seq % 1024))
		return false;

	_receiveHistory.set(seq % 1024);
	_latestSeq = IsSeqGreater(seq, _latestSeq) ? seq : _latestSeq;
	return true;
}

uint32 ReliableUdpSession::GenerateAckBitfield(uint16 latestSeq)
{
	uint32 bitfield = 0;
	for (int i = 1; i <= 32; ++i)
	{
		uint16 seq = latestSeq - i;

		if (!IsSeqGreater(latestSeq, seq))
			continue;

		if (_receiveHistory.test(seq % 1024))
		{
			bitfield |= (1 << (i - 1));
		}
	}
	return bitfield;
}

//void ReliableUdpSession::RegisterConnect()
//{
//	_socket.open(udp::v4());
//
//	udp::resolver resolver(_executor);
//	auto endpoints = resolver.resolve(_netAddress.ip, _netAddress.port);
//	_endpoint = *endpoints.begin();
//
//	RegisterRecv();
//}

//void ReliableUdpSession::RegisterDisconnect()
//{
//	if (_socket.is_open())
//	{
//		boost::system::error_code ec;
//		_socket.cancel(ec);
//		_socket.close(ec);
//		if (ec)
//		{
//			std::cout << "[ERROR] Socket Close Error : " << ec.message() << " (code=" << ec.value() << ")" << std::endl;
//		}
//		else
//		{
//			ProcessDisconnect();
//		}
//	}
//
//	//{
//	//	WRITE_LOCK
//
//	//	_sendQueue = {};
//	//	_currentSendBuffers.clear();
//	//	_sendRegistered.store(false);
//	//}
//}

void ReliableUdpSession::RegisterSend(SendBufferRef sendBuffer)
{
	if (GetService()->GetUdpSocket().is_open() == false)
		return;

	GetService()->GetUdpSocket().async_send_to(
		boost::asio::buffer(sendBuffer->Buffer(), sendBuffer->WriteSize()),
		_remoteEndpoint,
		[this, self = shared_from_this(), sendBuffer](const boost::system::error_code& ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				ProcessSend(static_cast<int32>(bytes_transferred));
			}
			else
			{
				std::cout << "[ERROR] UDP send failed: " << ec.message() << " (code=" << ec.value() << ")\n";
				// todo: retry logic or fallback
			}
		});
}

void ReliableUdpSession::ProcessConnect()
{
	_connected.store(true);
	GetService()->CompleteUdpHandshake(GetRemoteEndpoint());
	OnConnected();
}

void ReliableUdpSession::ProcessDisconnect()
{
	_connected.store(false);
	OnDisconnected();
	GetService()->ReleaseUdpSession(static_pointer_cast<ReliableUdpSession>(shared_from_this()));

	{
		WRITE_LOCK

		_sendQueue = {};
		_currentSendBuffers.clear();
		_sendRegistered.store(false);
	}
}

void ReliableUdpSession::ProcessSend(int32 numOfBytes)
{
	if (numOfBytes == 0)
	{
		Disconnect("Send 0 byte");
		return;
	}

	OnSend(numOfBytes);
}

void ReliableUdpSession::Update(float serverTime)
{
	Vector<uint16> resendList;

	{
		WRITE_LOCK

		for (auto& [seq, pkt] : _pendingAckMap)
		{
			float elapsed = serverTime - pkt.timestamp;

			if (elapsed >= _resendIntervalMs)
			{
				pkt.timestamp = serverTime;
				pkt.retryCount++;

				resendList.push_back(seq);
			}

			if (pkt.retryCount > 5)
			{
				std::cout << "[ReliableUDP] Max retry reached. Disconnecting.\n";
				Disconnect("Too many retries");
				continue;
			}
		}
	}

	for (uint16 seq : resendList)
	{
		auto it = _pendingAckMap.find(seq);
		if (it != _pendingAckMap.end())
		{
			std::cout << "[ReliableUDP] Re-sending seq: " << seq << "\n";
			SendReliable(it->second.buffer, serverTime);
		}
	}
}


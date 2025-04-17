#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include <boost/system/error_code.hpp>

Session::Session(ServiceRef service, boost::asio::any_io_executor executor)
	: _service(service), _executor(executor) /*_socket(executor),*//* _recvBuffer(BUFFER_SIZE)*/
{
}

Session::~Session()
{
	//_socket.close();
}

//void Session::Send(SendBufferRef sendBuffer)
//{
//	if (IsConnected() == false)
//		return;
//
//	bool registerSend = false;
//
//	{
//		WRITE_LOCK
//		_sendQueue.push(sendBuffer); // how about change to lock-free queue?
//
//		if (_sendRegistered.exchange(true) == false)
//			registerSend = true;
//	}
//
//	if (registerSend)
//		DoSend();
//}


//void Session::DoSend()
//{
//	if (IsConnected() == false || _socket.is_open() == false)
//		return;
//
//	vector<boost::asio::const_buffer> sendBuffers;
//
//	{
//		WRITE_LOCK
//
//		if (_sendQueue.empty())
//			return;
//
//		int32 writeSize = 0;
//		while (_sendQueue.empty() == false)
//		{
//			SendBufferRef sendBuffer = _sendQueue.front();
//
//			if (sendBuffer->Buffer() && sendBuffer->WriteSize() > 0)
//			{
//				writeSize += sendBuffer->WriteSize();
//				_currentSendBuffers.push_back(sendBuffer);
//				sendBuffers.push_back(boost::asio::buffer(sendBuffer->Buffer(), sendBuffer->WriteSize()));
//			}
//			_sendQueue.pop();
//		}
//	}
//
//	// Scatter-Gather
//
//	boost::asio::async_write(_socket, sendBuffers,
//		[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
//			if (!ec) 
//			{
//				std::cout << "Bytes sent: " << bytes_transferred << std::endl;
//				_sendRegistered.store(false);
//
//				{
//					WRITE_LOCK
//					_currentSendBuffers.clear();
//				}
//
//				OnSend(static_cast<int32>(bytes_transferred));
//			}
//			else 
//			{
//				std::cout << "Error: " << ec.message() << std::endl;
//
//				// retry send
//				{
//					WRITE_LOCK
//					for (auto& buffer : _currentSendBuffers)
//					{
//						_sendQueue.push(buffer);
//					}
//					_currentSendBuffers.clear();
//				}
//			}
//		});
//}

////void Session::DoRecv()  
////{  
////if (IsConnected() == false || _socket.is_open() == false)  
////	return;  
////
////_recvBuffer.Clean();  
////
//////_socket.async_read_some(  
//////	boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()),  
//////	[this](boost::system::error_code ec, size_t bytes_transferred) {  
//////		if (!ec) {  
//////			ProcessRecv(bytes_transferred);  
//////		}  
//////	});  
////
////	boost::asio::async_read(_socket, boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()), boost::asio::transfer_at_least(2),  
////		[this](const boost::system::error_code& ec, size_t bytes_transferred) {  
////			if (!ec) {  
////				ProcessRecv(bytes_transferred);  
////			}  
////		});  
////}

//void Session::DoRecvHeader()
//{
//	if (!IsConnected() || !_socket.is_open())
//		return;
//
//	_recvBuffer.Clean();
//
//	// 헤더 크기만큼 정확히 읽기
//	boost::asio::async_read(_socket,
//		boost::asio::buffer(_recvBuffer.WritePos(), sizeof(PacketHeader)),
//		boost::asio::transfer_exactly(sizeof(PacketHeader)),
//		[this](boost::system::error_code ec, size_t bytes_transferred)
//		{
//			if (!ec)
//			{
//				_recvBuffer.OnWrite(bytes_transferred);
//				PacketHeader* header = reinterpret_cast<PacketHeader*>(_recvBuffer.ReadPos());
//				DoRecvBody(header->size - sizeof(PacketHeader));
//			}
//			else
//			{
//				std::cout << "Recv header error: " << ec.message() << "\n";
//				Disconnect(ec.message());
//			}
//		});
//}
//
//void Session::DoRecvBody(int32 bodySize)
//{
//	if (!IsConnected() || !_socket.is_open())
//		return;
//
//	// 본문 크기만큼 정확히 읽기
//	boost::asio::async_read(_socket,
//		boost::asio::buffer(_recvBuffer.WritePos(), bodySize),
//		boost::asio::transfer_exactly(bodySize),
//		[this, bodySize](boost::system::error_code ec, size_t bytes_transferred)
//		{
//			if (!ec)
//			{
//				_recvBuffer.OnWrite(bytes_transferred);
//
//				ProcessRecv(sizeof(PacketHeader) + bodySize);
//			}
//			else
//			{
//				std::cout << "Recv body error: " << ec.message() << "\n";
//				Disconnect(ec.message());
//			}
//		});
//}
//
//void Session::ProcessRecv(size_t bytes_transferred)
//{
//	if (bytes_transferred == 0)
//	{
//		Disconnect("Recv 0");
//		return;
//	}
//
//	//if (_recvBuffer.OnWrite(bytes_transferred) == false)
//	//{
//	//	Disconnect("OnWrite Overflow");
//	//	return;
//	//}
//
//	int32 dataSize = _recvBuffer.DataSize();
//	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
//
//	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
//	{
//		Disconnect("OnRead Overflow");
//		return;
//	}
//
//	//_recvBuffer.Clean();
//
//	DoRecvHeader();
//}

//void Session::Connect()
//{
//	tcp::resolver resolver(_executor);
//
//	auto endpoints = resolver.resolve(_netAddress.ip, _netAddress.port);
//
//	bool result = true;
//
//	auto service = _service.lock();
//	
//
//	boost::asio::async_connect(_socket, endpoints,
//		[&result, this](const boost::system::error_code& ec, const tcp::endpoint& endpoint) {
//			if (!ec) 
//			{
//				std::cout << "Connected to: " << endpoint << std::endl;
//				_connected.store(true);
//
//				DoRecvHeader();
//
//				OnConnected();
//				//DoRecv();
//			}
//			else 
//			{
//				std::cout << "Connection failed: " << ec.message() << std::endl;
//				OnConnected();
//			}
//		});
//}
//
//void Session::Disconnect(const string cause)
//{
//	if (_connected.exchange(false) == false)
//		return;
//
//	cout << "Disconnect : " << cause << std::endl;
//
//	if (_socket.is_open())
//	{
//		boost::system::error_code ec;
//		_socket.cancel(ec);
//		_socket.close(ec);
//		if (ec)
//		{
//			std::cout << "Socket Close Error : " << ec.message() << std::endl;
//		}
//		else
//		{
//			OnDisconnected();
//		}
//	}
//
//	{
//		WRITE_LOCK
//
//		_sendQueue = {};
//		_currentSendBuffers.clear();
//		_sendRegistered.store(false);
//
//		// TODO
//	}
//}


//PacketSession::PacketSession(ServiceRef service, boost::asio::any_io_executor executor)
//	: Session(service, executor)
//{
//
//}
//
//PacketSession::~PacketSession()
//{
//}
//
//int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
//{
//	int32 processLen = 0;
//
//	while (true)
//	{
//		int32 dataSize = len - processLen;
//
//		if (dataSize < sizeof(PacketHeader))
//			break;
//
//		PacketHeader header = *reinterpret_cast<PacketHeader*>(&buffer[processLen]);
//
//		if (dataSize < header.size)
//			break;
//
//		OnRecvPacket(&buffer[0], header.size);
//		processLen += header.size;
//	}
//	return processLen;
//}

TcpSession::TcpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: Session(service, executor), _socket(executor), _recvBuffer(BUFFER_SIZE)
{
	auto owner = _service.lock();
	if (!owner) return;

	_netAddress = owner->GetTcpNetAddress();
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
	tcp::resolver resolver(_executor);

	auto endpoints = resolver.resolve(_netAddress.ip, _netAddress.port);
	_endpoint = *endpoints.begin();

	boost::asio::async_connect(_socket, endpoints,
		[this, self = shared_from_this()](const boost::system::error_code& ec, const tcp::endpoint& endpoint) {
			if (!ec)
			{
				ProcessConnect();
			}
			else
			{
				std::cout << "Connection failed: " << ec.message() << std::endl;
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
			std::cout << "Socket Close Error : " << ec.message() << std::endl;
		}
		else
		{
			ProcesssDisconnect();
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
				ProcessSend(static_cast<int32>(bytes_transferred));
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

void TcpSession::RegisterRecvHeader()
{
	if (!IsConnected() || !_socket.is_open())
		return;

	_recvBuffer.Clean();

	// 헤더 크기만큼 정확히 읽기
	boost::asio::async_read(_socket,
		boost::asio::buffer(_recvBuffer.WritePos(), sizeof(TcpPacketHeader)),
		boost::asio::transfer_exactly(sizeof(TcpPacketHeader)),
		[this](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				_recvBuffer.OnWrite(bytes_transferred);
				TcpPacketHeader* header = reinterpret_cast<TcpPacketHeader*>(_recvBuffer.ReadPos());

				if (header->size < sizeof(TcpPacketHeader))
				{
					std::cout << "Invalid packet size: " << header->size << std::endl;
					Disconnect("Invalid header size");
					return;
				}

				std::cout << "Recv header success: id=" << header->id << ", size=" << header->size << std::endl;
				RegisterRecvBody(header->size - sizeof(TcpPacketHeader));
			}
			else
			{
				std::cout << "Recv header error: " << ec.message() << "\n";
				Disconnect(ec.message());
			}
		});
}

void TcpSession::RegisterRecvBody(int32 bodySize)
{
	if (!IsConnected() || !_socket.is_open())
		return;

	if (bodySize <= 0 || bodySize > 0x10000) // 너무 크면 방어
	{
		std::cout << "Invalid body size: " << bodySize << std::endl;
		Disconnect("Invalid body size");
		return;
	}

	std::cout << "RegisterRecvBody: " << bodySize << " bytes\n";

	// 본문 크기만큼 정확히 읽기
	boost::asio::async_read(_socket,
		boost::asio::buffer(_recvBuffer.WritePos(), bodySize),
		boost::asio::transfer_exactly(bodySize),
		[this, bodySize](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				ProcessRecv(sizeof(TcpPacketHeader) + bodySize);
			}
			else
			{
				std::cout << "Recv body error: " << ec.message() << "\n";
				Disconnect(ec.message());
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
				std::cout << "recv error: " << ec.message() << "\n";
				Disconnect(ec.message());
				return;
			}

			//temp
			TcpPacketHeader* header = reinterpret_cast<TcpPacketHeader*>(_recvBuffer.ReadPos());
			std::cout << "Recv header success: id=" << header->id << ", size=" << header->size << std::endl;
			//~temp

			ProcessRecv(bytes_transferred);
		});
}

void TcpSession::ProcessConnect()
{
	_connected.store(true);
	GetService()->AddSession(GetSessionRef());
	OnConnected();

	//RegisterRecvHeader();
	RegisterRecv();
}

void TcpSession::ProcesssDisconnect()
{
	OnDisconnected();
	GetService()->ReleaseSession(GetSessionRef());

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

	//RegisterRecvHeader();
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
	: Session(service, executor), _socket(executor), _recvBuffer(BUFFER_SIZE)
{
	auto owner = _service.lock();
	if (!owner) return;

	_netAddress = owner->GetUdpNetAddress();
}

ReliableUdpSession::~ReliableUdpSession()
{
	_socket.close();
}

void ReliableUdpSession::Connect()
{
	RegisterConnect();
}

void ReliableUdpSession::Disconnect(const string cause)
{
	if (_connected.exchange(false) == false)
		return;

	RegisterDisconnect();
}

void ReliableUdpSession::Send(SendBufferRef sendBuffer)
{
	bool registerSend = false;

	{
		WRITE_LOCK
		_sendQueue.push(sendBuffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void ReliableUdpSession::SendReliable(SendBufferRef sendBuffer, float timestamp)
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
			_pendingAckMap.erase(ackSeq);
			std::cout << "[ACK] seq=" << ackSeq << '\n';
		}
	}
}

bool ReliableUdpSession::CheckAndRecordReceiveHistory(uint16 seq)
{
	if (_receiveHistory.test(seq % 1024))
		return false;

	_receiveHistory.set(seq % 1024);
	return true;
}

uint32 ReliableUdpSession::GenerateAckBitfield(uint16 latestSeq)
{
	uint32 bitfield = 0;
	for (int i = 1; i <= 32; ++i)
	{
		uint16 seq = latestSeq - i;
		if (_receiveHistory.test(seq % 1024))
		{
			bitfield |= (1 << (i - 1));
		}
	}

	return bitfield;
}

void ReliableUdpSession::RegisterConnect()
{
	_socket.open(udp::v4());

	udp::resolver resolver(_executor);
	auto endpoints = resolver.resolve(_netAddress.ip, _netAddress.port);
	_endpoint = *endpoints.begin();

	RegisterRecv();
}

void ReliableUdpSession::RegisterDisconnect()
{
	if (_socket.is_open())
	{
		boost::system::error_code ec;
		_socket.cancel(ec);
		_socket.close(ec);
		if (ec)
		{
			std::cout << "Socket Close Error : " << ec.message() << std::endl;
		}
		else
		{
			ProcessDisconnect();
		}
	}

	//{
	//	WRITE_LOCK

	//	_sendQueue = {};
	//	_currentSendBuffers.clear();
	//	_sendRegistered.store(false);
	//}
}

void ReliableUdpSession::RegisterSend()
{
	if (_socket.is_open() == false)
		return;

	if (_sendQueue.empty())
	{
		_sendRegistered = false;
		return;
	}

	Vector<boost::asio::const_buffer> sendBuffers;

	_currentSendBuffers.clear();

	{
		WRITE_LOCK

		int32 writeSize = 0;
		while (!_sendQueue.empty())
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

	_socket.async_send_to(sendBuffers, _endpoint,
		[this, self = shared_from_this()](const boost::system::error_code& ec, size_t bytes_transferred)
		{
			_sendRegistered.store(false);
			if (!ec)
			{
				ProcessSend(static_cast<int32>(bytes_transferred));
			}
			else
			{
				std::cout << "fail to send: " << ec.message() << " (code=" << ec.value() << ")" << std::endl;
				//Disconnect("Failed to send");
				{
					WRITE_LOCK
					for (auto& buffer : _currentSendBuffers)
					{
						_sendQueue.push(buffer);
					}
					_currentSendBuffers.clear();
				}
			}
		}
	);
}

void ReliableUdpSession::RegisterRecv()
{
	if (_socket.is_open() == false)
		return;


	_socket.async_receive_from(
		boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()), _endpoint,
		[this, self = shared_from_this()](const boost::system::error_code& ec, size_t bytes_transferred)
		{
			if (!ec && bytes_transferred > 0)
			{
				ProcessRecv(static_cast<int32>(bytes_transferred)); // 수신된 데이터 처리
			}
			else
			{
				std::cerr << "UDP Receive Error: " << ec.message() << std::endl;
				RegisterDisconnect();
			}
		}
	);
}

void ReliableUdpSession::ProcessConnect()
{
	_connected.store(true);
	GetService()->AddSession(GetSessionRef());
	OnConnected();
}

void ReliableUdpSession::ProcessDisconnect()
{
	OnDisconnected();
	GetService()->ReleaseSession(GetSessionRef());

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

	WRITE_LOCK
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void ReliableUdpSession::ProcessRecv(int32 numOfBytes)
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

int32 ReliableUdpSession::IsParsingPacket(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;

		if (dataSize < sizeof(UdpPacketHeader))
			break;

		UdpPacketHeader* header = reinterpret_cast<UdpPacketHeader*>(&buffer[processLen]);

		if (dataSize < header->size || header->size < sizeof(UdpPacketHeader))
			break;

		OnRecv(&buffer[processLen], header->size);

		processLen += header->size;
	}

	return processLen;
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
			Send(it->second.buffer);
		}
	}
}


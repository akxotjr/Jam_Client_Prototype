#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include <boost/system/error_code.hpp>

Session::Session(ServiceRef service, boost::asio::any_io_executor executor)
	: _service(service), _executor(executor), _socket(executor), _recvBuffer(BUFFER_SIZE)
{
	auto owner = _service.lock();
	if (!owner) return;

	_netAddress = owner->GetNetAddress();
}

Session::~Session()
{
	_socket.close();
}

void Session::Send(SendBufferRef sendBuffer)
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
		DoSend();
}


void Session::DoSend()
{
	if (IsConnected() == false || _socket.is_open() == false)
		return;

	vector<boost::asio::const_buffer> sendBuffers;

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
				std::cout << "Bytes sent: " << bytes_transferred << std::endl;
				_sendRegistered.store(false);

				{
					WRITE_LOCK
					_currentSendBuffers.clear();
				}

				OnSend(static_cast<int32>(bytes_transferred));
			}
			else 
			{
				std::cout << "Error: " << ec.message() << std::endl;

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

//void Session::DoRecv()  
//{  
//if (IsConnected() == false || _socket.is_open() == false)  
//	return;  
//
//_recvBuffer.Clean();  
//
////_socket.async_read_some(  
////	boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()),  
////	[this](boost::system::error_code ec, size_t bytes_transferred) {  
////		if (!ec) {  
////			ProcessRecv(bytes_transferred);  
////		}  
////	});  
//
//	boost::asio::async_read(_socket, boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()), boost::asio::transfer_at_least(2),  
//		[this](const boost::system::error_code& ec, size_t bytes_transferred) {  
//			if (!ec) {  
//				ProcessRecv(bytes_transferred);  
//			}  
//		});  
//}

void Session::DoRecvHeader()
{
	if (!IsConnected() || !_socket.is_open())
		return;

	_recvBuffer.Clean();

	// ��� ũ�⸸ŭ ��Ȯ�� �б�
	boost::asio::async_read(_socket,
		boost::asio::buffer(_recvBuffer.WritePos(), sizeof(PacketHeader)),
		boost::asio::transfer_exactly(sizeof(PacketHeader)),
		[this](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				_recvBuffer.OnWrite(bytes_transferred);
				PacketHeader* header = reinterpret_cast<PacketHeader*>(_recvBuffer.ReadPos());
				DoRecvBody(header->size - sizeof(PacketHeader));
			}
			else
			{
				std::cout << "Recv header error: " << ec.message() << "\n";
				Disconnect(ec.message());
			}
		});
}

void Session::DoRecvBody(int32 bodySize)
{
	if (!IsConnected() || !_socket.is_open())
		return;

	// ���� ũ�⸸ŭ ��Ȯ�� �б�
	boost::asio::async_read(_socket,
		boost::asio::buffer(_recvBuffer.WritePos(), bodySize),
		boost::asio::transfer_exactly(bodySize),
		[this, bodySize](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				_recvBuffer.OnWrite(bytes_transferred);

				ProcessRecv(sizeof(PacketHeader) + bodySize);
			}
			else
			{
				std::cout << "Recv body error: " << ec.message() << "\n";
				Disconnect(ec.message());
			}
		});
}

void Session::ProcessRecv(size_t bytes_transferred)
{
	if (bytes_transferred == 0)
	{
		Disconnect("Recv 0");
		return;
	}

	//if (_recvBuffer.OnWrite(bytes_transferred) == false)
	//{
	//	Disconnect("OnWrite Overflow");
	//	return;
	//}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect("OnRead Overflow");
		return;
	}

	//_recvBuffer.Clean();

	DoRecvHeader();
}

void Session::Connect()
{
	tcp::resolver resolver(_executor);

	auto endpoints = resolver.resolve(_netAddress.ip, _netAddress.port);

	bool result = true;

	auto service = _service.lock();
	

	boost::asio::async_connect(_socket, endpoints,
		[&result, this](const boost::system::error_code& ec, const tcp::endpoint& endpoint) {
			if (!ec) 
			{
				std::cout << "Connected to: " << endpoint << std::endl;
				_connected.store(true);

				DoRecvHeader();

				OnConnected();
				//DoRecv();
			}
			else 
			{
				std::cout << "Connection failed: " << ec.message() << std::endl;
				OnConnected();
			}
		});
}

void Session::Disconnect(const string cause)
{
	if (_connected.exchange(false) == false)
		return;

	cout << "Disconnect : " << cause << std::endl;

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
			OnDisconnected();
		}
	}

	{
		WRITE_LOCK

		_sendQueue = {};
		_currentSendBuffers.clear();
		_sendRegistered.store(false);

		// TODO
	}
}


PacketSession::PacketSession(ServiceRef service, boost::asio::any_io_executor executor)
	: Session(service, executor)
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *reinterpret_cast<PacketHeader*>(&buffer[processLen]);

		if (dataSize < header.size)
			break;

		OnRecvPacket(&buffer[0], header.size);
		processLen += header.size;
	}
	return processLen;
}
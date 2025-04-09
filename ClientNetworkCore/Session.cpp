#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include <boost/system/error_code.hpp>

Session::Session(ServiceRef service, boost::asio::any_io_executor executor)
	: _service(service), _executor(executor) /*_socket(executor),*//* _recvBuffer(BUFFER_SIZE)*/
{
	auto owner = _service.lock();
	if (!owner) return;

	_netAddress = owner->GetNetAddress();
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

	auto endpoints = resolver.resolve(_localAddr.ip, _localAddr.port);

	bool result = true;

	auto service = _service.lock();


	boost::asio::async_connect(_socket, endpoints,
		[&result, this](const boost::system::error_code& ec, const tcp::endpoint& endpoint) {
			if (!ec)
			{
				std::cout << "Connected to: " << endpoint << std::endl;
				_connected.store(true);

				RegisterRecvHeader();

				OnConnected();
			}
			else
			{
				std::cout << "Connection failed: " << ec.message() << std::endl;
				OnConnected();
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

void TcpSession::RegisterSend()
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

void TcpSession::RegisterRecvHeader()
{
	if (!IsConnected() || !_socket.is_open())
		return;

	_recvBuffer.Clean();

	// 헤더 크기만큼 정확히 읽기
	boost::asio::async_read(_socket,
		boost::asio::buffer(_recvBuffer.WritePos(), sizeof(PacketHeader)),
		boost::asio::transfer_exactly(sizeof(PacketHeader)),
		[this](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				_recvBuffer.OnWrite(bytes_transferred);
				PacketHeader* header = reinterpret_cast<PacketHeader*>(_recvBuffer.ReadPos());
				RegisterRecvBody(header->size - sizeof(PacketHeader));
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

	// 본문 크기만큼 정확히 읽기
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

void TcpSession::ProcessSend(int32 numOfBytes)
{

}

void TcpSession::ProcessRecv(int32 numOfBytes)
{
	if (numOfBytes == 0)
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
	int32 processLen = IsParsingPacket(_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect("OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();

	RegisterRecvHeader();
}

int32 TcpSession::IsParsingPacket(BYTE* buffer, int32 len)
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

		OnRecv(&buffer[0], header.size);
		processLen += header.size;
	}
	return processLen;
}



ReliableUdpSession::ReliableUdpSession(ServiceRef service, boost::asio::any_io_executor executor, NetAddress remoteAddr)
	: Session(service, executor), _socket(executor), _remoteAddr(remoteAddr), _recvBuffer(BUFFER_SIZE)
{
}

ReliableUdpSession::~ReliableUdpSession()
{
	_socket.close();
}

void ReliableUdpSession::Send(SendBufferRef sendBuffer)
{

}

void ReliableUdpSession::Connect()
{
	RegisterConnect();
}

void ReliableUdpSession::Disconnect(const string cause)
{
}

void ReliableUdpSession::RegisterSend()
{
	if (_socket.is_open() == false)
		return;

	if (_sendRegistered.exchange(true))
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

	_socket.async_send_to(sendBuffers, *_remoteEndpoints.begin(),
		[this, self = shared_from_this()](const boost::system::error_code& ec, size_t bytes_transferred)
		{
			_sendRegistered.store(false);
			if (!ec)
			{
				ProcessSend();
			}
			else
			{
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
		boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()), _remoteEndpoint,
		[this, self = shared_from_this()](const boost::system::error_code& ec, size_t bytes_transferred)
		{
			if (!ec && bytes_transferred > 0)
			{
				_recvBuffer.OnWrite(bytes_transferred);
				ProcessRecv(); // 수신된 데이터 처리
			}
			else
			{
				std::cerr << "UDP Receive Error: " << ec.message() << std::endl;
				RegisterDisconnect();
			}

			RegisterRecv();
		}
	);
}



void ReliableUdpSession::RegisterConnect()
{
	_socket.open(udp::v4());

	udp::resolver resolver(_executor);
	auto remoteEndpoints = resolver.resolve(_remoteAddr.ip, _remoteAddr.port);
	_remoteEndpoint = *remoteEndpoints.begin();

	RegisterRecv();
}

void ReliableUdpSession::RegisterDisconnect()
{
}

void ReliableUdpSession::ProcessSend()
{
}

void ReliableUdpSession::ProcessRecv()
{
}

void ReliableUdpSession::ProcessConnect()
{
}

void ReliableUdpSession::ProcessDisconnect()
{
}

#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include <boost/system/error_code.hpp>

Session::Session(ServiceRef service, boost::asio::any_io_executor executor)
	: _service(service), _executor(executor), _socket(boost::asio::system_executor())
{
	auto owner = _service.lock();
	if (!owner) return;

	_socket = tcp::socket(_executor);

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

	if (_sendQueue.empty())
		return;

	vector<boost::asio::const_buffer> sendBuffers;

	{
		WRITE_LOCK

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
				WRITE_LOCK
				_currentSendBuffers.clear();

				OnSend(static_cast<int32>(bytes_transferred));
			}
			else 
			{
				std::cout << "Error: " << ec.message() << std::endl;

				// retry send
				WRITE_LOCK
				for (auto& buffer : _currentSendBuffers)
				{
					_sendQueue.push(buffer);
				}
				_currentSendBuffers.clear();
			}
		});
}

bool Session::Connect()
{
	tcp::resolver resolver(_executor);

	auto endpoints = resolver.resolve(_netAddress.ip, _netAddress.port);

	bool result = true;

	boost::asio::async_connect(_socket, endpoints,
		[&result, this](const boost::system::error_code& ec, const tcp::endpoint& endpoint) {
			if (!ec) 
			{
				std::cout << "Connected to: " << endpoint << std::endl;
				_connected.store(true);
				OnConnected();
			}
			else 
			{
				std::cout << "Connection failed: " << ec.message() << std::endl;
			}
		});

	return result;
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
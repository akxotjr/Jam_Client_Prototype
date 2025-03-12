#pragma once

#include <boost/asio.hpp>
#include "Service.h"

using boost::asio::ip::tcp;

class Session : public enable_shared_from_this<Session>
{
	enum { BUFFER_SIZE = 0x10000 }; // 64KB

	USE_LOCK

public:
	Session(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~Session();

public:
	void					Send(SendBufferRef sendBuffer);
	bool					Connect();
	void					Disconnect(const string cause);


	shared_ptr<Service>		GetService() { return _service.lock(); }
	void					SetService(shared_ptr<Service> service) { _service = service; }

	void			SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress		GetAddress() { return _netAddress; }
	//remove_reference_t<asio::basic_stream_socket<tcp>&> GetSocket() { return std::move(_socket); }
	bool			IsConnected() { return _connected; }
	SessionRef		GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	void DoSend();

protected:

	virtual void	OnConnected() {}
	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected() {}

private:
	weak_ptr<Service>		_service;
	tcp::socket				_socket;
	boost::asio::any_io_executor	_executor;
	NetAddress				_netAddress;

	Atomic<bool>            _connected = false;


private:
	Atomic<bool> _sendRegistered = false;
	queue<SendBufferRef> _sendQueue;
	Vector<SendBufferRef> _currentSendBuffers;
};


struct PacketHeader
{
	uint16 size;
	uint16 id;	// Protocol ID
};

class PacketSession : public Session
{
public:
	PacketSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~PacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
};
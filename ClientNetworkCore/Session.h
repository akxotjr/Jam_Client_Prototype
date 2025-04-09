#pragma once

#include "boost/asio.hpp"
#include "Service.h"
#include "RecvBuffer.h"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class Session : public enable_shared_from_this<Session>
{
	enum { BUFFER_SIZE = 0x10000 }; // 64KB

	USE_LOCK

public:
	Session(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~Session();

public:
	virtual void					Send(SendBufferRef sendBuffer) = 0;
	virtual void					Connect() = 0;
	virtual void					Disconnect(const string cause) = 0;


	shared_ptr<Service>				GetService() { return _service.lock(); }
	void							SetService(shared_ptr<Service> service) { _service = service; }

	NetAddress						GetLocalNetAddress() { return _localAddr; }
	void							SetLocalNetAddress(NetAddress address) { _localAddr = address; }
	bool							IsConnected() { return _connected; }
	SessionRef						GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
	void							SetId(int32 id) { _id = id; }
	int32							GetId() { return _id; }

	//void DoRecv();

	//temp
	//void DoRecvHeader();
	//void DoRecvBody(int32 bodySize);

	//void ProcessRecv(size_t bytes_transferred);

//private:
//	void DoSend();
	

protected:
	virtual void	OnConnected() {}
	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected() {}

protected:
	weak_ptr<Service>					_service;
	//tcp::socket							_socket;
	boost::asio::any_io_executor		_executor;
	NetAddress							_localAddr;

	Atomic<bool>						_connected = false;

	uint32 _id = 0;

//private:
//	Atomic<bool>						_sendRegistered = false;
//	queue<SendBufferRef>				_sendQueue;
//	Vector<SendBufferRef>				_currentSendBuffers;
//
//	RecvBuffer							_recvBuffer;
};


struct PacketHeader
{
	uint16 size;
	uint16 id;	// Protocol ID
};

//class PacketSession : public Session
//{
//public:
//	PacketSession(ServiceRef service, boost::asio::any_io_executor executor);
//	virtual ~PacketSession();
//
//	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }
//
//protected:
//	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
//	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
//};

class TcpSession : public Session
{
	USE_LOCK

	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	TcpSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~TcpSession();

	virtual void Send(SendBufferRef sendBuffer) override;
	virtual void Connect() override;
	virtual void Disconnect(const string cause) override;

private:
	void RegisterConnect();
	void RegisterDisconnect();
	void RegisterSend();
	void RegisterRecvHeader();
	void RegisterRecvBody(int32 bodySize);

	void ProcessSend(int32 numOfBytes);
	void ProcessRecv(int32 numOfBytes);


	int32 IsParsingPacket(BYTE* buffer, int32 len);

private:
	tcp::socket							_socket;

	Atomic<bool>						_sendRegistered = false;
	queue<SendBufferRef>				_sendQueue;
	Vector<SendBufferRef>				_currentSendBuffers;

	RecvBuffer							_recvBuffer;
};


struct UdpPacketHeader : public PacketHeader
{
	uint16 sequence;
};

class ReliableUdpSession : public Session
{
	USE_LOCK

	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	ReliableUdpSession(ServiceRef service, boost::asio::any_io_executor executor, NetAddress remoteAddr);
	virtual ~ReliableUdpSession();

	virtual void Send(SendBufferRef sendBuffer) override;
	virtual void Connect() override;
	virtual void Disconnect(const string cause) override;

private:
	void RegisterSend();
	void RegisterRecv();
	void RegisterConnect();
	void RegisterDisconnect();

	void ProcessSend();
	void ProcessRecv();
	void ProcessConnect();
	void ProcessDisconnect();

private:
	udp::socket _socket;
	boost::asio::ip::basic_resolver_results<udp> _localEndpoints;

	//boost::asio::ip::basic_endpoint<udp> _remoteEndpoint;
    //boost::asio::ip::basic_resolver_results<udp> _remoteEndpoints;
	udp::endpoint _remoteEndpoint;

	NetAddress _remoteAddr;

	Atomic<bool> _sendRegistered = false;
	queue<SendBufferRef> _sendQueue;
	Vector<SendBufferRef> _currentSendBuffers;

	RecvBuffer _recvBuffer;
};


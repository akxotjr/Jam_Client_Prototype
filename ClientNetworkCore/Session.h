#pragma once

#include <bitset>

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
	virtual void						Send(SendBufferRef sendBuffer) = 0;
	virtual void						Connect() = 0;
	virtual void						Disconnect(const string cause) = 0;


	ServiceRef							GetService() { return _service.lock(); }
	void								SetService(ServiceRef service) { _service = service; }

	bool								IsConnected() { return _connected; }
	void								SetConnected(bool success) { _connected.store(success); }
	SessionRef							GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }


protected:
	virtual void						OnConnected() {}
	virtual void						OnRecv(BYTE* buffer, int32 len) {}
	virtual void						OnSend(int32 len) {}
	virtual void						OnDisconnected() {}

protected:
	weak_ptr<Service>					_service;
	boost::asio::any_io_executor		_executor;

	Atomic<bool>						_connected = false;
};


struct TcpPacketHeader
{
	uint16 size;
	uint16 id;
};

class TcpSession : public Session
{
	USE_LOCK

	friend class Service;

	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	TcpSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~TcpSession() override;

	virtual void						Send(SendBufferRef sendBuffer) override;
	virtual void						Connect() override;
	virtual void						Disconnect(const string cause) override;

	void								SetRemoteEndpoint(tcp::endpoint& ep) { _remoteEndpoint = ep; }
	tcp::endpoint&						GetRemoteEndpoint() { return _remoteEndpoint; }

private:
	void								RegisterConnect();
	void								RegisterDisconnect();
	void								RegisterSend();
	void								RegisterRecv();

public:
	void								ProcessConnect();
	void								ProcessDisconnect();
	void								ProcessSend(int32 numOfBytes);
	void								ProcessRecv(int32 numOfBytes);


	int32								IsParsingPacket(BYTE* buffer, int32 len);

private:
	tcp::socket							_socket;
	tcp::endpoint						_remoteEndpoint;

	Atomic<bool>						_sendRegistered = false;
	queue<SendBufferRef>				_sendQueue;
	Vector<SendBufferRef>				_currentSendBuffers;

	RecvBuffer							_recvBuffer;
};


struct UdpPacketHeader
{
	uint16 size;
	uint16 id;
	uint16 sequence = 0;
};

struct PendingPacket
{
	SendBufferRef buffer;
	uint16 sequence;
	double timestamp;
	uint32 retryCount = 0;
};

class ReliableUdpSession : public Session
{
	USE_LOCK

	friend class Service;
	friend class UdpReceiver;

	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	ReliableUdpSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~ReliableUdpSession() override;


	virtual void							Connect() override;
	virtual void							Disconnect(const string cause) override;
	virtual void							Send(SendBufferRef sendBuffer) override;
	virtual void							SendReliable(SendBufferRef sendBuffer, double timestamp);

	void									SetRemoteEndpoint(udp::endpoint& ep) { _remoteEndpoint = ep; }
	udp::endpoint&							GetRemoteEndpoint() { return _remoteEndpoint; }

	void									HandleAck(uint16 latestSeq, uint32 bitfield);
	bool									CheckAndRecordReceiveHistory(uint16 seq);
	uint32									GenerateAckBitfield(uint16 latestSeq);

private:	
	void									RegisterSend(SendBufferRef sendBuffer);

	void									ProcessConnect();
	void									ProcessDisconnect();
	void									ProcessSend(int32 numOfBytes);

	void									Update(float serverTime);	// resend
	bool									IsSeqGreater(uint16 a, uint16 b) { return static_cast<int16>(a - b) > 0; }


private:
	udp::endpoint							_remoteEndpoint;

	Atomic<bool>							_sendRegistered = false;
	queue<SendBufferRef>					_sendQueue;
	Vector<SendBufferRef>					_currentSendBuffers;

protected:
	unordered_map<uint16, PendingPacket>	_pendingAckMap;

	bitset<1024>							_receiveHistory;

	uint16									_latestSeq = 0;
	uint16									_sendSeq = 1;			// 다음 보낼 sequence
	float									_resendIntervalMs = 0.1f; // 재전송 대기 시간
};


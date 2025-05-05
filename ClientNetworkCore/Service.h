#pragma once
#include <functional>
#include <boost/asio/any_io_executor.hpp>
#include "UdpReceiver.h"


using boost::asio::ip::tcp;
using boost::asio::ip::udp;
using SessionFactory = function<SessionRef()>;

enum class ProtocolType
{
	PROTOCOL_TCP,
	PROTOCOL_UDP,
};

struct TransportConfig
{
	optional<tcp::endpoint> tcpRemoteEndpoint;
	optional<udp::endpoint> udpRemoteEndpoint;
};

/**
 * @brief 
 */

class Service : public enable_shared_from_this<Service>
{   
public:
	Service(TransportConfig config, int32 maxSTcpSessionCount = 1, int32 maxUdpSessionCount = 1);
	virtual ~Service();

	bool							Start();
	bool							CanStart() const { return _tcpSessionFactory != nullptr || _udpSessionFactory != nullptr; }

	SessionRef						CreateSession(ProtocolType protocol);

	void							Broadcast(SendBufferRef sendBuffer);

	void							AddTcpSession(TcpSessionRef session);
	void							ReleaseTcpSession(TcpSessionRef session);

	void							AddUdpSession(ReliableUdpSessionRef session);
	void							ReleaseUdpSession(ReliableUdpSessionRef session);


	ReliableUdpSessionRef			FindOrCreateUdpSession(udp::endpoint& from);
	void							CompleteUdpHandshake(udp::endpoint& from);

	int32							GetCurrentTcpSessionCount() const { return _tcpSessionCount; }
	int32							GetMaxTcpSessionCount() const { return _maxTcpSessionCount; }
	int32							GetCurrentUdpSessionCount() const { return _udpSessionCount; }
	int32							GetMaxUdpSessionCount() const { return _maxUdpSessionCount; }

	tcp::endpoint					GetTcpRemoteEndpoint() const { return _config.tcpRemoteEndpoint.value_or(tcp::endpoint(tcp::v4(), 0)); }
	void							SetTcpRemoteEndpoint(const tcp::endpoint& ep) { _config.tcpRemoteEndpoint = ep; }
	udp::endpoint					GetUdpRemoteEndpoint() const { return _config.udpRemoteEndpoint.value_or(udp::endpoint(udp::v4(), 0)); }
	void							SetUdpRemoteEndpoint(const udp::endpoint& ep) { _config.udpRemoteEndpoint = ep; }

	template<typename T>
	void							SetUdpReceiver();

	udp::socket&					GetUdpSocket() const { return _udpReceiver->GetSocket(); }

	ServiceRef						GetServiceRef() { return static_pointer_cast<Service>(shared_from_this()); }
	boost::asio::any_io_executor	GetExecutor() { return _io_context.get_executor(); }

	template<typename TCP, typename UDP>
	void							SetSessionFactory();

	void							RegisterToContextAsync(JobRef job);	//TODO


	//temp
	//uint32					GetUserId() { return _userId; }
	//void					SetUserId(uint32 id) { _userId = id; }

	//TcpSessionRef GetTcpSession() 
	//{ 
	//	if (_tcpSessions.empty())
	//		int a = 0;

	//	return *_tcpSessions.begin(); 
	//}
	//ReliableUdpSessionRef GetUdpSession() 
	//{ 
	//	if (_udpSessions.empty())
	//		int a = 0;

	//	return *_udpSessions.begin(); 
	//}
	//~temp

private:
	USE_LOCK

	TransportConfig										_config;

	boost::asio::io_context								_io_context;


	Set<TcpSessionRef>									_tcpSessions;
	Set<ReliableUdpSessionRef>							_udpSessions;
	unordered_map<udp::endpoint, ReliableUdpSessionRef>	_pendingUdpSessions;

	int32												_tcpSessionCount = 0;
	int32												_maxTcpSessionCount = 0;
	int32												_udpSessionCount = 0;
	int32												_maxUdpSessionCount = 0;


	SessionFactory										_tcpSessionFactory;
	SessionFactory										_udpSessionFactory;

	UdpReceiverRef										_udpReceiver = nullptr;


	//temp
	//uint32 _userId = 0;
	//~temp
};


template<typename TCP, typename UDP>
inline void Service::SetSessionFactory()
{
	_tcpSessionFactory = [this]() -> SessionRef
		{
			auto self = shared_from_this();
			auto session = MakeShared<TCP>(self, self->GetExecutor());
			return static_pointer_cast<Session>(session);
		};

	_udpSessionFactory = [this]() -> SessionRef
		{
			auto self = shared_from_this();
			auto session = MakeShared<UDP>(self, self->GetExecutor());
			return static_pointer_cast<Session>(session);
		};
}

template<typename T>
inline void Service::SetUdpReceiver()
{
	//if (CanCast<T, UdpReceiver>() == false)	// todo: validation check
	//	return;

	_udpReceiver = MakeShared<T>(GetExecutor());
}

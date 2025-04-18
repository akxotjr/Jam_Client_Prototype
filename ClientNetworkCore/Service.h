#pragma once
#include <functional>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <type_traits>
#include <utility>
#include "Job.h"

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
	optional<tcp::endpoint> tcpEndpoint;	// local
	optional<udp::endpoint> udpEndpoint;
};

class Service : public enable_shared_from_this<Service>
{   
public:
	Service(TransportConfig config, int32 maxSTcpSessionCount = 1, int32 maxUdpSessionCount = 1);
	virtual ~Service();

	void							Start();
	bool							CanStart() { return _tcpSessionFactory != nullptr || _udpSessionFactory != nullptr; }

	SessionRef						CreateSession(ProtocolType protocol);

	void							Broadcast(SendBufferRef sendBuffer);

	bool							AddSession(SessionRef session);
	void							ReleaseSession(SessionRef session);

	void							AddTcpSession(TcpSessionRef session);
	void							ReleaseTcpSession(TcpSessionRef session);

	void							AddUdpSession(ReliableUdpSessionRef session);
	void							ReleaseUdpSession(ReliableUdpSessionRef session);


	ReliableUdpSessionRef			FindOrCreateUdpSession(udp::endpoint from);


	int32							GetCurrentTcpSessionCount() { return _tcpSessionCount; }
	int32							GetMaxTcpSessionCount() { return _maxTcpSessionCount; }
	int32							GetCurrentUdpSessionCount() { return _udpSessionCount; }
	int32							GetMaxUdpSessionCount() { return _maxUdpSessionCount; }

	tcp::endpoint					GetTcpEndpoint() { return _config.tcpEndpoint.value_or(tcp::endpoint(tcp::v4(), 0)); }
	void							SetTcpEndpoint(const tcp::endpoint& ep) { _config.tcpEndpoint = ep; }

	udp::endpoint					GetUdpEndpoint() { return _config.udpEndpoint.value_or(udp::endpoint(udp::v4(), 0)); }
	void							SetUdpEndpoint(const udp::endpoint& ep) { _config.udpEndpoint = ep; }


	ServiceRef						GetServiceRef() { return static_pointer_cast<Service>(shared_from_this()); }
	boost::asio::any_io_executor	GetExecutor() { return _io_context.get_executor(); }

	template<typename TCP, typename UDP>
	void							SetSessionFactory();

	void							RegisterToContextAsync(JobRef job);

private:
	USE_LOCK

	TransportConfig						_config;

	boost::asio::io_context				_io_context;

	//<SessionRef>						_sessions;

	Set<TcpSessionRef>									_tcpSessions;
	Set<ReliableUdpSessionRef>							_udpSessions;
	unordered_map<udp::endpoint, ReliableUdpSessionRef>	_pendingUdpSessions;

	int32												_tcpSessionCount = 0;
	int32												_maxTcpSessionCount = 0;
	int32												_udpSessionCount = 0;
	int32												_maxUdpSessionCount = 0;

	//int32								_sessionCount = 0;
	//int32								_maxSessionCount = 1;

	SessionFactory						_tcpSessionFactory;
	SessionFactory						_udpSessionFactory;
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

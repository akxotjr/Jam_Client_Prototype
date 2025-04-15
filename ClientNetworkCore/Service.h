#pragma once
#include <functional>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <type_traits>
#include "Job.h"


using SessionFactory = function<SessionRef()>;

struct NetAddress
{
	string ip;
	string port;
};

class Service : public enable_shared_from_this<Service>
{   
public:
	Service(NetAddress address, int32 maxSessionCount = 1);
	virtual ~Service();

	void								Start();
	//bool								CanStart() { return _sessionFactory != nullptr; }

	SessionRef							CreateSession();

	void								Broadcast(SendBufferRef sendBuffer);

	bool								AddSession(SessionRef session);
	void								ReleaseSession(SessionRef session);

	int32								GetCurrentSessionCount() { return _sessionCount; }
	int32								GetMaxSessionCount() { return _maxSessionCount; }
	NetAddress							GetNetAddress() { return _address; }
	string								GetIpAddress() { return _address.ip; }
	string								GetPort() { return _address.port; }
	ServiceRef							GetServiceRef() { return static_pointer_cast<Service>(shared_from_this()); }
	boost::asio::any_io_executor		GetExecutor() { return _io_context.get_executor(); }

	template<typename T, typename... Args>
	void								SetSessionFactory(Args&&... args);

	void								RegisterToContextAsync(JobRef job);

private:
	USE_LOCK

	NetAddress							_address = {};

	boost::asio::io_context				_io_context;
	//boost::asio::thread_pool			_pool;

	Set<SessionRef>						_sessions;

	int32								_sessionCount = 0;
	int32								_maxSessionCount = 1;
	SessionFactory						_sessionFactory;
};


template<typename T, typename ...Args>
inline void Service::SetSessionFactory(Args && ...args)
{
	auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

	_sessionFactory = [this, argsTuple]() -> SessionRef
		{
			auto self = shared_from_this();
			return std::apply([&](auto&&... unpackedArgs) {
				return MakeShared<T>(self, self->GetExecutor(), std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
				}, argsTuple);
		};
}

#pragma once
#include <functional>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <type_traits>

using SessionFactory = function<SessionRef(ServiceRef, boost::asio::any_io_executor)>;

struct NetAddress
{
	string ip;
	string port;
};

template<typename SessionType>
class SessionIdBuilder
{
public:
	static_assert(std::is_enum_v<SessionType>, "SessionType must be an enum class value");

	SessionIdBuilder() {};
	~SessionIdBuilder() {};

	int32 GenerateId(SessionType sessionType)
	{
		auto typeValue = static_cast<int32>(sessionType);

		if (typeValue == 0)
		{
			return -1;
		}

		if (_typesToCount.find(typeValue) == _typesToCount.end())
		{
			_typesToCount[typeValue] = 0;
		}

		int32 count = ++_typesToCount[typeValue];
		int32 id = typeValue * 1000 + count;

		return id;
	}

private:
	unordered_map<int32, int32> _typesToCount;
};


class Service : public enable_shared_from_this<Service>
{   
public:
	Service(NetAddress address, int32 maxSessionCount = 1);
	virtual ~Service();

	void			Start();
	bool			CanStart() { return _sessionFactory != nullptr; }

	template<typename T>
	bool			CreateSession(int32 id);

	void			Broadcast(SendBufferRef sendBuffer);

	bool			AddSession(SessionFactory factory);
	void			ReleaseSession(SessionRef session);

	SessionRef		GetSessionById(int32 id) { return _sessions[id]; }

	template<typename SessionType>
	SessionRef		GetSessionByType(SessionType type);

	int32			GetCurrentSessionCount() { return _sessionCount; }
	int32			GetMaxSessionCount() { return _maxSessionCount; }

	NetAddress		GetNetAddress() { return _address; }
	string			GetIpAddress() { return _address.ip; }
	string			GetPort() { return _address.port; }

	ServiceRef		GetService() { return static_pointer_cast<Service>(shared_from_this()); }

	boost::asio::any_io_executor GetExecutor() { return _io_context.get_executor(); }

private:
	USE_LOCK

	NetAddress							_address = {};

	boost::asio::io_context				_io_context;
	//boost::asio::thread_pool			_pool;

	//unordered_map<uint32, SessionRef>	_sessions;  // key - session count, value - session ref

	unordered_map<int32, SessionRef>	_sessions; // key - session id, value - session ref

	int32								_sessionCount = 0;
	int32								_maxSessionCount = 1;
	SessionFactory						_sessionFactory;

};

template<typename T>
inline bool Service::CreateSession(int32 id)
{
	if (_sessionCount + 1 > _maxSessionCount)
		return false;

	shared_ptr<T> session = MakeShared<T>(GetService(), GetExecutor());

	if (session == nullptr) return false;

	session->SetId(id);

	WRITE_LOCK
	_sessions[id] = session;	// temp
	_sessionCount++;

	return true;
}

template<typename SessionType>
inline SessionRef Service::GetSessionByType(SessionType type)
{
	auto typeValue = static_cast<int32>(type);

	if (typeValue == 0)
	{
		return nullptr;
	}

	for (auto& [id, session] : _sessions)
	{
		if (id % 1000 == typeValue)
		{
			return session;
		}
	}

	return nullptr;
}

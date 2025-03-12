#pragma once
#include <functional>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/any_io_executor.hpp>


using SessionFactory = function<SessionRef(ServiceRef, boost::asio::any_io_executor)>;

struct NetAddress
{
	string ip;
	string port;
};

enum SessionType
{
	NONE = 0,
	GAME_SESSION,
	CHAT_SESSION,

	MAX
};

class SessionIdBuilder
{
public:
	SessionIdBuilder() {};
	~SessionIdBuilder() {};


	int32 GenerateId(SessionType sessionType)
	{
		if (sessionType == SessionType::NONE)
		{
			return 0;
		}

		if (_typesToCount.find(sessionType) == _typesToCount.end())
		{
			_typesToCount[sessionType] = 0;
		}

		int32 count = ++_typesToCount[sessionType];
		int32 id = sessionType * 1000 + count;

		return id;
	}

private:
	unordered_map<SessionType, int32> _typesToCount;
};


class Service : public enable_shared_from_this<Service>
{   
public:
	Service(NetAddress address, int32 maxSessionCount = 1);
	virtual ~Service();

	bool			Start();
	bool			CanStart() { return _sessionFactory != nullptr; }

	void			Broadcast(SendBufferRef sendBuffer);

	//SessionRef		CreateSession();
	bool			AddSession(SessionFactory factory);
	void			ReleaseSession(SessionRef session);

	int32			GetCurrentSessionCount() { return _sessionCount; }
	int32			GetMaxSessionCount() { return _maxSessionCount; }

	NetAddress		GetNetAddress() { return _address; }
	string			GetIpAddress() { return _address.ip; }
	string			GetPort() { return _address.port; }

	boost::asio::any_io_executor GetExecutor() { return _pool.get_executor(); }


	//template<typename T>
	//bool CreateSession()
	//{
	//	if (_sessionCount + 1 > _maxSessionCount)
	//		return false;

	//	shared_ptr<T> session = MakeShared<T>(shared_from_this(), GetExecutor());

	//	if (session == nullptr) return false;

	//	WRITE_LOCK
	//	_sessions.insert(session);
	//	_sessionCount++;

	//	return true;
	//}

private:
	//array<function<void(void)>, static_cast<size_t>(SessionType::COUNT)> SessionBuilder = {
	//	[this]() {CreateSession<class GameSession>(); }
	//};

private:
	USE_LOCK

	NetAddress							_address = {};

	boost::asio::thread_pool			_pool;

	unordered_map<uint32, SessionRef>	_sessions;
	int32								_sessionCount = 0;
	int32								_maxSessionCount = 1;
	SessionFactory						_sessionFactory;

	SessionIdBuilder					_sessionIdBuilder;
};
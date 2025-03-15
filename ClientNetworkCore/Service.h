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

//enum SessionType
//{
//	NONE = 0,
//	GAME_SESSION,
//	CHAT_SESSION,
//
//	MAX
//};


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

	bool			Start();
	bool			CanStart() { return _sessionFactory != nullptr; }

	template<typename T>
	bool CreateSession(int32 id)
	{
		if (_sessionCount + 1 > _maxSessionCount)
			return false;

		shared_ptr<T> session = MakeShared<T>(shared_from_this(), GetExecutor());

		if (session == nullptr) return false;

		session->SetId(id);

		WRITE_LOCK
		_sessions[id] = session;	// temp
		_sessionCount++;

		return true;
	}

	void			Broadcast(SendBufferRef sendBuffer);

	bool			AddSession(SessionFactory factory);
	void			ReleaseSession(SessionRef session);

	int32			GetCurrentSessionCount() { return _sessionCount; }
	int32			GetMaxSessionCount() { return _maxSessionCount; }

	NetAddress		GetNetAddress() { return _address; }
	string			GetIpAddress() { return _address.ip; }
	string			GetPort() { return _address.port; }

	boost::asio::any_io_executor GetExecutor() { return _pool.get_executor(); }

private:
	USE_LOCK

	NetAddress							_address = {};

	boost::asio::thread_pool			_pool;

	//unordered_map<uint32, SessionRef>	_sessions;  // key - session count, value - session ref

	unordered_map<int32, SessionRef> _sessions; // key - session id, value - session ref

	int32								_sessionCount = 0;
	int32								_maxSessionCount = 1;
	SessionFactory						_sessionFactory;

};
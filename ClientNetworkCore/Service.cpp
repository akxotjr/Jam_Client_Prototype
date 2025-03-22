#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

Service::Service(NetAddress address, int32 maxSessionCount)
	: _address(address), _maxSessionCount(maxSessionCount)/*, _pool(4)*/
{
	
}

Service::~Service()
{
	//_pool.join();
	GThreadManager->Join();
}

void Service::Start()
{
	for (int i = 0; i < 4; i++)
	{
		//boost::asio::post(_pool, [this]()
		//	{
		//		_io_context.run();
		//	});

		GThreadManager->Launch([this]() 
			{
				while (true)
				{
					_io_context.run();
				}
			});
	}

	//if (!CanStart()) return false;

	for (auto& session : _sessions)
	{
		session.second->Connect();
	}
}

void Service::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK

	for (auto& session : _sessions)
	{
		session.second->Send(sendBuffer);
	}
}

//SessionRef Service::CreateSession()
//{
//	SessionRef session = _sessionFactory();
//
//	session->Init(shared_from_this());
//
//	return session;
//}

bool Service::AddSession(SessionFactory factory)
{
	//if (_sessionCount + 1 > _maxSessionCount)
	//	return false;

	//SessionRef session = factory(shared_from_this(), _pool.get_executor());

	//if (session == nullptr) return false;

	//WRITE_LOCK
	//_sessions.insert({ _sessionCount, session });
	//_sessionCount++;

	return true;
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	//ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

void Service::RegisterToContextAsync(JobRef job)
{
	boost::asio::post(_io_context, [job]() { job->Execute(); });
}



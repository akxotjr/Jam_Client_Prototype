#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

Service::Service(TransportConfig config, int32 maxSessionCount)
	: _config(config), _maxSessionCount(maxSessionCount)/*, _pool(4)*/
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

	//for (auto& session : _sessions)
	//{
	//	session->Connect();
	//}

	SessionRef session = CreateSession();
	if (session == nullptr)
		return;
	session->Connect();
}

SessionRef Service::CreateSession()
{
	if (GetCurrentSessionCount() + 1 > GetMaxSessionCount())
		return nullptr;

	_sessionCount++;

	SessionRef session = _sessionFactory();
	return session;
}

void Service::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK

	for (auto& session : _sessions)
	{
		session->Send(sendBuffer);
	}
}


bool Service::AddSession(SessionRef session)
{
	WRITE_LOCK
	_sessions.insert(session);
	_sessionCount++;

	cout << "Current Session Count : " << _sessionCount << '\n';

	return true;
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;

	cout << "Current Session Count : " << _sessionCount << '\n';
}

void Service::RegisterToContextAsync(JobRef job)
{
	boost::asio::post(_io_context, [job]() { job->Execute(); });
}



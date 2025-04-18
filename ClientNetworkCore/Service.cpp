#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

Service::Service(TransportConfig config, int32 maxTcpSessionCount, int32 maxUdpSessionCount)
	: _config(config), _maxTcpSessionCount(maxTcpSessionCount), _maxUdpSessionCount(maxUdpSessionCount)
{
}

Service::~Service()
{
	GThreadManager->Join();
}

void Service::Start()
{
	for (int i = 0; i < 4; i++)
	{
		GThreadManager->Launch([this]() 
			{
				while (true)
				{
					_io_context.run();
				}
			});
	}

	SessionRef session = CreateSession(ProtocolType::PROTOCOL_TCP);
	if (session == nullptr)
		return;
	session->Connect();
}

SessionRef Service::CreateSession(ProtocolType protocol)
{
	//if (GetCurrentSessionCount() + 1 > GetMaxSessionCount())
	//	return nullptr;
	SessionRef session = nullptr;

	if (protocol == ProtocolType::PROTOCOL_TCP)
	{
		session = _tcpSessionFactory();
	}
	else if (protocol == ProtocolType::PROTOCOL_UDP)
	{
		session = _udpSessionFactory();
	}

	return session;
}

void Service::Broadcast(SendBufferRef sendBuffer)
{
	//WRITE_LOCK

	//for (auto& session : _sessions)
	//{
	//	session->Send(sendBuffer);
	//}
}


//bool Service::AddSession(SessionRef session)
//{
//	WRITE_LOCK
//	_sessions.insert(session);
//	_sessionCount++;
//
//	cout << "Service::AddSession  Current Session Count : " << _sessionCount << '\n';
//
//	return true;
//}
//
//void Service::ReleaseSession(SessionRef session)
//{
//	WRITE_LOCK
//	ASSERT_CRASH(_sessions.erase(session) != 0);
//	_sessionCount--;
//
//	cout << "Service::ReleaseSession  Current Session Count : " << _sessionCount << '\n';
//}


void Service::AddTcpSession(TcpSessionRef session)
{
	WRITE_LOCK
	_tcpSessionCount++;
	_tcpSessions.insert(session);
}

void Service::ReleaseTcpSession(TcpSessionRef session)
{
	WRITE_LOCK
	ASSERT_CRASH(_tcpSessions.erase(session) != 0);
	_tcpSessionCount--;
}

void Service::AddUdpSession(ReliableUdpSessionRef session)
{
	WRITE_LOCK
	_udpSessionCount++;
	_udpSessions.insert(session);
}

void Service::ReleaseUdpSession(ReliableUdpSessionRef session)
{
	WRITE_LOCK
	ASSERT_CRASH(_udpSessions.erase(session) != 0);
	_udpSessionCount--;
}

ReliableUdpSessionRef Service::FindOrCreateUdpSession(udp::endpoint from)
{
	WRITE_LOCK

	for (auto& session : _udpSessions)
	{
		if (session->GetRemoteEndpoint() == from)
			return session;
	}

	auto it = _pendingUdpSessions.find(from);
	if (it != _pendingUdpSessions.end())
	{
		return it->second;
	}

	auto newSession = static_pointer_cast<ReliableUdpSession>(CreateSession(ProtocolType::PROTOCOL_UDP));
	if (newSession == nullptr)
		return nullptr;

	newSession->GetRemoteEndpoint(from);
	_pendingUdpSessions[from] = newSession;

	return newSession;
}

void Service::RegisterToContextAsync(JobRef job)
{
	boost::asio::post(_io_context, [job]() { job->Execute(); });
}



#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#include "Job.h"

Service::Service(TransportConfig config, int32 maxTcpSessionCount, int32 maxUdpSessionCount)
	: _config(config), _maxTcpSessionCount(maxTcpSessionCount), _maxUdpSessionCount(maxUdpSessionCount)
{
}

Service::~Service()
{
	GThreadManager->Join();
}

bool Service::Start()
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

	if (_udpReceiver == nullptr)
		return false;
	if (_udpReceiver->Start(shared_from_this()) == false)
		return false;

	SessionRef session = CreateSession(ProtocolType::PROTOCOL_TCP);
	if (session == nullptr)
		return false;
	session->Connect();
	return true;
}

SessionRef Service::CreateSession(ProtocolType protocol)
{
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

ReliableUdpSessionRef Service::FindOrCreateUdpSession(udp::endpoint& from)
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

	newSession->SetRemoteEndpoint(from);
	newSession->ProcessConnect();

	_pendingUdpSessions[from] = newSession;

	return newSession;
}

void Service::CompleteUdpHandshake(udp::endpoint& from)
{
	WRITE_LOCK

	auto it = _pendingUdpSessions.find(from);
	if (it != _pendingUdpSessions.end())
	{
		AddUdpSession(it->second);
		//AddSession(it->second);              // 공용 세션 관리에 추가 (선택적)
		_pendingUdpSessions.erase(it);
	}
}

void Service::RegisterToContextAsync(JobRef job)
{
	boost::asio::post(_io_context, [job]() { job->Execute(); });
}



#include "pch.h"
#include "GameSession.h"
#include "ServerPacketHandler.h"
#include "TimeManager.h"


GameSession::GameSession(ServiceRef service, boost::asio::any_io_executor executor)
	: PacketSession(service, executor)
{
}

void GameSession::OnConnected()
{


	{
		Protocol::C_TIMESYNC timesyncPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(timesyncPkt);
		Send(sendBuffer);
	}

	
	TimeManager::GetInstance()->SetSession(GetSessionRef());
	TimeManager::GetInstance()->SetPrevClientTime();

	{
		Protocol::C_LOGIN loginPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
		Send(sendBuffer);
	}
}

void GameSession::OnDisconnected()
{
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();

	// TODO:packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
	
}


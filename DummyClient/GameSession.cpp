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
	Protocol::C_TIMESYNC timesyncPkt;
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(timesyncPkt);
	Send(sendBuffer);
	
	TimeManager::GetInstance()->SetSession(GetSessionRef());
	TimeManager::GetInstance()->SetPrevClientTime();
}

void GameSession::OnDisconnected()
{
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	//PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);



	// TODO:packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
	
}


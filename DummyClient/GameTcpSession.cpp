#include "pch.h"
#include "GameTcpSession.h"
#include "ServerPacketHandler.h"
#include "TimeManager.h"

GameTcpSession::GameTcpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: TcpSession(service, executor)
{

}

void GameTcpSession::OnConnected()
{
	std::cout << "[TCP] OnConnected\n"; // debug

	{
		std::cout << "[TCP] Send : C_SYNC_TIME\n"; // debug
		Protocol::C_SYNC_TIME syncPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(syncPkt);
		Send(sendBuffer);
	}

	TimeManager::Instance().SetSession(GetSessionRef());

	{
		std::cout << "[TCP] Send : C_LOGIN\n"; // debug
		Protocol::C_LOGIN loginPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(loginPkt);
		Send(sendBuffer);
	}
}

void GameTcpSession::OnDisconnected()
{
	std::cout << "[TCP] OnDisconnected\n"; // debug
}

void GameTcpSession::OnSend(int32 len)
{
	//std::cout << "[TCP] OnSend : " << len << " bytes\n"; // debug
}

void GameTcpSession::OnRecv(BYTE* buffer, int32 len)
{
	//std::cout << "[TCP] OnRecv : " << len << " bytes\n"; // debug

	SessionRef session = GetSessionRef();
	ServerPacketHandler::HandlePacket<TcpPacketHeader>(session, buffer, len);
}

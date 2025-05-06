#include "pch.h"
#include "GameTcpSession.h"
#include "SessionManager.h"
#include "ServerPacketHandler.h"
#include "TimeManager.h"

GameTcpSession::GameTcpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: TcpSession(service, executor)
{

}

void GameTcpSession::OnConnected()
{
	SessionManager::Instance().SetTcpSession(static_pointer_cast<GameTcpSession>(shared_from_this()));
	TimeManager::Instance().SetSession(GetSessionRef());		// TODO:?

	std::cout << "[TCP] OnConnected\n"; // debug

	{
		std::cout << "[TCP] Send : C_SYNC_TIME\n"; // debug
		Protocol::C_SYNC_TIME syncPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(syncPkt);
		Send(sendBuffer);
	}


	{
		std::cout << "[TCP] Send : C_LOGIN\n"; // debug
		Protocol::C_LOGIN loginPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(loginPkt);
		Send(sendBuffer);
	}
}

void GameTcpSession::OnDisconnected()
{
	SessionManager::Instance().SetTcpSession(nullptr);
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

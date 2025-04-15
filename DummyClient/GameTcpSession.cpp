#include "pch.h"
#include "GameTcpSession.h"
#include "ServerPacketHandler.h"
#include "TimeManager.h"
#include "ClientService.h"

GameTcpSession::GameTcpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: TcpSession(service, executor)
{

}

void GameTcpSession::OnConnected()
{
	auto service = dynamic_pointer_cast<ClientService>(GetService());
	if (service == nullptr)
		return;

	service->SetGameTcpSession(static_pointer_cast<GameTcpSession>(shared_from_this()));


	//{
	//	Protocol::C_TIMESYNC timesyncPkt;
	//	auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(timesyncPkt);
	//	Send(sendBuffer);
	//}


	//TimeManager::GetInstance()->SetSession(GetSessionRef());

	{
		Protocol::C_LOGIN loginPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(loginPkt);
		Send(sendBuffer);
	}
}

void GameTcpSession::OnDisconnected()
{
	auto service = dynamic_pointer_cast<ClientService>(GetService());
	if (service == nullptr)
		return;

	service->SetGameTcpSession(nullptr);
}

void GameTcpSession::OnSend(int32 len)
{
}

void GameTcpSession::OnRecv(BYTE* buffer, int32 len)
{
	SessionRef session = GetSessionRef();

	ServerPacketHandler::HandlePacket<TcpPacketHeader>(session, buffer, len);
}

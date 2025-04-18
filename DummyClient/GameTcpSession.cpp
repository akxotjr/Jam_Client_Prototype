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
	std::cout << "[TCP] OnConnected\n"; // debug

	auto service = dynamic_pointer_cast<ClientService>(GetService());
	if (service == nullptr)
		return;

	service->SetGameTcpSession(static_pointer_cast<GameTcpSession>(shared_from_this()));

	{
		std::cout << "[TCP] Send : C_TIMESYNC\n"; // debug
		Protocol::C_TIMESYNC timesyncPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(timesyncPkt);
		Send(sendBuffer);
	}


	TimeManager::GetInstance()->SetSession(GetSessionRef());

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

	auto service = dynamic_pointer_cast<ClientService>(GetService());
	if (service == nullptr)
		return;

	service->SetGameTcpSession(nullptr);
}

void GameTcpSession::OnSend(int32 len)
{
	//std::cout << "[TCP] OnSend : " << len << " bytes\n"; // debug
}

void GameTcpSession::OnRecv(BYTE* buffer, int32 len)
{
	//std::cout << "[TCP] OnRecv : " << len << " bytes\n"; // debug

	SessionRef session = static_pointer_cast<Session>(shared_from_this());

	ServerPacketHandler::HandlePacket<TcpPacketHeader>(session, buffer, len);
}

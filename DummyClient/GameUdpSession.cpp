#include "pch.h"
#include "GameUdpSession.h"
#include "ServerPacketHandler.h"
#include "TimeManager.h"
#include "ClientService.h"

GameUdpSession::GameUdpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: ReliableUdpSession(service, executor)
{

}

void GameUdpSession::OnConnected()
{
	auto service = dynamic_pointer_cast<ClientService>(GetService());
	if (service == nullptr)
		return;

	service->SetGameUdpSession(static_pointer_cast<GameUdpSession>(shared_from_this()));
}

void GameUdpSession::OnDisconnected()
{
	auto service = dynamic_pointer_cast<ClientService>(GetService());
	if (service == nullptr)
		return;

	service->SetGameTcpSession(nullptr);
}

void GameUdpSession::OnSend(int32 len)
{
}

void GameUdpSession::OnRecv(BYTE* buffer, int32 len)
{
	SessionRef session = GetSessionRef();

	ServerPacketHandler::HandlePacket<UdpPacketHeader>(session, buffer, len);
}

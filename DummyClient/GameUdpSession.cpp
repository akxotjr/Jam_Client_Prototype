#include "pch.h"
#include "GameUdpSession.h"
#include "ServerPacketHandler.h"

GameUdpSession::GameUdpSession(ServiceRef service, boost::asio::any_io_executor executor)
	: ReliableUdpSession(service, executor)
{

}

void GameUdpSession::OnConnected()
{
	std::cout << "[UDP] OnConnected\n";

	//auto service = static_pointer_cast<ClientService>(GetService());
	//if (service == nullptr)
	//	return;

	//SetConnected(true);
	//service->AddSession(static_pointer_cast<Session>(shared_from_this()));
	//service->SetGameUdpSession(static_pointer_cast<GameUdpSession>(shared_from_this()));
	//service->SetPendingGameUdpSession(nullptr);
}

void GameUdpSession::OnDisconnected()
{
	std::cout << "[UDP] OnDisconnected\n";

	//auto service = static_pointer_cast<ClientService>(GetService());
	//if (service == nullptr)
	//	return;

	//service->SetGameUdpSession(nullptr);
}

void GameUdpSession::OnSend(int32 len)
{
	std::cout << "[UDP] OnSend : " << len << " bytes\n";
}

void GameUdpSession::OnRecv(BYTE* buffer, int32 len)
{
	//std::cout << "[UDP] OnRecv : " << len << " bytes\n";
	

	SessionRef session = GetSessionRef();

	ServerPacketHandler::HandlePacket<UdpPacketHeader>(session, buffer, len);
}

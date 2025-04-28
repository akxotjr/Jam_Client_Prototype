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
}

void GameUdpSession::OnDisconnected()
{
	std::cout << "[UDP] OnDisconnected\n";
}

void GameUdpSession::OnSend(int32 len)
{
	//std::cout << "[UDP] OnSend : " << len << " bytes\n";
}

void GameUdpSession::OnRecv(BYTE* buffer, int32 len)
{
	//std::cout << "[UDP] OnRecv : " << len << " bytes\n";
	

	SessionRef session = GetSessionRef();

	ServerPacketHandler::HandlePacket<UdpPacketHeader>(session, buffer, len);
}

#include "pch.h"
#include "GameUdpReceiver.h"
#include "ServerPacketHandler.h"

GameUdpReceiver::GameUdpReceiver(boost::asio::any_io_executor executor)
	: UdpReceiver(executor)
{
}

GameUdpReceiver::~GameUdpReceiver()
{
}

void GameUdpReceiver::OnRecv(SessionRef& session, BYTE* buffer, int32 len)
{
	ServerPacketHandler::HandlePacket<UdpPacketHeader>(session, buffer, len);
}

#pragma once


class GameSession : public PacketSession
{
public:
	GameSession(ServiceRef service, boost::asio::any_io_executor executor);
	~GameSession() {};

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
};


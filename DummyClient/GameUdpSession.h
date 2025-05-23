#pragma once

class GameUdpSession : public ReliableUdpSession
{
public:
	GameUdpSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~GameUdpSession() override = default;

	/* Session impl */
	virtual void			OnConnected() override;
	virtual void			OnDisconnected() override;
	virtual void			OnSend(int32 len) override;
	virtual void			OnRecv(BYTE* buffer, int32 len) override;
};


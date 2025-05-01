#pragma once

class GameTcpSession : public TcpSession
{
public:
	GameTcpSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~GameTcpSession() override = default;

	/* Session impl */
	virtual void			OnConnected() override;
	virtual void			OnDisconnected() override;
	virtual void			OnSend(int32 len) override;
	virtual void			OnRecv(BYTE* buffer, int32 len) override;
};


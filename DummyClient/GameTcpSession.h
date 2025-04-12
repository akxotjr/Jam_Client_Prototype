#pragma once

class GameTcpSession : public TcpSession
{
public:
	GameTcpSession(ServiceRef service, boost::asio::any_io_executor executor);
	virtual ~GameTcpSession() = default;

	virtual void			OnConnected() override;
	virtual void			OnDisconnected() override;
	virtual void			OnSend(int32 len) override;
	virtual void			OnRecv(BYTE* buffer, int32 len) override;

	uint32					GetId() { return _id; }
	void					SetId(uint32 id) { _id = id; }

private:
	uint32					_id = 0;
};


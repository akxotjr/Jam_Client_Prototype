#pragma once
#include "Service.h"


class ClientService : public Service
{
public:
	ClientService(TransportConfig config, int32 maxSessionCount = 1);
	virtual ~ClientService();

	GameTcpSessionRef		GetGameTcpSession() { return _gameTcpSession; }
	void					SetGameTcpSession(GameTcpSessionRef session) { _gameTcpSession = session; }
	GameUdpSessionRef		GatGameUdpSession() { return _gameUdpSession; }
	void					SetGameUdpSession(GameUdpSessionRef session) { _gameUdpSession = session; }

private:
	GameTcpSessionRef		_gameTcpSession;
	GameUdpSessionRef		_gameUdpSession;
};


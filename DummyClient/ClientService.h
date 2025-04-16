#pragma once
#include "Service.h"


class ClientService : public Service
{
public:
	ClientService(TransportConfig config, int32 maxSessionCount = 2);
	virtual ~ClientService();

	GameTcpSessionRef		GetGameTcpSession() { return _gameTcpSession; }
	void					SetGameTcpSession(GameTcpSessionRef session) { _gameTcpSession = session; }
	GameUdpSessionRef		GetGameUdpSession() { return _gameUdpSession; }
	void					SetGameUdpSession(GameUdpSessionRef session) { _gameUdpSession = session; }
	
	void					SetPendingGameUdpSession(GameUdpSessionRef session) { _pendingGameUdpSession = session; }

private:
	GameTcpSessionRef		_gameTcpSession;
	GameUdpSessionRef		_gameUdpSession;

	GameUdpSessionRef		_pendingGameUdpSession;
};


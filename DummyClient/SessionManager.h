#pragma once

class SessionManager
{
	DECLARE_SINGLETON(SessionManager)

public:
	void				SetTcpSession(GameTcpSessionRef session) { _tcpSession = session; }
	GameTcpSessionRef	GetTcpSession() { return _tcpSession; }

	void				SetUdpSession(GameUdpSessionRef session) { _udpSession = session; }
	GameUdpSessionRef	GetUdpSession() { return _udpSession; }

	void				SetUserId(uint32 id) { _userId = id; }
	uint32&				GetUserId() { return _userId; }

private:
	GameTcpSessionRef	_tcpSession = nullptr;
	GameUdpSessionRef	_udpSession = nullptr;
	uint32				_userId = 0; // temp
};


#include "pch.h"
#include "SessionManager.h"

void SessionManager::Shutdown()
{
	_tcpSession = nullptr;
	_udpSession = nullptr;
}

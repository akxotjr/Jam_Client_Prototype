#include "pch.h"
#include "ClientService.h"

ClientService::ClientService(NetAddress address, int32 maxSessionCount)
	: Service(address, maxSessionCount)
{
}

ClientService::~ClientService()
{
}



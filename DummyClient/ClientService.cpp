#include "pch.h"
#include "ClientService.h"

ClientService::ClientService(TransportConfig config, int32 maxSessionCount)
	: Service(config, maxSessionCount)
{
}

ClientService::~ClientService()
{
}



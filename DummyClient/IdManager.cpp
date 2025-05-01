#include "pch.h"
#include "IdManager.h"

ActorTypePrefix IdManager::GetActorType(uint32 actorId)
{
	return static_cast<ActorTypePrefix>(actorId >> 28);
}

uint32 IdManager::GetActorInstanceId(uint32 actorId)
{
	return actorId & 0x0FFFFFFF;
}

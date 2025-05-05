#pragma once

enum class ActorTypePrefix : uint8
{
	None = 0x0,
	Player = 0x1,
	Monster = 0x2,
	Bot = 0x3,
	Projectile = 0x4,

	COUNT
};

class IdManager
{
public:
	static ActorTypePrefix		GetActorType(uint32 actorId);
	static uint32				GetActorInstanceId(uint32 actorId);
};


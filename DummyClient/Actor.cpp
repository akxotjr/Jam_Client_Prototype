#include "pch.h"
#include "Actor.h"
#include "Scene.h"

Actor::~Actor()
{
}

void Actor::Init(SceneRef scene)
{
	_owner = scene;
}

void Actor::SetTransform(uint64 position, uint64 velocity_speed, uint64 speed)
{

}

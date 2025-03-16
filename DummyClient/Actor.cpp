#include "pch.h"
#include "Actor.h"
#include "Scene.h"

Actor::Actor()
{
}

Actor::~Actor()
{
}

void Actor::Init(shared_ptr<Scene> owner)
{
	// todo : null check
	_owner = owner;
}

void Actor::Update()
{
}

void Actor::Render(HDC hdc)
{
}

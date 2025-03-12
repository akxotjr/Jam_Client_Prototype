#include "pch.h"
#include "Scene.h"
#include "Actor.h"
#include "Character.h"

Scene::Scene()
{
	_player = make_shared<Character>();
}

Scene::~Scene()
{
}

void Scene::Init()
{
	if (_player)
	{
		_player->Init();
	}

	for (auto actor : _otherActors)
	{
		if (actor)
		{
			actor->Init();
		}
	}
}

void Scene::Update()
{
	if (_player)
	{
		_player->Update();
	}

	for (auto actor : _otherActors)
	{
		if (actor)
		{
			actor->Update();
		}
	}
}

void Scene::Render(HDC hdc)
{
	if (_player)
	{
		_player->Render(hdc);
	}

	for (auto actor : _otherActors)
	{
		if (actor)
		{
			actor->Render(hdc);
		}
	}
}

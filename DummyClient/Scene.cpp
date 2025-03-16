#include "pch.h"
#include "Game.h"
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

void Scene::Init(shared_ptr<Game> game)
{
	_game = game;

	if (_player)
	{
		_player->Init(shared_from_this());
	}

	for (auto actor : _otherActors)
	{
		if (actor)
		{
			actor->Init(shared_from_this());
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

SessionRef Scene::GetSessionByType(SessionType type)  
{  
	if (auto game = _game.lock())  
	{  
		return game->GetService()->GetSessionByType(type);  
	}  
	return nullptr;  
}

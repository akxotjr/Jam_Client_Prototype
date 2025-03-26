#include "pch.h"
#include "Game.h"
#include "Scene.h"
#include "Actor.h"
#include "Character.h"
#include "SceneManager.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Init()
{
	for (auto actor : _actors)
	{
		if (actor)
		{
			actor->Init();
			actor->SetScene(shared_from_this());
		}
	}
}

void Scene::Update()
{
	for (auto actor : _actors)
	{
		if (actor)
		{
			actor->Update();
		}
	}
}

void Scene::Render(HDC hdc)
{
	for (auto actor : _actors)
	{
		if (actor)
		{
			actor->Render(hdc);
		}
	}
}

void Scene::AddActor(shared_ptr<Actor> actor)
{
	_actors.push_back(actor);
}

void Scene::RemoveActor(shared_ptr<Actor> actor)
{
	_actors.erase(std::remove(_actors.begin(), _actors.end(), actor), _actors.end());
}

SessionRef Scene::GetSessionByType(SessionType type)  
{  
	if (auto game = SceneManager::GetInstance()->GetGame())  
	{  
		return game->GetService()->GetSessionByType(type);  
	}  
	return nullptr;  
}

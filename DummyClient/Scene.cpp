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
	for (auto& [id, actor] : _actors)
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
	for (auto& [id, actor] : _actors)
	{
		if (actor)
		{
			actor->Update();
		}
	}
}

void Scene::Render(HDC hdc)
{
	for (auto& [id, actor] : _actors)
	{
		if (actor)
		{
			actor->Render(hdc);
		}
	}
}

void Scene::AddActor(uint32 id, shared_ptr<Actor> actor)
{
	_actors[id] = actor;
}

void Scene::RemoveActor(uint32 id)
{
	_actors.erase(id);
}

SessionRef Scene::GetSessionByType(SessionType type)  
{  
	if (auto game = SceneManager::GetInstance()->GetGame())  
	{  
		return game->GetService()->GetSessionByType(type);  
	}  
	return nullptr;  
}

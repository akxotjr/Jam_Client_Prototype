#include "pch.h"
#include "Game.h"
#include "Scene.h"
#include "Actor.h"
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

	//TEMP
	actor->SetScene(shared_from_this());
}

void Scene::RemoveActor(uint32 id)
{
	_actors.erase(id);
}

SessionRef Scene::GetSessionByProtocolType(ProtocolType type)  
{  
	if (auto game = SceneManager::GetInstance()->GetGame())
	{
		auto service = game->GetService();
		if (service == nullptr) return nullptr;

		if (type == ProtocolType::PROTOCOL_TCP)
			return service->GetTcpSession();
		else if (type == ProtocolType::PROTOCOL_UDP)
			return service->GetUdpSession();
	}
	return nullptr;  
}

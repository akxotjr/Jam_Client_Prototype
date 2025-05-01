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
	for (auto& actor : _actors | views::values)
	{
		if (actor)
		{
			actor->Init(shared_from_this());
		}
	}
}

void Scene::Update()
{
	for (auto& actor : _actors | views::values)
	{
		if (actor)
		{
			actor->Update();
		}
	}
}

void Scene::Render(HDC hdc)
{
	for (auto& actor : _actors | views::values)
	{
		if (actor)
		{
			actor->Render(hdc);
		}
	}
}

void Scene::AddActor(ActorRef actor)
{
	actor->Init(shared_from_this());
	_actors.insert({ actor->GetActorId(), actor });
}

void Scene::RemoveActor(uint32 id)
{
	_actors.erase(id);
}

ActorRef Scene::GetActorByActorId(uint32 actorId)
{
	if (_actors.contains(actorId)) 
	{
		return _actors[actorId];
	}
	return nullptr;
}

SessionRef Scene::GetSessionByProtocolType(ProtocolType type)  
{  
	if (auto game = SceneManager::Instance().GetGame())
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

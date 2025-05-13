#include "pch.h"
#include "Scene.h"
#include "Actor.h"

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
		if (actor.get())
		{
			actor->Update();
		}
	}
}

void Scene::Render()
{
	for (auto& actor : _actors | views::values)
	{
		if (actor)
		{
			actor->Render();
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
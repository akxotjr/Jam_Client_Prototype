#pragma once
#include "Game.h"

class Actor;
class Character;
class Session;

class Scene : public enable_shared_from_this<Scene>
{
public:
	Scene();
	virtual ~Scene();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

	void AddActor(uint32 id, shared_ptr<Actor> actor);
	void RemoveActor(uint32 id);

	shared_ptr<Actor> GetActorById(uint32 id) { return _actors[id]; }

	SessionRef GetSessionByType(SessionType type);

	unordered_map<uint32, shared_ptr<Actor>>& GetActors() { return _actors; }

private:
	unordered_map<uint32, shared_ptr<Actor>> _actors;
};


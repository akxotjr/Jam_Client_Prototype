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

	void AddActor(shared_ptr<Actor> actor);
	void RemoveActor(shared_ptr<Actor> actor);

	SessionRef GetSessionByType(SessionType type);

private:
	vector<shared_ptr<Actor>> _actors;
};


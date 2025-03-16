#pragma once
#include "Game.h"

class Actor;
class Character;
class Session;

class Scene : public enable_shared_from_this<Scene>
{
public:
	Scene();
	~Scene();

	virtual void Init(shared_ptr<Game> game);
	virtual void Update();
	virtual void Render(HDC hdc);

	Character* GetPlayer() { return _player.get(); }

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	SessionRef GetSessionByType(SessionType type);

private:
	shared_ptr<Character> _player = nullptr;
	vector<shared_ptr<Actor>> _otherActors;

	weak_ptr<Game> _game;
};


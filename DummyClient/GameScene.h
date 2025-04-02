#pragma once
#include "Scene.h"

class Player;

class GameScene : public Scene
{
	using Super = Scene;

public:
	GameScene();
	virtual ~GameScene();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

	void SetPlayer(shared_ptr<Player> player) { _player = player; }
	shared_ptr<Player> GetPlayer() { return _player; }

private:
	shared_ptr<Player> _player = nullptr;
};


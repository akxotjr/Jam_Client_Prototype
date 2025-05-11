#pragma once
#include "Scene.h"

class Player;

class GameScene : public Scene
{
	using Super = Scene;

public:
	GameScene();
	virtual ~GameScene() override;

	virtual void			Init() override;
	virtual void			Update() override;
	virtual void			Render(/*HDC hdc*/) override;

	void					SetPlayer(const PlayerRef& player) { _player = player; }
	PlayerRef				GetPlayer() { return _player; }

	void OnReceiveServerTime();

private:
	PlayerRef				_player = nullptr;
};


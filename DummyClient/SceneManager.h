#pragma once
#include "Scene.h"

class Game;


class SceneManager
{
	DECLARE_SINGLETON(SceneManager)

public:
	void					Init(shared_ptr<Game> game);
	void					Update();
	void					Render(HDC hdc);

	void					ChangeScene(SceneType sceneType);

	shared_ptr<Scene>		GetCurrentScene() { return _scene; }
	shared_ptr<Game>		GetGame() { return _game; }

private:
	shared_ptr<Scene>		_scene = nullptr;
	SceneType				_sceneType = SceneType::None;

	shared_ptr<Game>		_game;
};


#pragma once

class Service;
class Session;
class Scene;

class Game
{
public:
	Game();
	~Game();

	void					Init();
	void					Update();
	void					Render();

private:
	/* Network */
	ServiceRef				_service;
};


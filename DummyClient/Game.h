#pragma once

class Service;
class Session;
class Scene;

class Game : public enable_shared_from_this<Game>
{
public:
	Game();
	~Game();

	void					Init(/*HWND hwnd*/);
	void					Update();
	void					Render();

private:
	/* Network */
	ServiceRef				_service;
};


#pragma once

class Service;
class Session;
class Scene;

// temp
//enum class SessionType : int32
//{
//	None = 0,
//	GAME_SESSION,
//	CHAT_SESSION,
//
//	COUNT
//};


class Game : public enable_shared_from_this<Game>
{
public:
	Game();
	~Game();

	void					Init(/*HWND hwnd*/);
	void					Update();
	void					Render();

	//ServiceRef				GetService() { return _service; }

private:

	/* Rendering */
	//HWND							_hwnd;
	//HDC								_hdc;
	//RECT							_rect;
	//HDC								_hdcBack = {};
	//HBITMAP							_bmpBack = {};

	/* Network */
	ServiceRef				_service;
};


#pragma once

class Service;
class Session;
class Scene;

// temp
enum class SessionType : int32
{
	None = 0,
	GAME_SESSION,
	CHAT_SESSION,

	COUNT
};


class Game
{
public:
	Game();
	~Game();
	void				Init(HWND hwnd);
	void				Update();
	void				Render();;

private:

	/* Rendering */
	HWND				_hwnd;
	HDC					_hdc;

	RECT				_rect;
	HDC					_hdcBack = {};
	HBITMAP				_bmpBack = {};


	/* Network */
	ServiceRef			_service;
	SessionFactory		_factory;

		
	/* Contents */
	shared_ptr<Scene>	_scene = nullptr;
};


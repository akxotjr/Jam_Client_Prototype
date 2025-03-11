#pragma once

class Service;
class Session;
class SendBuffer;
class BufferWriter;
class ServerPacketHandler;

class Game
{
public:
	Game();
	~Game();
	void			Init(HWND hwnd);
	void			Update();
	void			Render();;

private:
	HWND			_hwnd;
	HDC				_hdc;

	RECT			_rect;
	HDC				_hdcBack = {};
	HBITMAP			_bmpBack = {};

	ServiceRef		_service;
	SessionFactory	_factory;

private:
	//* Character Move *//
	Vec2			_targetPos = {};
	Vec2			_currentPos = {};
	float			_speed = 0.1f;
	Vec2			_dir = { 0, 0 };
};


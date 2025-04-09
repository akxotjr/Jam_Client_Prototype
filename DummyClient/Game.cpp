#include "pch.h"
#include "Game.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "SendBuffer.h"
#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"

#include "boost/asio.hpp"
#include "GameSession.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_hdc = ::GetDC(hwnd);

	::GetClientRect(hwnd, &_rect);

	_hdcBack = ::CreateCompatibleDC(_hdc);
	_bmpBack = ::CreateCompatibleBitmap(_hdc, _rect.right, _rect.bottom);
	HBITMAP prev = (HBITMAP)::SelectObject(_hdcBack, _bmpBack);
	::DeleteObject(prev);

	ServerPacketHandler::Init();

	InputManager::GetInstance()->Init(hwnd);
	TimeManager::GetInstance()->Init();
	SceneManager::GetInstance()->Init(shared_from_this());
	SceneManager::GetInstance()->ChangeScene(SceneType::GameScene);

	_service = make_shared<Service>(NetAddress{ "127.0.0.1", "7777" });
	_service->CreateSession<GameSession>();
	_service->Start();
}

void Game::Update()
{
	InputManager::GetInstance()->Update();
	TimeManager::GetInstance()->Update();
	SceneManager::GetInstance()->Update();
}

void Game::Render()
{
	SceneManager::GetInstance()->Render(_hdcBack);

	{
		// temp
		float rtt = TimeManager::GetInstance()->GetRoundTripTime();
		float clientTime = TimeManager::GetInstance()->GetClientTime();
		wstring str = std::format(L"ClientTime({0})", clientTime);

		Utils::PrintText(_hdcBack, Vec2(10, 10), str);
	}


	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY);
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}

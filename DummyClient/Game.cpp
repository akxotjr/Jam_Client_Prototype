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
#include "Scene.h"

#include "boost/asio.hpp"
#include "GameSession.h"

Game::Game()
{
	_scene = make_shared<Scene>();
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

	InputManager::GetInstance()->Init(hwnd);
	TimeManager::GetInstance()->Init();

	_service = make_shared<Service>(NetAddress{ "127.0.0.1", "7777" });
	SessionIdBuilder<SessionType> idBuilder;
	int32 id = idBuilder.GenerateId(SessionType::GAME_SESSION);
	_service->CreateSession<GameSession>(id);
	//_factory = [this](ServiceRef serviceRef, boost::asio::any_io_executor executor) -> SessionRef {return MakeShared<Session>(serviceRef, executor); };
	//_service->AddSession(_factory);
	/*ASSERT_CRASH(_service->Start());*/
	_service->Start();

    _scene->Init(shared_from_this());
}

void Game::Update()
{
	InputManager::GetInstance()->Update();
	TimeManager::GetInstance()->Update();

	_scene->Update();
}

void Game::Render()
{
	_scene->Render(_hdcBack);

	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY);
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}

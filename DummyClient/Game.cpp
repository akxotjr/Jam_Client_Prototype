#include "pch.h"
#include "Game.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "SendBuffer.h"
#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include "InputManager.h"

#include "boost/asio.hpp"

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

	InputManager::GetInstance()->Init(hwnd);

	_service = make_shared<Service>(NetAddress{ "127.0.0.1", "7777" }, 1);
	_factory = [this](ServiceRef serviceRef, boost::asio::any_io_executor executor) -> SessionRef {return MakeShared<Session>(serviceRef, executor); };

	_service->AddSession(_factory);
	_service->Start();

	_currentPos = { 400, 300 };
}

void Game::Update()
{
	InputManager::GetInstance()->Update();

	if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
	{
		_targetPos = InputManager::GetInstance()->GetMousePos();
		_dir = _targetPos - _currentPos;
		_dir.Normalize();
	}

	Vec2 nextPos = _currentPos + _dir * _speed;
	
	float distanceNext = (nextPos + _targetPos).Length();
	float distanceCurrent = (_currentPos - _targetPos).Length();

	//if (distanceNext > distanceCurrent)
	//{
	//	_currentPos = _targetPos;
	//	return;
	//}
	
	if ((_targetPos - _currentPos).Length() < 1.5f)
	{
		/*_bIsArrived = true;*/
		return;
	}

	_currentPos = nextPos;
}

void Game::Render()
{
	POINT mousePos = InputManager::GetInstance()->GetMousePos();
	//wstring str = std::format(L"Mouse({0}, {1})", mousePos.x, mousePos.y);
	//Utils::PrintText(_hdcBack, mousePos, str);
	Utils::DrawCircle(_hdcBack, _currentPos, 10);


	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY);
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}

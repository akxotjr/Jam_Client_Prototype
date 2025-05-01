#include "pch.h"
#include "Game.h"
#include "Service.h"
#include "ServerPacketHandler.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"

#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "GameUdpReceiver.h"

using boost::asio::ip::address;

Game::Game()
{
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocatorCallback, _errorCallback);
}

Game::~Game()
{
	_foundation->release();
	_foundation = nullptr;
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

	InputManager::Instance().Init(hwnd);
	TimeManager::Instance().Init();
	SceneManager::Instance().Init(shared_from_this());
	SceneManager::Instance().ChangeScene(SceneType::GameScene);

	TransportConfig config = {
		.tcpRemoteEndpoint = tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 7777)
	};

	_service = MakeShared<Service>(config);
	_service->SetSessionFactory<GameTcpSession, GameUdpSession>();
	_service->SetUdpReceiver<GameUdpReceiver>();
	ASSERT_CRASH(_service->Start());
}

void Game::Update()
{
	InputManager::Instance().Update();
	TimeManager::Instance().Update();
	SceneManager::Instance().Update();
}

void Game::Render()
{
	SceneManager::Instance().Render(_hdcBack);

	{
		// temp
		double rtt = TimeManager::Instance().GetRoundTripTime();
		double clientTime = TimeManager::Instance().GetClientTime();
		double deltaTime = TimeManager::Instance().GetDeltaTime();
		wstring str = std::format(L"ClientTime({0}), DeltaTime({1})", clientTime, deltaTime);

		Utils::PrintText(_hdcBack, Vec2(10, 10), str);
	}


	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY);
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}

#include "pch.h"
#include "Game.h"
#include "Renderer.h"
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
}

Game::~Game()
{
	Renderer::Instance().Shutdown();
}

void Game::Init()
{
	Renderer::Instance().Init();

	ServerPacketHandler::Init();

	InputManager::Instance().Init();
	TimeManager::Instance().Init();
	SceneManager::Instance().Init();
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
	//TimeManager::Instance().Update();
	SceneManager::Instance().Update();
}

void Game::Render()
{
	Renderer::Instance().PreRender();
	SceneManager::Instance().Render();
	Renderer::Instance().PostRender();
}

#include "pch.h"
#include "GameScene.h"
#include "RemoteActor.h"
#include "Player.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
	Super::Init();
}

void GameScene::Update()
{
	Super::Update();

	if (_player)
		_player->Update();
}

void GameScene::Render()
{
	Super::Render();

	if (_player)
		_player->Render();
}

void GameScene::OnReceiveServerTime()
{
	for (auto& actor : _actors | views::values)
	{
		if (auto remoteActor = static_pointer_cast<RemoteActor>(actor))
		{
			remoteActor->UpdateInterpolatorBase();
		}
	}
}

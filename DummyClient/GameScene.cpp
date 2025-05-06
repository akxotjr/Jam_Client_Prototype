#include "pch.h"
#include "GameScene.h"

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

void GameScene::Render(/*HDC hdc*/)
{
	Super::Render(/*hdc*/);

	if (_player)
		_player->Render();
}

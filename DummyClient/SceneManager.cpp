#include "pch.h"
#include "SceneManager.h"
#include "GameScene.h"


void SceneManager::Shutdown()
{
	_scene = nullptr;
	_roomList.clear();
}

void SceneManager::Update()
{
	if (_scene)
		_scene->Update();
}

void SceneManager::Render()
{
	if (_scene)
		_scene->Render();
}

void SceneManager::ChangeScene(SceneType sceneType)
{
	if (_sceneType == sceneType) return;

	switch (sceneType)
	{
	case SceneType::GameScene:
		_scene = make_shared<GameScene>();
		break;
	}

	_sceneType = sceneType;

	_scene->Init();
}
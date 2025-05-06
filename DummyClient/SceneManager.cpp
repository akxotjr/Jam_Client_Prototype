#include "pch.h"
#include "SceneManager.h"
#include "GameScene.h"

void SceneManager::Init()
{
}

void SceneManager::Update()
{
	if (_scene)
		_scene->Update();
}

void SceneManager::Render(/*HDC hdc*/)
{
	if (_scene)
		_scene->Render(/*hdc*/);
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
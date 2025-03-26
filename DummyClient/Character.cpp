#include "pch.h"
#include "Character.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Scene.h"
#include "Game.h"
#include "SendBuffer.h"
#include "ServerPacketHandler.h"


Character::Character()
{
}

Character::~Character()
{
}

void Character::Init()
{
	Super::Init();
	//SetPosition(Vec2{ 400, 300 });
}

void Character::Update()
{
	Super::Update();
	interpolator.Update();
	_position = interpolator.GetLastRenderedPosition();

	//UpdateMovement();
}

void Character::Render(HDC hdc)
{
	Super::Render(hdc);

	Utils::DrawCircle(hdc, _position, 10);
}

//void Character::UpdateMovement()
//{
//	//if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
//	//{
//	//	_targetPos = InputManager::GetInstance()->GetMousePos();
//	//	_direction = _targetPos - _position;
//	//	_direction.Normalize();
//	//}
//
//	//float deltaTime = TimeManager::GetInstance()->GetAdjustDeltaTime();
//
//	//Vec2 nextPos = _position + _direction * _speed * deltaTime;
//
//	//if ((_targetPos - _position).Length() < 1.5f)
//	//{
//	//	return;
//	//}
//
//	//_position = nextPos;
//
//	interpolator.Update();
//	_position = interpolator.GetLastRenderedPosition();
//}

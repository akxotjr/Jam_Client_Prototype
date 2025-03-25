#include "pch.h"
#include "Player.h"
#include "InputManager.h"
#include "TimeManager.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	//Super::Init();
}

void Player::Update()
{
	Super::Update();
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);
}

void Player::UpdateMovement()
{
	//if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
	//{
	//	_targetPos = InputManager::GetInstance()->GetMousePos();
	//	_direction = _targetPos - _position;
	//	_direction.Normalize();
	//}

	//float deltaTime = TimeManager::GetInstance()->GetAdjustDeltaTime();

	//Vec2 nextPos = _position + _direction * _speed * deltaTime;

	//if ((_targetPos - _position).Length() < 1.5f)
	//{
	//	return;
	//}

	//_position = nextPos;

	//1. 입력처리
	//2. 서버로 입력 전송
	//3. 서버에서 위치 도착했으면 되새김 처리

	Input input = CaptureInput();
	if (input.keyType == KeyType::None)
	{
		return;
	}

	_pendingInputs.push_back(input);
	ApplyInput(input);

	{
		// send to server
	}
}

Input Player::CaptureInput()
{
	// temp : 현재는 마우스 입력밖에 안받음
	if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
	{
		float timestamp = TimeManager::GetInstance()->GetClientTime();
		KeyType type = KeyType::LeftMouse;
		POINT mousePos = InputManager::GetInstance()->GetMousePos();

		return Input(timestamp, type, mousePos);
	}
	return Input();
}

void Player::ApplyInput(Input& input)
{
	_targetPos = input.mousePosition;
	_direction = _targetPos - _position;
	_direction.Normalize();


	float deltaTime = TimeManager::GetInstance()->GetAdjustDeltaTime();

	Vec2 nextPos = _position + _direction * _speed * deltaTime;

	if ((_targetPos - _position).Length() < 1.5f)
	{
		return;
	}

	_position = nextPos;
}

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
	Super::Init();
}

void Player::Update()
{
	//1. 입력처리
	//2. 서버로 입력 전송
	//3. 서버에서 위치 도착했으면 되새김 처리 (비동기 처리)
	Input input = CaptureInput();
	if (input.keyType != KeyType::None)
	{
		_pendingInputs.push_back(input);
		SendInputToServer(input);
	}

	ApplyInput(input);
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);
}

void Player::SendInputToServer(const Input& input)
{
	// todo
}


Input Player::CaptureInput()
{
	// temp : 현재는 마우스 입력밖에 안받음
	if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
	{
		float timestamp = TimeManager::GetInstance()->GetClientTime();
		KeyType type = KeyType::LeftMouse;
		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		float dt = TimeManager::GetInstance()->GetDeltaTime();

		return Input(timestamp, type, mousePos, _lastCommandSequenceNumber++, dt);
	}
	return Input();
}

void Player::ApplyInput(Input& input)
{
	float deltaTime = input.deltaTime;

	if ((_targetPos - _position).Length() < 1.5f)
	{
		return;
	}


	if (input.keyType != KeyType::None)
	{
		_targetPos = input.mousePosition;
		_direction = _targetPos - _position;
		_direction.Normalize();
	}
	else
	{
		deltaTime = TimeManager::GetInstance()->GetDeltaTime();
	}

	_position = _position + _direction * _speed * input.deltaTime;

}

void Player::Reconcile(Vec2 serverPosition, int32 ackCommandSequenceNumber)
{
	_position = serverPosition;

	// ackCommandSequenceNumber 이후의 입력들만 다시 적용
	Vector<Input> newPending;
	for (auto& input : _pendingInputs)
	{
		if (input.commandSequenceNumber > ackCommandSequenceNumber)
		{
			ApplyInput(input);
			newPending.push_back(input);
		}
	}

	_pendingInputs = newPending;
}

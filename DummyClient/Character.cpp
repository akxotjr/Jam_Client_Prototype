#include "pch.h"
#include "Character.h"
#include "InputManager.h"
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

void Character::Init(shared_ptr<Scene> owner)
{
	Super::Init(owner);
	SetPosition(Vec2{ 400, 300 });
}

void Character::Update()
{
	Super::Update();

	if (auto owner = _owner.lock())
	{
		if (auto session = owner->GetSessionByType(SessionType::GAME_SESSION))
		{
			Protocol::C_CHAT chatPkt;
			chatPkt.set_msg("Hello");
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);

			session->Send(sendBuffer);
		}
	}

	//auto session = owner->GetSessionByType(SessionType::GAME_SESSION);

	UpdateCharacterMovement();
}

void Character::Render(HDC hdc)
{
	Super::Render(hdc);

	Utils::DrawCircle(hdc, _position, 10);
}

void Character::UpdateCharacterMovement()
{
	if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
	{
		_targetPos = InputManager::GetInstance()->GetMousePos();
		_direction = _targetPos - _position;
		_direction.Normalize();
	}

	Vec2 nextPos = _position + _direction * _speed;

	if ((_targetPos - _position).Length() < 1.5f)
	{
		return;
	}

	_position = nextPos;
}

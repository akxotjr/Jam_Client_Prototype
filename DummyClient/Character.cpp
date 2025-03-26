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
}

void Character::Update()
{
	Super::Update();
	interpolator.Update();
	_position = interpolator.GetLastRenderedPosition();
}

void Character::Render(HDC hdc)
{
	Super::Render(hdc);

	Utils::DrawCircle(hdc, _position, 10);
}

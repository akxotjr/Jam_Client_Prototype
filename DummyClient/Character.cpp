#include "pch.h"
#include "Character.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Scene.h"
#include "Game.h"
#include "SendBuffer.h"
//#include "ServerPacketHandler.h"


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

	wstring name;
	name.assign(_name.begin(), _name.end());
	Vec2 pos = _position;
	pos.x -= 20.f;
	pos.y += 15.f;
	Utils::PrintText(hdc, pos, name);


	Utils::DrawRect(hdc, _position, 20, 20);
}

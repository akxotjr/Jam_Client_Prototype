#include "pch.h"
#include "Bot.h"

void Bot::Init()
{
	Super::Init();
}

void Bot::Update()
{
	Super::Update();
}

void Bot::Render(HDC hdc)
{
	//Super::Render(hdc);

	Utils::DrawCircle(hdc, _position, 10);
}

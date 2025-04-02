#pragma once
#include "Character.h"

class Bot : public Character
{
	using Super = Character;

public:
	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);
};


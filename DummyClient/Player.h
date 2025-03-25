#pragma once
#include "Character.h"

struct Input
{
	float timestamp = 0.f;
	KeyType keyType = KeyType::None;
	POINT mousePosition = {};
};

class Player : public Character
{
	using Super = Character;

public:
	Player();
	virtual ~Player();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

private:
	virtual void UpdateMovement() override;

	Input CaptureInput();
	void ApplyInput(Input& input);


	Vector<Input> _pendingInputs;
	int32 _lastAckSequence = 0;
	Vec2 _predictedPosition = {};


};


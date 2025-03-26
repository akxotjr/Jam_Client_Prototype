#pragma once
#include "Character.h"
#include "InputManager.h"

struct Input
{
	Input() {};

	Input(float ts, KeyType key, POINT pos, int seq, float dt)
		: timestamp(ts), keyType(key), mousePosition(pos), commandSequenceNumber(seq), deltaTime(dt) 
	{
	}

	float timestamp = 0.f;
	KeyType keyType = KeyType::None;
	POINT mousePosition = {};
	int32 commandSequenceNumber = 0;
	float deltaTime = 0.f;
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

	void SendInputToServer(const Input& input);

private:
	Input CaptureInput();
	void ApplyInput(Input& input);

	void Reconcile(Vec2 serverPosition, int32 ackSequence);


private:
	Vector<Input>	_pendingInputs;
	int32			_lastCommandSequenceNumber = 0;
	Vec2			_predictedPosition = {};
};


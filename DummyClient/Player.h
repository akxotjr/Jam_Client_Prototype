#pragma once
#include "Character.h"
#include "InputManager.h"

struct Input
{
	Input() {};

	Input(float ts, KeyType key, POINT pos, int seq, float dt)
		: timestamp(ts), keyType(key), mousePosition(pos), sequenceNumber(seq), deltaTime(dt) 
	{
	}

	float timestamp = 0.f;
	KeyType keyType = KeyType::None;
	POINT mousePosition = {};
	uint32 sequenceNumber = 0;
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

	void Reconcile(Vec2 serverPosition, Vec2 serverVelocity, uint32 ackSequenceNumber);

private:
	Input CaptureInput();
	void ApplyInput(Input& input);

private:
	USE_LOCK

	//temp
	KeyState	_prevState = KeyState::None;


private:
	Vector<Input>	_pendingInputs;
	uint32			_lastSequenceNumber = 0;
	Vec2			_predictedPosition = {};
};


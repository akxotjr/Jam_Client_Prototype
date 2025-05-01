#pragma once
#include "Character.h"
#include "InputManager.h"

struct Input
{
	Input(double ts, KeyType key, POINT pos, int seq, double dt)
		: timestamp(ts), keyType(key), mousePosition(pos), sequenceNumber(seq), deltaTime(dt) 
	{
	}

	double timestamp = 0.0;
	KeyType keyType = KeyType::None;
	POINT mousePosition = {};
	uint32 sequenceNumber = 0;
	double deltaTime = 0.f;
};

class Player : public Actor
{
	using Super = Actor;

public:
	Player();
	virtual ~Player() override;

	virtual void		Init(SceneRef scene) override;
	virtual void		Update() override;
	virtual void		Render(HDC hdc) override;

	void				SendInputToServer(const Input& input);

	void				Reconcile(Vec3 serverPosition, Vec3 serverVelocity, uint32 ackSequenceNumber);

private:
	Input				CaptureInput();
	void				ApplyInput(Input& input);

private:
	USE_LOCK
	//temp
	KeyState			_prevState = KeyState::None;

private:
	Vector<Input>		_pendingInputs;
	uint32				_lastSequenceNumber = 0;
	Vec3				_predictedPosition = {};
};


#pragma once
#include "Actor.h"
#include "InputManager.h"


class Player : public Actor
{
	using Super = Actor;

public:
	Player() = default;
	virtual ~Player() override = default;

	/* Actor impl */
	virtual void		Init(SceneRef scene) override;
	virtual void		Update() override;
	virtual void		Render(/*HDC hdc*/) override;

	void				Reconcile(Vec3 serverPosition, Vec3 serverVelocity, uint32 ackSequenceNumber);

private:
	void				SendInputToServer(const Input& input) const;
	void				ApplyInput(const Input& input);

private:
	USE_LOCK

	Vector<Input>		_pendingInputs;
	uint32				_lastSequenceNumber = 0;
	Vec3				_predictedPosition = {};
};


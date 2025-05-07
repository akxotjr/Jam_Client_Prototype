#pragma once
#include "Actor.h"
#include "InputManager.h"


/** 
 *  Player
 * 	1. Input Processing
 *	2. Send Input to Server
 *	3. Reconcile
 */

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

	void				Reconcile(uint64 position, uint64 velocity_speed, uint64 rotation, uint32 ackSequence);

private:
	void				SendInputToServer(const Input& input) const;
	void				ApplyInput(const Input& input);
	void				ProcessKeyField(const uint32& keyField);

private:
	USE_LOCK

	Vector<Input>		_pendingInputs;
	uint32				_lastSequenceNumber = 0;
	Vec3				_predictedPosition = {};
};


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
	virtual void		Render() override;

	void				Reconcile(uint64 position, uint64 velocity_speed, uint32 rotation, uint32 ackSequence);

	Vec3				GetPlayerDirection();

	//temp
	void				Fire(float targetX, float targetY, float targetZ);

private:
	void				SendInputToServer(const Input& input) const;
	void				ApplyInput(const Input& input);
	void				ProcessKeyField(const uint32 keyField);


private:
	USE_LOCK

	Vector<Input>		_pendingInputs;
	uint32				_lastSequenceNumber = 0;
	Vec3				_predictedPosition = {};

	Vec3				_playerDir = {};

	Vec3				_renderPosition = {};
	Vec3				_renderRotation = {};
	Vec3				_renderVelocity = {};

	glm::vec4			_muzzleOffset = { 0.0f, 0.0f, 2.0f, 1.0f };
};


#pragma once
#include "Actor.h"
#include "MovementInterpolator.h"

class Character : public Actor
{
	using Super = Actor;

public:
	Character();
	virtual ~Character();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;


	void SetVelocity(Vec2 velocity) { _velocity = velocity; }
	void SetDirection(Vec2 direction) { _direction = direction; }
	void SetSpeed(float speed)
	{
		_speed = speed;
		_velocity = _direction * _speed;
	}

	void AddSnapshot(Snapshot& snap) { interpolator.AddSnapshot(snap); }

	/*Character Movement*/
	//virtual void UpdateMovement() abstract;

public:
	MovementInterpolator interpolator;	// not pointer?

protected:
	Vec2 _velocity = {};
	Vec2 _direction = {};
	float _speed = 0.f;

	Vec2 _targetPos = {};	//temp
};


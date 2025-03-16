#pragma once
#include "Actor.h"

class Character : public Actor
{
	using Super = Actor;

public:
	Character();
	virtual ~Character();

	virtual void Init(shared_ptr<Scene> owner) override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;


	void SetVelocity(Vec2 velocity) { _velocity = velocity; }
	void SetDirection(Vec2 direction) { _direction = direction; }
	void SetSpeed(float speed)
	{
		_speed = speed;
		_velocity = _direction * _speed;
	}

private:
	/*Character Movement*/
	void UpdateCharacterMovement();

private:
	Vec2 _velocity = {};
	Vec2 _direction = {};
	float _speed = 0.1f;

	Vec2 _targetPos = {};
};


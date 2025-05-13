#pragma once
#include "Actor.h"

class MovementInterpolator;

class RemoteActor : public Actor
{
	using Super = Actor;

public:
	RemoteActor();
	virtual ~RemoteActor() override = default;

	/* Actor impl */
	virtual void		Init(SceneRef scene) override;
	virtual void		Update() override;
	virtual void		Render() override;

	void				UpdateSnapshot(uint64 position, uint64 velocity_speed, uint32 rotation, double timestamp);
	void				UpdateInterpolatorBase();

private:
	unique_ptr<MovementInterpolator>	_interpolator;

	Vec3								_renderPosition;
	Vec3								_renderRotation;
};


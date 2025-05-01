#pragma once
#include "Actor.h"

class MovementInterpolator;

class RemoteActor : public Actor
{
	using Super = Actor;

public:
	RemoteActor();
	virtual ~RemoteActor() override;

	/* Actor impl */
	virtual void		Init(SceneRef scene) override;
	virtual void		Update() override;
	virtual void		Render(HDC hdc) override;

private:
	unique_ptr<MovementInterpolator>	_interpolator;
};


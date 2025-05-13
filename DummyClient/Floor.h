#pragma once
#include "Actor.h"


class Floor : public Actor
{
	using Super = Actor;

public:
	Floor() = default;
	virtual ~Floor() override = default;

	virtual void				Init(SceneRef scene) override;
	virtual void				Update() override;
	virtual void				Render() override;
};


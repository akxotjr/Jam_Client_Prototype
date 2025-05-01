#include "pch.h"
#include "RemoteActor.h"
#include "MovementInterpolator.h"

RemoteActor::RemoteActor()
{
	_interpolator = make_unique<MovementInterpolator>();
}

RemoteActor::~RemoteActor()
{
}

void RemoteActor::Init(SceneRef scene)
{
	Super::Init(scene);
}

void RemoteActor::Update()
{
	_interpolator->Update();
	_position = _interpolator->GetLastRenderedPosition();
}

void RemoteActor::Render(HDC hdc)
{

}





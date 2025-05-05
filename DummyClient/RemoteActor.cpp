#include "pch.h"
#include "RemoteActor.h"
#include "MovementInterpolator.h"
#include "TransformCompressor.h"

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

void RemoteActor::Render(/*HDC hdc*/)
{

}

void RemoteActor::UpdateSnapshot(uint64 position, uint64 velocity_speed, uint64 rotation, double timestamp) const
{
	Vec3 p = {};
	Vec3 v = {};
	Vec3 r = {};
	float s = 0.f;

	TransformCompressor::UnPackPosition(position, p.x, p.y, p.z);
	TransformCompressor::UnpackVelocityAndSpeed(velocity_speed, v.x, v.y, v.z, s);
	TransformCompressor::UnPackRotation(rotation, r.x, r.y, r.z);

	//_moveSpeed = s;

	_interpolator->AddSnapshot(Snapshot{ .timestamp= timestamp, .position= p, .velocity= v, .rotation = r });
}





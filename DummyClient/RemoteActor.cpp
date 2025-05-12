#include "pch.h"
#include "RemoteActor.h"
#include "MovementInterpolator.h"
#include "TransformCompressor.h"
#include "Renderer.h"

RemoteActor::RemoteActor()
{
	_interpolator = make_unique<MovementInterpolator>();
}

void RemoteActor::Init(SceneRef scene)
{
	Super::Init(scene);
}

void RemoteActor::Update()
{
}

void RemoteActor::Render()
{
	_interpolator->Process(_renderPosition, _renderRotation);
	Renderer::Instance().DrawCube(_renderPosition, _renderRotation, Vec3(1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void RemoteActor::UpdateSnapshot(uint64 position, uint64 velocity_speed, uint32 rotation, double timestamp)
{
	TransformCompressor::UnPackPosition(position, _position.x, _position.y, _position.z);
	TransformCompressor::UnpackVelocityAndSpeed(velocity_speed, _velocity.x, _velocity.y, _velocity.z, _moveSpeed);
	TransformCompressor::UnPackRotation(rotation, _rotation.y, _rotation.x);

	_interpolator->AddSnapshot(Snapshot{ .timestamp= timestamp, .position= _position, .velocity= _velocity, .rotation = _rotation });
}

void RemoteActor::UpdateInterpolatorBase()
{
	_interpolator->SetBasedOnServerTime();
}





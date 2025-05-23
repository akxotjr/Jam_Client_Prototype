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
	Renderer::Instance().DrawCube(glm::vec3(_renderPosition.x, _renderPosition.y, _renderPosition.z), glm::vec3(_renderRotation.x, _renderRotation.y, _renderRotation.z), glm::vec3(1.0f, 1.0f, 1.0f), _color);
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





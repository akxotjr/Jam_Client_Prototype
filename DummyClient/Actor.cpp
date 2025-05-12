#include "pch.h"
#include "Actor.h"
#include "Scene.h"
#include "TransformCompressor.h"

Actor::~Actor()
{
}

void Actor::Init(SceneRef scene)
{
	_owner = scene;
}

void Actor::SetTransform(uint64 position, uint64 velocity_speed, uint32 rotation)
{
	TransformCompressor::UnPackPosition(position, _position.x, _position.y, _position.z);
	TransformCompressor::UnpackVelocityAndSpeed(velocity_speed, _velocity.x, _velocity.y, _velocity.z, _moveSpeed);
	TransformCompressor::UnPackRotation(rotation, _rotation.y, _rotation.x);
}

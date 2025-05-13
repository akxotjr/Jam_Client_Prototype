#include "pch.h"
#include "Player.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Session.h"
#include "GameUdpSession.h"
#include "ServerPacketHandler.h"
#include "SessionManager.h"
#include "TransformCompressor.h"


void Player::Init(SceneRef scene)
{
	Super::Init(scene);
}

void Player::Update()
{
	Input input = InputManager::Instance().CaptureInput();

	if (input.timestamp != 0.0)
	{
		input.sequence = _lastSequenceNumber++;

		_pendingInputs.push_back(input);
		SendInputToServer(input);

		ApplyInput(input);
	}
}

void Player::Render()
{
	Renderer::Instance().UpdateCamera(_renderPosition, _renderRotation.y, _renderRotation.x, 5.f);
	Renderer::Instance().DrawCube(glm::vec3(_renderPosition.x, _renderPosition.y, _renderPosition.z), glm::vec3(0.0f, _renderRotation.y, 0.0f), glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}


void Player::SendInputToServer(const Input& input) const
{
	//cout << "[UDP] Send : C_PLAYER_INPUT\n";

	Protocol::C_PLAYER_INPUT pkt;
	pkt.set_sequence(input.sequence);
	pkt.set_keyfield(input.keyField);
	pkt.set_yaw_pitch(TransformCompressor::PackRotation(input.yaw, input.pitch));
	pkt.set_timestamp(input.timestamp);

	auto sendBuffer = ServerPacketHandler::MakeSendBufferUdp(pkt);
	auto session = SessionManager::Instance().GetUdpSession();
	if (session == nullptr)
		return;

	double timestamp = TimeManager::Instance().GetClientTime();

	session->SendReliable(sendBuffer, timestamp);
}

void Player::ApplyInput(const Input& input)
{
	WRITE_LOCK

	ProcessKeyField(input.keyField);

	_renderPosition += _renderVelocity * static_cast<float>(LOGIC_TICK_INTERVAL);
}

void Player::ProcessKeyField(const uint32 keyField)
{
	float dx = 0.f, dz = 0.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveForward)))	// W
		dz = 1.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveBackward)))	// S
		dz = -1.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveLeft)))	// A
		dx = 1.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveRight)))	// D
		dx = -1.f;

	Vec3 dir = { dx, 0.0f, dz };
	if (dir.LengthSquared() > 0.0f)
		dir.Normalize();

    Vec3 rotatedDir = {
        dir.x * cosf(-_rotation.y) - dir.z * sinf(-_rotation.y),
        0.f,
        dir.x * sinf(-_rotation.y) + dir.z * cosf(-_rotation.y)
    };

	WRITE_LOCK

	_renderVelocity.x = rotatedDir.x * _moveSpeed;
	_renderVelocity.z = rotatedDir.z * _moveSpeed;
}

void Player::Reconcile(uint64 position, uint64 velocity_speed, uint32 rotation, uint32 ackSequence)
{
	WRITE_LOCK

	TransformCompressor::UnPackPosition(position, _position.x, _position.y, _position.z);
	TransformCompressor::UnpackVelocityAndSpeed(velocity_speed, _velocity.x, _velocity.y, _velocity.z, _moveSpeed);
	TransformCompressor::UnPackRotation(rotation, _rotation.y, _rotation.x);

	Vec3 delta = _position - _renderPosition;
	if (delta.Length() > SMOOTH_THRESHOLD)
	{
		_renderPosition = _position;
	}
	else
	{
		Vec3 compressedPos = Vec3(
			TransformCompressor::ExpandPosition(TransformCompressor::CompressPosition(_position.x)),
			TransformCompressor::ExpandPosition(TransformCompressor::CompressPosition(_position.y)),
			TransformCompressor::ExpandPosition(TransformCompressor::CompressPosition(_position.z))
		);
		Vec3 error = _position - compressedPos;

		_renderPosition += error;
		_renderPosition = Utils::Lerp<Vec3>(_renderPosition, _position, SMOOTH_FACTOR);
	}

	float yawDiff = fabsf(_rotation.y - _renderRotation.y);
	float pitchDiff = fabsf(_rotation.x - _renderRotation.x);

	if (yawDiff > 1.0f || pitchDiff > 1.0f)
	{
		_renderRotation = _rotation;
	}
	else
	{
		_renderRotation.y = Utils::LerpAngle(_renderRotation.y, _rotation.y, SMOOTH_FACTOR);
		_renderRotation.x = Utils::Lerp<float>(_renderRotation.x, _rotation.x, SMOOTH_FACTOR);
	}

	for (auto& input : _pendingInputs) 
	{
		if (input.sequence > ackSequence)
			ApplyInput(input);
	}

	std::erase_if(_pendingInputs, [&](const Input& i) { return i.sequence <= ackSequence; });
}

Vec3 Player::GetPlayerDirection()
{
	READ_LOCK
	float pitch = _renderRotation.x; 
	float yaw = _renderRotation.y;

	Vec3 dir;
	dir.x = cosf(pitch) * sinf(yaw);
	dir.y = sinf(pitch);
	dir.z = cosf(pitch) * cosf(yaw);

	return dir;
}
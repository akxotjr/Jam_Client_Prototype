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
	}

	ApplyInput(input);
}

void Player::Render()
{
	Renderer::Instance().UpdateCamera(_position, GetPlayerDirection(), 20.f);
	Renderer::Instance().DrawCube(_position, _rotation, Vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}


void Player::SendInputToServer(const Input& input) const
{
	//cout << "[UDP] Send : C_PLAYER_INPUT\n";

	Protocol::C_PLAYER_INPUT pkt;
	pkt.set_sequence(input.sequence);
	pkt.set_keyfield(input.keyField);
	pkt.set_yaw(input.yaw);
	pkt.set_pitch(input.pitch);

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

	_rotation.x = input.pitch;
	_rotation.y = input.yaw;

	ProcessKeyField(input.keyField, input.yaw);

	_position += _velocity * static_cast<float>(LOGIC_TICK_INTERVAL);
}

void Player::ProcessKeyField(const uint32 keyField, const float yaw)
{
	float dx = 0.f, dz = 0.f;
	if (keyField & (1 << 0))	// W
		dz = 1.f;
	if (keyField & (1 << 1))	// S
		dz = -1.f;
	if (keyField & (1 << 2))	// A
		dx = 1.f;
	if (keyField & (1 << 3))	// D
		dx = -1.f;

	Vec3 dir = { dx, 0.0f, dz };
	dir.Normalize();

	Vec3 rotatedDir = {};
	rotatedDir.x = dir.x * cosf(yaw) - dir.z * sinf(yaw);
	rotatedDir.z = dir.x * sinf(yaw) + dir.z * cosf(yaw);

	_velocity.x = rotatedDir.x * _moveSpeed;
	_velocity.z = rotatedDir.z * _moveSpeed;
}

void Player::Reconcile(uint64 position, uint64 velocity_speed, uint32 rotation, uint32 ackSequence)
{
	WRITE_LOCK

	TransformCompressor::UnPackPosition(position, _position.x, _position.y, _position.z);
	TransformCompressor::UnpackVelocityAndSpeed(velocity_speed, _velocity.x, _velocity.y, _velocity.z, _moveSpeed);
	TransformCompressor::UnPackRotation(rotation, _rotation.y, _rotation.x);

	Vector<Input> newPending;
	for (auto& input : _pendingInputs)
	{
		if (input.sequence > ackSequence)
		{
			ApplyInput(input);
			newPending.push_back(input);
		}
	}

	_pendingInputs = newPending;
}

Vec3 Player::GetPlayerDirection()
{
	float pitch = _rotation.x; 
	float yaw = _rotation.y;

	Vec3 dir;
	dir.x = cosf(pitch) * sinf(yaw);
	dir.y = sinf(pitch);
	dir.z = cosf(pitch) * cosf(yaw);

	return dir;
}
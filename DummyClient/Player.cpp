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
	Renderer::Instance().DrawCube(_position, Vec3(0.0f, _yaw, 0.0f), Vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}


void Player::SendInputToServer(const Input& input) const
{
	//cout << "[UDP] Send : C_PLAYER_INPUT\n";

	Protocol::C_PLAYER_INPUT pkt;
	pkt.set_sequence(input.sequence);
	pkt.set_keyfield(input.keyField);

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
	_position += _velocity * static_cast<float>(LOGIC_TICK_INTERVAL);
}

void Player::ProcessKeyField(const uint32& keyField)
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

	_velocity.x = dir.x * _moveSpeed;
	_velocity.z = dir.z * _moveSpeed;
}

void Player::Reconcile(uint64 position, uint64 velocity_speed, uint32 rotation, uint32 ackSequence)
{
	WRITE_LOCK

	TransformCompressor::UnPackPosition(position, _position.x, _position.y, _position.z);
	TransformCompressor::UnpackVelocityAndSpeed(velocity_speed, _velocity.x, _velocity.y, _velocity.z, _moveSpeed);
	TransformCompressor::UnPackRotation(rotation, _yaw, _yawSpeed);

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
	float pitch = InputManager::Instance().GetPitch();
	_yaw = InputManager::Instance().GetYaw();

	Vec3 playerDir;

	playerDir.x = cosf(pitch) * sinf(_yaw);
	playerDir.y = sinf(pitch);
	playerDir.z = cosf(pitch) * cosf(_yaw);

	return playerDir;
}
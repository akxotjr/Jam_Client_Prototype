#include "pch.h"
#include "Player.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Session.h"
#include "GameUdpSession.h"
#include "ServerPacketHandler.h"
#include "SessionManager.h"


void Player::Init(SceneRef scene)
{
	Super::Init(scene);
}

void Player::Update()
{
	//1. Input Processing
	//2. Send Input to Server
	//3. Reconcile

	Input input = InputManager::Instance().CaptureInput();
	if (input.timestamp != 0.0)
	{
		input.sequence = _lastSequenceNumber++;

		_pendingInputs.push_back(input);
		SendInputToServer(input);
	}

	ApplyInput(input);
	Renderer::Instance().UpdateCamera(_position, _rotation, 5.f);
}

void Player::Render(/*HDC hdc*/)
{
	Renderer::Instance().DrawCube(_position, _rotation, Vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void Player::SendInputToServer(const Input& input) const
{
	cout << "[UDP] Send : C_PLAYER_INPUT\n";

	Protocol::C_PLAYER_INPUT pkt;
	pkt.set_timestamp(input.timestamp);
	pkt.set_sequencenumber(input.sequence);
	pkt.set_keyfield(input.keyField);
	pkt.set_deltatime(input.deltaTime);
	pkt.set_mouseposx(input.mousePosition.x);
	pkt.set_mouseposy(input.mousePosition.y);

	auto sendBuffer = ServerPacketHandler::MakeSendBufferUdp(pkt);

	//auto session = static_pointer_cast<GameUdpSession>(_owner.lock()->GetSessionByProtocolType(ProtocolType::PROTOCOL_UDP));
	auto session = SessionManager::Instance().GetUdpSession();
	if (session == nullptr)
		return;

	double timestamp = TimeManager::Instance().GetClientTime();

	session->SendReliable(sendBuffer, timestamp);
}

void Player::ApplyInput(const Input& input)
{
	WRITE_LOCK

	double deltaTime = input.deltaTime;		
	_position += _velocity * static_cast<float>(deltaTime);
}

void Player::Reconcile(Vec3 serverPosition, Vec3 serverVelocity, uint32 ackSequenceNumber)
{
	WRITE_LOCK

	_position = serverPosition;
	_velocity = serverVelocity;

	// ackCommandSequenceNumber 이후의 입력들만 다시 적용
	Vector<Input> newPending;
	for (auto& input : _pendingInputs)
	{
		if (input.sequence > ackSequenceNumber)
		{
			ApplyInput(input);
			newPending.push_back(input);
		}
	}

	_pendingInputs = newPending;
}

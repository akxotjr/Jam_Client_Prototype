#include "pch.h"
#include "Player.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SendBuffer.h"
#include "Session.h"
#include "GameUdpSession.h"
#include "ServerPacketHandler.h"
#include "Scene.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	Super::Init();
}

void Player::Update()
{
	//1. �Է�ó��
	//2. ������ �Է� ����
	//3. �������� ��ġ ���������� �ǻ��� ó�� (�񵿱� ó��)
	Input input = CaptureInput();
	if (input.keyType != KeyType::None)
	{
		_pendingInputs.push_back(input);
		SendInputToServer(input);
	}

	ApplyInput(input);
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);
}

void Player::SendInputToServer(const Input& input)
{
	cout << "[UDP] Send : C_PLAYER_INPUT\n";

	Protocol::C_PLAYER_INPUT pkt;
	pkt.set_timestamp(input.timestamp);
	pkt.set_sequencenumber(input.sequenceNumber);
	pkt.set_keytype(InputManager::GetInstance()->ConvertToProtoKey(input.keyType));
	pkt.set_deltatime(input.deltaTime);
	pkt.set_mouseposx(input.mousePosition.x);
	pkt.set_mouseposy(input.mousePosition.y);

	auto sendBuffer = ServerPacketHandler::MakeSendBufferUdp(pkt);

	auto session = static_pointer_cast<GameUdpSession>(_owner.lock()->GetSessionByProtocolType(ProtocolType::PROTOCOL_UDP));
	if (session == nullptr)
		return;

	float timestamp = TimeManager::GetInstance()->GetClientTime();

	session->SendReliable(sendBuffer, timestamp);
}


Input Player::CaptureInput()
{
	// temp : ����� ���콺 �Է¹ۿ� �ȹ���
	if (InputManager::GetInstance()->GetButton(KeyType::LeftMouse))
	{
		float timestamp = TimeManager::GetInstance()->GetClientTime();
		KeyType type = KeyType::LeftMouse;
		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		float dt = TimeManager::GetInstance()->GetDeltaTime();

		return Input(timestamp, type, mousePos, _lastSequenceNumber++, dt);
	}

	return Input();
}

void Player::ApplyInput(Input& input)
{
	float deltaTime = input.deltaTime;

	if ((_targetPos - _position).Length() < 1.5f)
	{
		return;
	}


	if (input.keyType != KeyType::None)
	{
		_targetPos = input.mousePosition;
		_direction = _targetPos - _position;
		_direction.Normalize();

		_velocity = _direction * _speed;
	}
	else
	{
		deltaTime = TimeManager::GetInstance()->GetDeltaTime();
	}

	_position = _position + _velocity * input.deltaTime;

}

void Player::Reconcile(Vec2 serverPosition, Vec2 serverVelocity, uint32 ackSequenceNumber)
{
	_position = serverPosition;
	_velocity = serverVelocity;
	// ackCommandSequenceNumber ������ �Էµ鸸 �ٽ� ����
	Vector<Input> newPending;
	for (auto& input : _pendingInputs)
	{
		if (input.sequenceNumber > ackSequenceNumber)
		{
			ApplyInput(input);
			newPending.push_back(input);
		}
	}

	_pendingInputs = newPending;
}

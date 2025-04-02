#include "pch.h"
#include "ServerPacketHandler.h"
#include "Job.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"
#include "Service.h"
#include "Player.h"
#include "Bot.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return false;

	Protocol::C_ENTER_GAME enterGamePkt;

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (pkt.success() == false)
		return false;

	Protocol::C_SPAWN_ACTOR spawnActorPkt;

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnActorPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	std::cout << pkt.msg() << std::endl;
	return true;
}

bool Handle_S_TIMESYNC(PacketSessionRef& session, Protocol::S_TIMESYNC& pkt)
{
	float timestamp = pkt.timestamp();
	TimeManager::GetInstance()->OnServerTimeReceived(timestamp);
	return true;
}

bool Handle_S_SPAWN_ACTOR(PacketSessionRef& session, Protocol::S_SPAWN_ACTOR& pkt)
{
	uint32 playerId = pkt.playerid();

	auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());

	const auto& characters = pkt.characterinfo();

	for (int i = 0; i < characters.size(); ++i)
	{
		const Protocol::CharacterInfo& info = characters.Get(i);
		Protocol::CharacterMovementInfo movementInfo = info.movementinfo();

		Protocol::ActorType type = info.type();
		string name = info.name();
		uint32 id = info.id();

		auto actor = gameScene->GetActorById(id);	// 이미 생성되었으면 패스
		if (actor && actor->GetId() == id)
		{
			continue;
		}


		float px = movementInfo.positionx();
		float py = movementInfo.positiony();
		float vx = movementInfo.velocityx();
		float vy = movementInfo.velocityy();
		float speed = movementInfo.speed();

		if (type == Protocol::ActorType::ACTOR_TYPE_BOT)
		{
			shared_ptr<Bot> bot = MakeShared<Bot>();
			bot->SetName(name);
			bot->SetId(id);
			bot->SetPosition(Vec2(px, py));
			bot->SetVelocity(Vec2(vx, vy));
			bot->SetSpeed(speed);

			bot->Init();

			gameScene->AddActor(id, bot);
		}
		else if (type == Protocol::ActorType::ACTOR_TYPE_PLAYER)
		{
			if (!gameScene->GetPlayer() && id == playerId)
			{
				shared_ptr<Player> player = MakeShared<Player>();

				player->SetName(name);
				player->SetId(id);
				player->SetPosition(Vec2(px, py));
				player->SetVelocity(Vec2(vx, vy));
				player->SetSpeed(speed);
				player->Init();

				gameScene->AddActor(id, player);
				gameScene->SetPlayer(player);
			}
			else
			{
				shared_ptr<Character> otherPlayer = MakeShared<Character>();

				otherPlayer->SetName(name);
				otherPlayer->SetId(id);
				otherPlayer->SetPosition(Vec2(px, py));
				otherPlayer->SetVelocity(Vec2(vx, vy));
				otherPlayer->SetSpeed(speed);
				otherPlayer->Init();

				gameScene->AddActor(id, otherPlayer);
			}
		}
	}

	return true;
}

bool Handle_S_CHARACTER_SYNC(PacketSessionRef& session, Protocol::S_CHARACTER_SYNC& pkt)
{
	float timestamp = pkt.timestamp();

	const auto& characters = pkt.characterinfo(); // or pkt.characterinfo(i) per index

	auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());

	for (int i = 0; i < characters.size(); ++i)
	{
		const Protocol::CharacterInfo& info = characters.Get(i);
		
		string name = info.name();
		uint32 id = info.id();

		float px = info.movementinfo().positionx();
		float py = info.movementinfo().positiony();
		float vx = info.movementinfo().velocityx();
		float vy = info.movementinfo().velocityy();

		auto player = gameScene->GetPlayer();
		if (player && id == player->GetId())
		{
			//player->Reconcile(Vec2(px, py);

			continue;
		}

		auto actor = gameScene->GetActorById(id);

		if (actor == nullptr)
			continue;

		auto character = dynamic_pointer_cast<Character>(actor);
		if (character == nullptr)
			continue;

		Snapshot snap;
		snap.timestamp = timestamp;
		snap.position = Vec2(px, py);
		snap.velocity = Vec2(vx, vy);

		character->AddSnapshot(snap);
	}

	return true;
}

bool Handle_S_PLAYER_INPUT(PacketSessionRef& session, Protocol::S_PLAYER_INPUT& pkt)
{
	// todo
	uint32 sequenceNumber = pkt.sequencenumber();

	auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());
	if (!gameScene)
		return false;

	auto player = gameScene->GetPlayer();
	if (!player)
		return false;

	auto& info = pkt.characterinfo();
	uint32 id = info.id();

	if (player->GetId() != id)
		return false;

	float px = info.movementinfo().positionx();
	float py = info.movementinfo().positiony();
	float vx = info.movementinfo().velocityx();
	float vy = info.movementinfo().velocityy();

	player->Reconcile(Vec2(px, py), Vec2(vx, vy), sequenceNumber);

	return true;
}

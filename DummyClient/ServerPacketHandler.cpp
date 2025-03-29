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

	//if (pkt.actortype() == Protocol::ACTOR_TYPE_PLAYER)
	//{
	//	auto scene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());
	//	Protocol::CharacterInfo characterInfo = pkt.characterinfo();
	//	Protocol::CharacterMovementInfo movementInfo = characterInfo.movementinfo();

	//	string name = characterInfo.name();
	//	uint32 id = characterInfo.id();

	//	float px = movementInfo.positionx();
	//	float py = movementInfo.positiony();
	//	float vx = movementInfo.velocityx();
	//	float vy = movementInfo.velocityy();

	//	float speed = movementInfo.speed();

	//	shared_ptr<Player> player = MakeShared<Player>();
	//	player->SetName(name);
	//	player->SetId(id);
	//	player->SetPosition(Vec2(px, py));
	//	player->SetVelocity(Vec2(vx, vy));
	//	player->SetSpeed(speed);

	//	player->Init();

	//	scene->AddActor(id, player);
	//	scene->SetPlayer(player);
	//}

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

	//auto service = session->GetService();
	//auto job = MakeShared<Job>(static_pointer_cast<TimeManager>(TimeManager::GetInstance()->shared_from_this()), &TimeManager::OnServerTimeReceived, timestamp);
	//service->RegisterToContextAsync(MakeShared<Job>(TimeManager::GetInstance()->shared_from_this(), &TimeManager::OnServerTimeReceived, timestamp));
	//service->RegisterToContextAsync(MakeShared<Job>([timestamp]() {
	//		TimeManager::GetInstance()->OnServerTimeReceived(timestamp);
	//	}));

	TimeManager::GetInstance()->OnServerTimeReceived(timestamp);
	return true;
}

bool Handle_S_SPAWN_ACTOR(PacketSessionRef& session, Protocol::S_SPAWN_ACTOR& pkt)
{

	auto scene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());

	const auto& characters = pkt.characterinfo();

	for (int i = 0; i < characters.size(); ++i)
	{
		const Protocol::CharacterInfo& info = characters.Get(i);
		Protocol::CharacterMovementInfo movementInfo = info.movementinfo();

		Protocol::ActorType type = info.type();
		string name = info.name();
		uint32 id = info.id();

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

			scene->AddActor(id, bot);
		}
		else if (type == Protocol::ActorType::ACTOR_TYPE_PLAYER)
		{
			shared_ptr<Player> player = MakeShared<Player>();

			player->SetName(name);
			player->SetId(id);
			player->SetPosition(Vec2(px, py));
			player->SetVelocity(Vec2(vx, vy));
			player->SetSpeed(speed);

			player->Init();

			scene->AddActor(id, player);
			scene->SetPlayer(player);
		}
	}

	return true;
}

bool Handle_S_CHARACTER_SYNC(PacketSessionRef& session, Protocol::S_CHARACTER_SYNC& pkt)
{
	float timestamp = pkt.timestamp();

	const auto& characters = pkt.characterinfo(); // or pkt.characterinfo(i) per index

	auto scene = SceneManager::GetInstance()->GetCurrentScene();

	for (int i = 0; i < characters.size(); ++i)
	{
		const Protocol::CharacterInfo& info = characters.Get(i);

		string name = info.name();
		uint32 id = info.id();
		
		float px = info.movementinfo().positionx();
		float py = info.movementinfo().positiony();
		float vx = info.movementinfo().velocityx();
		float vy = info.movementinfo().velocityy();

		auto actor = scene->GetActorById(id);

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


	return true;
}

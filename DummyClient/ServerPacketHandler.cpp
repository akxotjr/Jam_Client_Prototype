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

	if (pkt.actortype() == Protocol::ACTOR_TYPE_PLAYER)
	{
		auto scene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());
		Protocol::CharacterMovementInfo movementInfo = pkt.characterinfo().movementinfo();

		float px = movementInfo.positionx();
		float py = movementInfo.positiony();
		float vx = movementInfo.velocityx();
		float vy = movementInfo.velocityy();
		// todo : id, name

		shared_ptr<Player> player = MakeShared<Player>();
		player->SetPosition(Vec2(px, py));
		player->SetVelocity(Vec2(vx, vy));
		scene->AddActor(player);
	}

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

	auto service = session->GetService();
	//auto job = MakeShared<Job>(static_pointer_cast<TimeManager>(TimeManager::GetInstance()->shared_from_this()), &TimeManager::OnServerTimeReceived, timestamp);
	//service->RegisterToContextAsync(MakeShared<Job>(TimeManager::GetInstance()->shared_from_this(), &TimeManager::OnServerTimeReceived, timestamp));
	service->RegisterToContextAsync(MakeShared<Job>([timestamp]() {
			TimeManager::GetInstance()->OnServerTimeReceived(timestamp);
		}));
	//TimeManager::GetInstance()->OnServerTimeReceived(timestamp);



	return true;
}

bool Handle_S_SPAWN_ACTOR(PacketSessionRef& session, Protocol::S_SPAWN_ACTOR& pkt)
{
	if (pkt.actortype() == Protocol::ACTOR_TYPE_BOT)
	{
		auto scene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());

		Protocol::CharacterMovementInfo movementInfo = pkt.characterinfo().movementinfo();

		float px = movementInfo.positionx();
		float py = movementInfo.positiony();
		float vx = movementInfo.velocityx();
		float vy = movementInfo.velocityy();

		shared_ptr<Bot> bot = MakeShared<Bot>();
		bot->SetPosition(Vec2(px, py));
		bot->SetVelocity(Vec2(vx, vy));

		scene->AddActor(bot);
	}

	return true;
}

bool Handle_S_CHARACTER_SYNC(PacketSessionRef& session, Protocol::S_CHARACTER_SYNC& pkt)
{
	float timestamp = pkt.timestamp();

	const auto& characters = pkt.characterinfo(); // or pkt.characterinfo(i) per index

	for (int i = 0; i < characters.size(); ++i)
	{
		const Protocol::CharacterInfo& info = characters.Get(i);

		int id = info.id();
		float px = info.movementinfo().positionx();
		float py = info.movementinfo().positiony();
		float vx = info.movementinfo().velocityx();
		float vy = info.movementinfo().velocityy();

		//todo
	}
	return true;
}

bool Handle_S_PLAYER_INPUT(PacketSessionRef& session, Protocol::S_PLAYER_INPUT& pkt)
{
	// todo

	return true;
}

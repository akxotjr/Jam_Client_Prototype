#include "pch.h"
#include "ServerPacketHandler.h"
#include "Job.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"
#include "Service.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "Player.h"
#include "Bot.h"

PacketHandlerFunc GPacketHandler_Tcp[UINT16_MAX];
PacketHandlerFunc GPacketHandler_Udp[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	//TcpPacketHeader* header = reinterpret_cast<TcpPacketHeader*>(buffer);

	return false;
}

bool Handle_S_LOGIN(SessionRef& session, Protocol::S_LOGIN& pkt)
{
	std::cout << "[TCP] Recv : S_LOGIN\n";

	uint32 userId = pkt.userid();
	if (userId == 0)
		return false;
	
	//temp
	session->GetService()->SetUserId(userId);
	//~temp

	Protocol::C_ENTER_GAME enterGamePkt;
	auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(enterGamePkt);
	session->Send(sendBuffer);

	std::cout << "[TCP] Send : C_ENTER_GAME\n";	// debug

	return true;
}

bool Handle_S_ENTER_GAME(SessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	std::cout << "[TCP] Recv : S_ENTER_GAME\n";
	if (pkt.success() == false)
		return false;

	string ip = pkt.ip();
	uint32 port = pkt.port();

	auto service = session->GetService();
	service->SetUdpRemoteEndpoint(udp::endpoint(boost::asio::ip::make_address(ip), port));

	auto udpSession = static_pointer_cast<GameUdpSession>(service->CreateSession(ProtocolType::PROTOCOL_UDP));

	{
		float timestamp = TimeManager::Instance().GetClientTime();
		uint32 userId = service->GetUserId();

		Protocol::C_HANDSHAKE handshakePkt;
		handshakePkt.set_userid(userId);
		auto sendBuffer = ServerPacketHandler::MakeSendBufferUdp(handshakePkt);
		udpSession->SendReliable(sendBuffer, timestamp);

		std::cout << "[UDP] Send : C_HANDSHAKE\n";	// debug
	}
	return true;
}

bool Handle_S_ACK(SessionRef& session, Protocol::S_ACK& pkt)
{
	std::cout << "[UDP] Recv : S_ACK\n";

	auto udpSession = static_pointer_cast<GameUdpSession>(session);
	if (udpSession == nullptr)
		return false;

	uint16 latestSeq = pkt.latestsequence();
	uint32 bitfield = pkt.bitfield();

	udpSession->HandleAck(latestSeq, bitfield);

	return true;
}

bool Handle_S_HANDSHAKE(SessionRef& session, Protocol::S_HANDSHAKE& pkt)
{
	std::cout << "[UDP] Recv : S_HANDSHAKE\n";

	if (pkt.success() == false)
		return false;

	auto udpSession = static_pointer_cast<GameUdpSession>(session);
	if (udpSession == nullptr)
		return false;

	{
		double timestamp = TimeManager::Instance().GetClientTime();

		Protocol::C_SPAWN_ACTOR spawnActorPkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBufferUdp(spawnActorPkt);
		udpSession->SendReliable(sendBuffer, timestamp);
	}

	return true;
}

bool Handle_S_CHAT(SessionRef& session, Protocol::S_CHAT& pkt)
{
	std::cout << pkt.msg() << std::endl;
	return true;
}

bool Handle_S_SYNC_TIME(SessionRef& session, Protocol::S_SYNC_TIME& pkt)
{
	//std::cout << "[TCP] Recv : S_SYNC_TIME\n";

	float timestamp = pkt.timestamp();
	TimeManager::Instance().OnServerTimeReceived(timestamp);
	return true;
}

bool Handle_S_SPAWN_ACTOR(SessionRef& session, Protocol::S_SPAWN_ACTOR& pkt)
{
	const auto& actors = pkt.actorinfo();

	auto scene = static_pointer_cast<GameScene>(SceneManager::Instance().GetCurrentScene());

	for (int32 i = 0; i < actors.size(); ++i)
	{
		auto& info = actors.Get(i);
		uint32 actorId = info.id();

		auto& transform = info.transform();

		uint64 position = transform.position();
		uint64 velocity_speed = transform.velocity_speed();
		uint64 rotation = transform.rotation();

		if (scene->GetActorByActorId(actorId))
			continue;

		ActorRef actor = MakeShared<Actor>();
		actor->SetActorId(actorId);
		actor->SetTransform(position, velocity_speed, rotation);

		scene->AddActor(actor);
	}

	//std::cout << "[UDP] Recv : S_SPAWN_ACTOR\n";

	//uint32 playerId = pkt.playerid();

	//auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());

	//const auto& characters = pkt.characterinfo();

	//for (int i = 0; i < characters.size(); ++i)
	//{
	//	const Protocol::CharacterInfo& info = characters.Get(i);
	//	Protocol::CharacterMovementInfo movementInfo = info.movementinfo();

	//	Protocol::ActorType type = info.type();
	//	string name = info.name();
	//	uint32 id = info.id();

	//	auto actor = gameScene->GetActorById(id);	
	//	if (actor && actor->GetId() == id)
	//	{
	//		continue;
	//	}


	//	float px = movementInfo.positionx();
	//	float py = movementInfo.positiony();
	//	float vx = movementInfo.velocityx();
	//	float vy = movementInfo.velocityy();
	//	float speed = movementInfo.speed();

	//	if (type == Protocol::ActorType::ACTOR_TYPE_BOT)
	//	{
	//		shared_ptr<Bot> bot = MakeShared<Bot>();
	//		bot->SetName(name);
	//		bot->SetId(id);
	//		bot->SetPosition(Vec2(px, py));
	//		bot->SetVelocity(Vec2(vx, vy));
	//		bot->SetSpeed(speed);

	//		bot->Init();

	//		gameScene->AddActor(id, bot);
	//	}
	//	else if (type == Protocol::ActorType::ACTOR_TYPE_PLAYER)
	//	{
	//		if (!gameScene->GetPlayer() && id == playerId)
	//		{
	//			shared_ptr<Player> player = MakeShared<Player>();

	//			player->SetName(name);
	//			player->SetId(id);
	//			player->SetPosition(Vec2(px, py));
	//			player->SetVelocity(Vec2(vx, vy));
	//			player->SetSpeed(speed);
	//			player->Init();

	//			gameScene->AddActor(id, player);
	//			gameScene->SetPlayer(player);
	//		}
	//		else
	//		{
	//			shared_ptr<Character> otherPlayer = MakeShared<Character>();

	//			otherPlayer->SetName(name);
	//			otherPlayer->SetId(id);
	//			otherPlayer->SetPosition(Vec2(px, py));
	//			otherPlayer->SetVelocity(Vec2(vx, vy));
	//			otherPlayer->SetSpeed(speed);
	//			otherPlayer->Init();

	//			gameScene->AddActor(id, otherPlayer);
	//		}
	//	}
	//}

	return true;
}

bool Handle_S_CHARACTER_SYNC(SessionRef& session, Protocol::S_CHARACTER_SYNC& pkt)
{
	std::cout << "[UDP] Recv : S_CHARACTER_SYNC\n";

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

bool Handle_S_PLAYER_INPUT(SessionRef& session, Protocol::S_PLAYER_INPUT& pkt)
{
	//std::cout << "[UDP] Recv : S_PLAYER_INPUT\n";

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
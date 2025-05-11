#include "pch.h"
#include "ServerPacketHandler.h"
#include "SessionManager.h"
#include "Actor.h"
#include "Job.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"
#include "Service.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "IdManager.h"
#include "Player.h"
#include "RemoteActor.h"


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
		return false;		// TODO : processing when failed login to server

	SessionManager::Instance().SetUserId(userId);

	return true;
}

bool Handle_S_CREATE_ROOM(SessionRef& session, Protocol::S_CREATE_ROOM& pkt)
{
	std::cout << "[TCP] Recv : S_CREATE_ROOM\n";
	if (!pkt.success()) return false;

	const uint32 roomId = pkt.roomid();
	SceneManager::Instance().SetRoomId(roomId);
	SceneManager::Instance().SetIsInRoom(true);

	return true;
}

bool Handle_S_ENTER_ROOM(SessionRef& session, Protocol::S_ENTER_ROOM& pkt)
{
	std::cout << "[TCP] Recv : S_ENTER_ROOM\n";
	if (!pkt.success()) return false;

	const uint32 roomId = pkt.roomid();
	SceneManager::Instance().SetRoomId(roomId);
	SceneManager::Instance().SetIsInRoom(true);

	return true;
}

bool Handle_S_SYNC_ROOMLIST(SessionRef& session, Protocol::S_SYNC_ROOMLIST& pkt)
{
	std::cout << "[TCP] Recv : S_SYNC_ROOMLIST\n";

	const auto& roomList = pkt.roomlist().roominfo();

	unordered_map<uint32, Vector<uint32>> rooms;

	for (int32 i = 0; i < roomList.size(); i++)
	{
		const auto& roomInfo = roomList.Get(i);
		uint32 roomId = roomInfo.roomid();

		Vector<uint32> playerIdList;
		const auto& playerList = roomInfo.playerlist();
		for (int32 j = 0; j < playerList.size(); j++)
		{
			const auto playerId = playerList.Get(j);
			playerIdList.push_back(playerId);
		}

		rooms[roomId] = playerIdList;
	}

	SceneManager::Instance().SetRoomList(rooms);

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
		double timestamp = TimeManager::Instance().GetClientTime();
		uint32 userId = SessionManager::Instance().GetUserId();

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
	//std::cout << "[UDP] Recv : S_ACK\n";

	auto udpSession = static_pointer_cast<GameUdpSession>(session);
	if (udpSession == nullptr)
		return false;

	uint16 latestSeq = static_cast<uint16>(pkt.latestsequence());
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

		std::cout << "[UDP] Send : C_SPAWN_ACTOR\n";
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

	double timestamp = pkt.timestamp();
	TimeManager::Instance().OnServerTimeReceived(timestamp);

	auto gameScene = static_pointer_cast<GameScene>(SceneManager::Instance().GetCurrentScene());
	gameScene->OnReceiveServerTime();

	return true;
}

bool Handle_S_SPAWN_ACTOR(SessionRef& session, Protocol::S_SPAWN_ACTOR& pkt)
{
	std::cout << "[UDP] Recv : S_SPAWN_ACTOR\n";

	SceneManager::Instance().SetIsInGame(true);
	InputManager::Instance().SetMouseVisible(false);

	uint32 playerActorId = pkt.playeractorid();
	const auto& actors = pkt.actorinfo();

	auto gameScene = static_pointer_cast<GameScene>(SceneManager::Instance().GetCurrentScene());

	for (int32 i = 0; i < actors.size(); ++i)
	{
		auto& info = actors.Get(i);

		const uint32 actorId = info.id();
		const Protocol::Transform& transform = info.transform();

		if (gameScene->GetActorByActorId(actorId))	// if actor is already spawned then continue //TODO check player
			continue;

		uint64 position = transform.position();
		uint64 velocity_speed = transform.velocity_speed();
		uint32 rotation = transform.rotation();

		if (actorId == playerActorId)
		{
			PlayerRef player = MakeShared<Player>();
			player->SetActorId(actorId);
			player->SetTransform(position, velocity_speed, rotation);

			gameScene->SetPlayer(player);
		}
		else
		{
			RemoteActorRef remoteActor = MakeShared<RemoteActor>();
			remoteActor->SetActorId(actorId);
			remoteActor->SetTransform(position, velocity_speed, rotation);

			gameScene->AddActor(remoteActor);
		}
	}

	return true;
}

bool Handle_S_SYNC_ACTOR(SessionRef& session, Protocol::S_SYNC_ACTOR& pkt)
{
	//std::cout << "[UDP] Recv : S_SYNC_ACTOR\n";

	const double timestamp = pkt.timestamp();
	const auto& actors = pkt.actorinfo();

	auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::Instance().GetCurrentScene());

	for (int32 i = 0; i < actors.size(); ++i)
	{
		const Protocol::ActorInfo& info = actors.Get(i);

		const uint32 actorId = info.id();
		const Protocol::Transform& transform = info.transform();

		const uint64 position = transform.position();
		const uint64 velocity_speed = transform.velocity_speed();
		const uint32 rotation = transform.rotation();

		auto player = gameScene->GetPlayer();
		if (!player) continue;

		if (actorId == player->GetActorId())
		{
			if (info.has_sequence())
			{
				const uint32 seq = info.sequence();
				player->Reconcile(position, velocity_speed, rotation, seq);
			}
		}
		else
		{
			auto remoteActor = static_pointer_cast<RemoteActor>(gameScene->GetActorByActorId(actorId));
			if (remoteActor != nullptr)
				remoteActor->UpdateSnapshot(position, velocity_speed, rotation, timestamp);
		}
	}

	return true;
}

bool Handle_S_PLAYER_INPUT(SessionRef& session, Protocol::S_PLAYER_INPUT& pkt)
{
	return true;
}
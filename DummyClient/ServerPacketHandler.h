#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
	PKT_C_TIMESYNC = 1006,
	PKT_S_TIMESYNC = 1007,
	PKT_C_SPAWN_ACTOR = 1008,
	PKT_S_SPAWN_ACTOR = 1009,
	PKT_C_CHARACTER_SYNC = 1010,
	PKT_S_CHARACTER_SYNC = 1011,
	PKT_C_PLAYER_INPUT = 1012,
	PKT_S_PLAYER_INPUT = 1013,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN&pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME&pkt);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT&pkt);
bool Handle_S_TIMESYNC(PacketSessionRef& session, Protocol::S_TIMESYNC&pkt);
bool Handle_S_SPAWN_ACTOR(PacketSessionRef& session, Protocol::S_SPAWN_ACTOR&pkt);
bool Handle_S_CHARACTER_SYNC(PacketSessionRef& session, Protocol::S_CHARACTER_SYNC&pkt);
bool Handle_S_PLAYER_INPUT(PacketSessionRef& session, Protocol::S_PLAYER_INPUT&pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_LOGIN> (Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_ENTER_GAME> (Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_CHAT> (Handle_S_CHAT, session, buffer, len); };
		GPacketHandler[PKT_S_TIMESYNC] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_TIMESYNC> (Handle_S_TIMESYNC, session, buffer, len); };
		GPacketHandler[PKT_S_SPAWN_ACTOR] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_SPAWN_ACTOR> (Handle_S_SPAWN_ACTOR, session, buffer, len); };
		GPacketHandler[PKT_S_CHARACTER_SYNC] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_CHARACTER_SYNC> (Handle_S_CHARACTER_SYNC, session, buffer, len); };
		GPacketHandler[PKT_S_PLAYER_INPUT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket <Protocol::S_PLAYER_INPUT> (Handle_S_PLAYER_INPUT, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN&pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME&pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT&pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_TIMESYNC&pkt) { return MakeSendBuffer(pkt, PKT_C_TIMESYNC); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SPAWN_ACTOR&pkt) { return MakeSendBuffer(pkt, PKT_C_SPAWN_ACTOR); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHARACTER_SYNC&pkt) { return MakeSendBuffer(pkt, PKT_C_CHARACTER_SYNC); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PLAYER_INPUT&pkt) { return MakeSendBuffer(pkt, PKT_C_PLAYER_INPUT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T & pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
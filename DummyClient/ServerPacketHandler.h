#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler_Tcp[UINT16_MAX];
extern PacketHandlerFunc GPacketHandler_Udp[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_C_HANDSHAKE = 1005,
	PKT_S_HANDSHAKE = 1006,
	PKT_S_CHAT = 1007,
	PKT_C_TIMESYNC = 1008,
	PKT_S_TIMESYNC = 1009,
	PKT_C_SPAWN_ACTOR = 1010,
	PKT_S_SPAWN_ACTOR = 1011,
	PKT_C_CHARACTER_SYNC = 1012,
	PKT_S_CHARACTER_SYNC = 1013,
	PKT_C_PLAYER_INPUT = 1014,
	PKT_S_PLAYER_INPUT = 1015,
};

// Custom Handlers
bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(SessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_GAME(SessionRef& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_HANDSHAKE(SessionRef& session, Protocol::S_HANDSHAKE& pkt);
bool Handle_S_CHAT(SessionRef& session, Protocol::S_CHAT& pkt);
bool Handle_S_TIMESYNC(SessionRef& session, Protocol::S_TIMESYNC& pkt);
bool Handle_S_SPAWN_ACTOR(SessionRef& session, Protocol::S_SPAWN_ACTOR& pkt);
bool Handle_S_CHARACTER_SYNC(SessionRef& session, Protocol::S_CHARACTER_SYNC& pkt);
bool Handle_S_PLAYER_INPUT(SessionRef& session, Protocol::S_PLAYER_INPUT& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler_Tcp[i] = Handle_INVALID;
			GPacketHandler_Udp[i] = Handle_INVALID;
		}
		GPacketHandler_Tcp[PKT_S_LOGIN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_LOGIN> (Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_LOGIN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_LOGIN> (Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_ENTER_GAME> (Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_ENTER_GAME> (Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_HANDSHAKE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_HANDSHAKE> (Handle_S_HANDSHAKE, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_HANDSHAKE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_HANDSHAKE> (Handle_S_HANDSHAKE, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_CHAT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_CHAT> (Handle_S_CHAT, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_CHAT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_CHAT> (Handle_S_CHAT, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_TIMESYNC] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_TIMESYNC> (Handle_S_TIMESYNC, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_TIMESYNC] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_TIMESYNC> (Handle_S_TIMESYNC, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_SPAWN_ACTOR] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_SPAWN_ACTOR> (Handle_S_SPAWN_ACTOR, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_SPAWN_ACTOR] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_SPAWN_ACTOR> (Handle_S_SPAWN_ACTOR, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_CHARACTER_SYNC] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_CHARACTER_SYNC> (Handle_S_CHARACTER_SYNC, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_CHARACTER_SYNC] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_CHARACTER_SYNC> (Handle_S_CHARACTER_SYNC, session, buffer, len); };
		GPacketHandler_Tcp[PKT_S_PLAYER_INPUT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::S_PLAYER_INPUT> (Handle_S_PLAYER_INPUT, session, buffer, len); };
		GPacketHandler_Udp[PKT_S_PLAYER_INPUT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::S_PLAYER_INPUT> (Handle_S_PLAYER_INPUT, session, buffer, len); };
	}

	template<typename T>
	struct always_false : std::false_type {};

	template<typename HeaderType>
	static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
	{
		HeaderType* header = reinterpret_cast<HeaderType*>(buffer);

		if constexpr (std::is_same_v<HeaderType, TcpPacketHeader>)
		{
			return GPacketHandler_Tcp[header->id](session, buffer, len);
		}
		else if constexpr (std::is_same_v<HeaderType, UdpPacketHeader>)
		{
			return GPacketHandler_Udp[header->id](session, buffer, len);
		}
		else
		{
			ASSERT_CRASH(always_false<HeaderType>::value);
			return false;
		}
	}
	static SendBufferRef MakeSendBufferTcp(Protocol::C_LOGIN& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_LOGIN& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_ENTER_GAME& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_ENTER_GAME& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_CHAT& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_CHAT); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_CHAT& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_CHAT); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_HANDSHAKE& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_HANDSHAKE); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_HANDSHAKE& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_HANDSHAKE); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_TIMESYNC& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_TIMESYNC); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_TIMESYNC& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_TIMESYNC); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_SPAWN_ACTOR& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_SPAWN_ACTOR); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_SPAWN_ACTOR& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_SPAWN_ACTOR); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_CHARACTER_SYNC& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_CHARACTER_SYNC); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_CHARACTER_SYNC& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_CHARACTER_SYNC); }
	static SendBufferRef MakeSendBufferTcp(Protocol::C_PLAYER_INPUT& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_C_PLAYER_INPUT); }
	static SendBufferRef MakeSendBufferUdp(Protocol::C_PLAYER_INPUT& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_C_PLAYER_INPUT); }

private:
	template<typename HeaderType, typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, SessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(HeaderType), len - sizeof(HeaderType)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename HeaderType, typename T>
	static SendBufferRef MakeSendBufferImpl(T& pkt, uint16 pktId, std::optional<uint16> sequence = std::nullopt)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(HeaderType);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		HeaderType* header = reinterpret_cast<HeaderType*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;

		if constexpr(std::is_same_v<HeaderType, UdpPacketHeader>)
		{
			ASSERT_CRASH(sequence.has_value());
			header->sequence = sequence.value();
		}

		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
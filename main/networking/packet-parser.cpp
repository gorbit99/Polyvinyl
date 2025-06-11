#include "packet-parser.h"

static const char* TAG = "PACKETPARSER";

ReceivedPacket PacketParser::parse(PacketContainer&& packet) const {
	ReceivePacketId packetId = packet.take<ReceivePacketId>();

	switch (packetId) {
		using enum ReceivePacketId;
		case HeartBeatReply:
			return parseHeartBeat(std::move(packet));
		case Vibrate:
			return parseVibrate(std::move(packet));
		case Handshake:
			return parseHandshake(std::move(packet));
		default:
			ESP_LOGE(TAG, "Unknown packet: %d", static_cast<uint8_t>(packetId));
			return VibratePacket{};
	}
}

HeartBeatReplyPacket PacketParser::parseHeartBeat(PacketContainer&& packet) const {
	return {};
}

VibratePacket PacketParser::parseVibrate(PacketContainer&& packet) const { return {}; }

HandshakeReplyPacket PacketParser::parseHandshake(PacketContainer&& packet) const {
	return {};
}

#pragma once

#include <esp_log.h>

#include <variant>

#include "packet-container.h"
#include "packets.h"

using ReceivedPacket
	= std::variant<HeartBeatReplyPacket, VibratePacket, HandshakeReplyPacket>;

class PacketParser {
public:
	ReceivedPacket parse(PacketContainer&& packet) const;

private:
	HeartBeatReplyPacket parseHeartBeat(PacketContainer&& packet) const;
	VibratePacket parseVibrate(PacketContainer&& packet) const;
	HandshakeReplyPacket parseHandshake(PacketContainer&& packet) const;
};

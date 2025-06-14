#include "packet-bundle.h"

#include <cstdint>

#include "networking/packet-container.h"
#include "networking/packets.h"

PacketBundle::PacketBundle(uint64_t packetIndex)
	: packet{SendPacketId::Bundle, packetIndex} {}

void PacketBundle::insert(PacketContainer&& innerPacket) {
	uint16_t actualPacketSize = innerPacket.size() - sizeof(uint64_t);
	packet.insert(BigEndian{actualPacketSize});
	packet.insert(innerPacket.data(), sizeof(SendPacketId));
	packet.insert(
		innerPacket.data() + sizeof(SendPacketId) + sizeof(uint64_t),
		actualPacketSize - sizeof(SendPacketId)
	);

	empty = false;
}

const uint8_t* PacketBundle::data() const { return packet.data(); }

size_t PacketBundle::size() const { return packet.size(); }

bool PacketBundle::isEmpty() const { return empty; }

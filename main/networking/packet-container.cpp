#include "packet-container.h"

PacketContainer::PacketContainer(SendPacketId packetId, uint64_t packetIndex) {
	bytes.resize(sizeof(SendPacketId) + sizeof(uint64_t));

	BigEndian<SendPacketId> packetIdBE{packetId};
	memcpy(bytes.data(), &packetIdBE, sizeof(packetIdBE));

	BigEndian<uint64_t> packetIndexBE{packetIndex};
	memcpy(bytes.data() + sizeof(SendPacketId), &packetIndexBE, sizeof(packetIndexBE));
}

PacketContainer::PacketContainer(uint8_t* data, size_t length) {
	bytes.resize(length);
	memcpy(bytes.data(), data, length);
}

size_t PacketContainer::size() const { return bytes.size(); }

const uint8_t* PacketContainer::data() const { return bytes.data(); }

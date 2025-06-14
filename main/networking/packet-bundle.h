#pragma once

#include <cstdint>
#include <vector>

#include "networking/packet-container.h"

class PacketBundle {
public:
	PacketBundle(uint64_t packetIndex);
	void insert(PacketContainer&& innerPacket);
	const uint8_t* data() const;
	size_t size() const;
	bool isEmpty() const;

private:
	PacketContainer packet;
	bool empty = true;
};

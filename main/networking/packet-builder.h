#pragma once

#include <esp_log.h>
#include <esp_wifi.h>

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>

#include "packet-container.h"
#include "packets.h"

class PacketBuilder {
public:
	PacketContainer heartBeat();
	PacketContainer handShake();
	PacketContainer sensorInfo(SensorInfoPacket&& packetData);

private:
	uint64_t getNextPacketIndex();

	uint64_t nextPacketIndex = 0;
};

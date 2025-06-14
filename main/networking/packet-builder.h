#pragma once

#include <esp_log.h>
#include <esp_wifi.h>

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>

#include "networking/packet-bundle.h"
#include "packet-container.h"
#include "packets.h"

class PacketBuilder {
public:
	PacketContainer heartBeat();
	PacketContainer handShake();
	PacketContainer accel(uint8_t sensorId, float accel[3]);
	PacketContainer batteryLevel(float voltage, float percentage);
	PacketContainer sensorInfo(SensorInfoPacket&& packetData);
	PacketContainer rotationData(uint8_t sensorId, float quat[4]);
	PacketContainer signalStrength(uint8_t rssi);
	PacketContainer temperature(uint8_t sensorId, float temperature);

	PacketBundle bundle();

private:
	uint64_t getNextPacketIndex();

	uint64_t nextPacketIndex = 0;
};

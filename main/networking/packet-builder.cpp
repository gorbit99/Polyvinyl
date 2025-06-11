#include "packet-builder.h"

#include <esp_wifi.h>

#include "config/config.h"
#include "networking/packets.h"

uint64_t PacketBuilder::getNextPacketIndex() {
	uint64_t packetIndex = nextPacketIndex;
	nextPacketIndex++;
	return packetIndex;
}

PacketContainer PacketBuilder::heartBeat() { return {SendPacketId::HeartBeat, 0}; }

PacketContainer PacketBuilder::handShake() {
	PacketContainer container{SendPacketId::Handshake, getNextPacketIndex()};

	uint8_t mac[6];
	esp_wifi_get_mac(WIFI_IF_STA, mac);

	container.insert(BigEndian{BOARD_TYPE});
	container.insert(BigEndian<uint32_t>{0});  // Unused sensor type
	container.insert(BigEndian{MCU_TYPE});
	container.insert(BigEndian<uint32_t>{0});  // Unused IMU data
	container.insert(BigEndian<uint32_t>{0});  // Unused IMU data
	container.insert(BigEndian<uint32_t>{0});  // Unused IMU data
	container.insert(BigEndian{PROTOCOL_VERSION});
	container.insert(std::string{"Polyvinyl"});
	container.insert(mac, sizeof(mac));

	return container;
}

PacketContainer PacketBuilder::sensorInfo(SensorInfoPacket&& packetData) {
	PacketContainer container{SendPacketId::SensorInfo, getNextPacketIndex()};
	container.insert(packetData);
	return container;
}

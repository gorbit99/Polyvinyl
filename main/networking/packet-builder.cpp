#include "packet-builder.h"

#include <esp_wifi.h>

#include "config/config.h"
#include "networking/packets.h"

uint64_t PacketBuilder::getNextPacketIndex() {
	uint64_t packetIndex = nextPacketIndex;
	nextPacketIndex++;
	return packetIndex;
}

PacketContainer PacketBuilder::heartBeat() {
	return {SendPacketId::HeartBeat, getNextPacketIndex()};
}

PacketContainer PacketBuilder::handShake() {
	PacketContainer container{SendPacketId::Handshake, 0};

	uint8_t mac[6];
	esp_wifi_get_mac(WIFI_IF_STA, mac);

	container.insert(BigEndian{BOARD_TYPE});
	container.insert(BigEndian<uint32_t>{0});  // Unused sensor type
	container.insert(BigEndian{MCU_TYPE});
	container.insert(BigEndian<uint32_t>{0});  // Unused IMU data
	container.insert(BigEndian<uint32_t>{0});  // Unused IMU data
	container.insert(BigEndian<uint32_t>{0});  // Unused IMU data
	container.insert(BigEndian{PROTOCOL_VERSION});
	// TODO: Replace with firmware version
	container.insert(std::string{"Polyvinyl"});
	container.insert(mac, sizeof(mac));
	container.insert(TRACKER_TYPE);

	return container;
}

PacketContainer PacketBuilder::accel(uint8_t sensorId, float accel[3]) {
	PacketContainer container{SendPacketId::Accel, getNextPacketIndex()};
	container.insert(
		AccelPacket{
			.x = accel[0],
			.y = accel[1],
			.z = accel[2],
			.sensorId = sensorId,
		}
	);
	return container;
}

PacketContainer PacketBuilder::batteryLevel(float voltage, float percentage) {
	PacketContainer container{SendPacketId::BatteryLevel, getNextPacketIndex()};
	container.insert(BatteryLevelPacket{.voltage = voltage, .percentage = percentage});
	return container;
}

PacketContainer PacketBuilder::sensorInfo(SensorInfoPacket&& packetData) {
	PacketContainer container{SendPacketId::SensorInfo, getNextPacketIndex()};
	container.insert(packetData);
	return container;
}

PacketContainer PacketBuilder::rotationData(uint8_t sensorId, float quat[4]) {
	PacketContainer container{SendPacketId::RotationData, getNextPacketIndex()};

	container.insert(
		RotationDataPacket{
			.sensorId = sensorId,
			.dataType = RotationDataType::Normal,
			.x = quat[0],
			.y = quat[1],
			.z = quat[2],
			.w = quat[3],
		}
	);
	return container;
}

PacketContainer PacketBuilder::signalStrength(uint8_t rssi) {
	PacketContainer container{SendPacketId::SignalStrength, getNextPacketIndex()};

	container.insert(
		SignalStrengthPacket{
			.sensorId = 255,
			.signalStrength = rssi,
		}
	);

	return container;
}

PacketContainer PacketBuilder::temperature(uint8_t sensorId, float temperature) {
	PacketContainer container{SendPacketId::Temperature, getNextPacketIndex()};

    container.insert(TemperaturePacket{
        .sensorId = sensorId,
        .temperature = temperature,
    });
    return container;
}

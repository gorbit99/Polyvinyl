
#include "networking/wifi-connection.h"

#include <esp_check.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_netif_types.h>
#include <esp_wifi.h>
#include <esp_wifi_default.h>
#include <esp_wifi_netif.h>
#include <esp_wifi_types_generic.h>
#include <freertos/idf_additions.h>
#include <portmacro.h>
#include <socket.h>

#include <cstdint>
#include <cstdlib>
#include <limits>

#include "lwip/sockets.h"
#include "networking/packet-builder.h"
#include "networking/packet-bundle.h"
#include "networking/packet-parser.h"
#include "peripherals/battery.h"
#include "sdkconfig.h"
#include "sensors/sensor-manager.h"
#include "utils/visitor.h"

static const char* TAG = "WIFICONN";

void WifiConnection::init() {
	createOnlineSendTimers();

	serverSearchTimerHandle = xTimerCreate(
		"server_search",
		1000 * CONFIG_SERVER_DISCOVERY_RATE_SECONDS / portTICK_PERIOD_MS,
		true,
		this,
		serverSearchTimer
	);

	if (!socket.init(
			[&](const uint8_t* data, size_t length, sockaddr_storage&& sourceAddress) {
				PacketContainer rawPacket{data, length};

				handlePacket(std::move(rawPacket), std::move(sourceAddress));
			}
		)) {
		// TODO: handle more gracefully
		return;
	}

	setConnectionStatus(ConnectionStatus::ConnectedToWifi);

	xTimerStart(serverSearchTimerHandle, portMAX_DELAY);

	Battery::getInstance().onBatteryReading([&](float voltage, float level) {
		auto payload = packetBuilder.batteryLevel(voltage, level);
		sendData(payload.data(), payload.size());
	});
}

void WifiConnection::sendData(const uint8_t* data, size_t length) {
	socket.sendData(data, length);
}

void WifiConnection::serverSearchTimer(TimerHandle_t handle) {
	auto* self = reinterpret_cast<WifiConnection*>(pvTimerGetTimerID(handle));

	auto payload = self->packetBuilder.handShake();
	self->sendData(payload.data(), payload.size());
}

void WifiConnection::handlePacket(
	PacketContainer&& rawPacket,
	sockaddr_storage&& sourceAddress
) {
	auto parsedPacket = packetParser.parse(std::move(rawPacket));

	Visitor PacketVisitor{
		[&](HeartBeatReplyPacket heartBeat) {
			auto payload = packetBuilder.heartBeat();
			sendData(payload.data(), payload.size());
		},
		[&](VibratePacket vibrate) { ESP_LOGI(TAG, "Got a vibrate packet"); },
		[&](HandshakeReplyPacket handshake) {
			if (getConnectionStatus() == ConnectionStatus::ConnectedToServer) {
				return;
			}

			assert(sourceAddress.ss_family == AF_INET);

			xTimerStop(serverSearchTimerHandle, portMAX_DELAY);

			sockaddr_in* source = reinterpret_cast<sockaddr_in*>(&sourceAddress);

			auto* address = inet_ntoa(source->sin_addr.s_addr);
			auto port = ntohs(source->sin_port);

			ESP_LOGI(
				TAG,
				"Connected to server at %d.%d.%d.%d:%d",
				address[0],
				address[1],
				address[2],
				address[3],
				port
			);

			startOnlineSendTimers();
			setConnectionStatus(ConnectionStatus::ConnectedToServer);
			socket.setServerAddress(address, port);
		},
	};

	std::visit(PacketVisitor, parsedPacket);
}

void WifiConnection::sensorInfoTimer(TimerHandle_t handle) {
	auto* self = reinterpret_cast<WifiConnection*>(pvTimerGetTimerID(handle));

	auto& sensors = SensorManager::getInstance().getSensors();

	for (uint8_t i = 0; i < sensors.size(); i++) {
		SensorDriver& sensor = sensors[i];

		auto payload = self->packetBuilder.sensorInfo({
			.sensorId = i,
			.sensorStatus = sensor.getStatus(),
			.sensorType = sensor.getType(),
			.sensorConfigData = 0,
			.hasCompletedRestCalibration = sensor.getRestCalibrated(),
			.sensorPosition = sensor.getPosition(),
			.sensorDataType = sensor.getDataType(),
			.tpsCounterAveragedTps = 0,
			.dataCounterAveragedTps = 0,
		});

		self->sendData(payload.data(), payload.size());
	}
}

void WifiConnection::sensorDataTimer(TimerHandle_t handle) {
	auto* self = reinterpret_cast<WifiConnection*>(pvTimerGetTimerID(handle));
	bool sendTemp = self->shouldSendTemp.exchange(false);

	// TODO: assert that bundles are supported by the server
	// They are first class citizens here, not optional
	PacketBundle bundle = self->packetBuilder.bundle();

	auto& sensors = SensorManager::getInstance().getSensors();
	for (uint8_t sensorId = 0; sensorId < sensors.size(); sensorId++) {
		auto& sensor = sensors[sensorId];

		if (sensor.getHasNewFusionState()) {
			float quat[4];
			float accel[3];
			sensor.getFusionState(quat, accel);

			auto payload = self->packetBuilder.rotationData(sensorId, quat);
			bundle.insert(std::move(payload));

			payload = self->packetBuilder.accel(sensorId, accel);
			bundle.insert(std::move(payload));
		}

		if (sendTemp) {
			auto payload
				= self->packetBuilder.temperature(sensorId, sensor.getTemperature());
			bundle.insert(std::move(payload));
		}
	}

	self->sendData(bundle.data(), bundle.size());
}

void WifiConnection::rssiSendTimer(TimerHandle_t handle) {
	auto* self = reinterpret_cast<WifiConnection*>(pvTimerGetTimerID(handle));

	int rssi;
	esp_wifi_sta_get_rssi(&rssi);

	auto payload = self->packetBuilder.signalStrength(static_cast<uint8_t>(rssi));
	self->sendData(payload.data(), payload.size());
}

void WifiConnection::sensorTempFlagTimer(TimerHandle_t handle) {
	auto* self = reinterpret_cast<WifiConnection*>(pvTimerGetTimerID(handle));

	self->shouldSendTemp = true;
}

void WifiConnection::createOnlineSendTimers() {
	assert(onlineSendTimers.size() == 0);

	onlineSendTimers.push_back(xTimerCreate(
		"sensor_info",
		1000 * CONFIG_SENSOR_INFO_RATE_SECONDS / portTICK_PERIOD_MS,
		true,
		this,
		sensorInfoTimer
	));

	onlineSendTimers.push_back(xTimerCreate(
		"sensor_data",
		1000 / CONFIG_SENSOR_DATA_RATE_HZ / portTICK_PERIOD_MS,
		true,
		this,
		sensorDataTimer
	));

	onlineSendTimers.push_back(xTimerCreate(
		"rssi",
		1000 * CONFIG_TRACKER_RSSI_RATE_SECONDS / portTICK_PERIOD_MS,
		true,
		this,
		rssiSendTimer
	));

	onlineSendTimers.push_back(xTimerCreate(
		"temperature",
		1000 / CONFIG_SENSOR_TEMPERATURE_RATE_HZ / portTICK_PERIOD_MS,
		true,
		this,
		sensorTempFlagTimer
	));
}

void WifiConnection::startOnlineSendTimers() {
	assert(onlineSendTimers.size() > 0);

	for (auto timer : onlineSendTimers) {
		xTimerStart(timer, portMAX_DELAY);
	}
}

void WifiConnection::stopOnlineSendTimers() {
	assert(onlineSendTimers.size() > 0);

	for (auto timer : onlineSendTimers) {
		xTimerStop(timer, portMAX_DELAY);
	}
}

#pragma once

#include <esp_event_base.h>
#include <esp_wifi_types_generic.h>
#include <networking/connection.h>

#include <cstdint>
#include <string_view>
#include <vector>

#include "freertos/idf_additions.h"
#include "lwip/sockets.h"
#include "networking/packet-builder.h"
#include "networking/packet-container.h"
#include "networking/packet-parser.h"

class WifiConnection final : public Connection {
public:
	~WifiConnection() override = default;

	void init() override;
	void sendData(const uint8_t* data, size_t length) override;

private:
	static void eventHandler(
		void* userArg,
		esp_event_base_t eventBase,
		int32_t eventId,
		void* eventData
	);

	void setServerAddress(std::string_view address, uint16_t port);
	void handlePacket(PacketContainer&& rawPacket, sockaddr_storage&& sourceAddress);
	void createOnlineSendTasks();
	void stopOnlineSendTasks();

	static void serverSearchTask(void* userArg);
	static void udpReceiveTask(void* userArg);
	static void sensorInfoTask(void* userArg);
	static void sensorDataTask(void* userArg);
	static void rssiSendTask(void* userArg);

	static constexpr std::string_view BROADCAST_ADDRESS = "255.255.255.255";
	static constexpr uint16_t BASE_PORT = 6969;

	size_t connectionRetries = 0;
	EventGroupHandle_t eventGroup;
	sockaddr_in destinationAddress;

	std::vector<TaskHandle_t> onlineSendTasks;
	TaskHandle_t serverSearchTaskHandle = nullptr;
	TaskHandle_t receiveTaskHandle = nullptr;

	PacketParser packetParser;
	PacketBuilder packetBuilder;

	int socketHandle = 0;

	static constexpr auto WIFI_CONNECTED_BIT = BIT(0);
	static constexpr auto WIFI_CONNECTION_FAILED_BIT = BIT(1);
	static constexpr auto RETRY_COUNT = 1u;
};

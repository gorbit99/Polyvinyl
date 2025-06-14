#pragma once

#include <esp_event_base.h>

#include <cstddef>
#include <cstdint>
#include <functional>

#include "freertos/idf_additions.h"
#include "lwip/sockets.h"

class WifiSocket {
public:
	bool init(
		std::function<void(const uint8_t*, size_t, sockaddr_storage&& sourceAddress)>&&
			onReceive
	);
	void sendData(const uint8_t* data, size_t length);
	void setServerAddress(const char* address, uint16_t port);

private:
	static void eventHandler(
		void* userArg,
		esp_event_base_t eventBase,
		int32_t eventId,
		void* eventData
	);
	static void udpReceiveTask(void* userArg);

	int socketHandle = 0;
	EventGroupHandle_t eventGroup;
	size_t connectionRetries = 0;
	sockaddr_in destinationAddress;
	TaskHandle_t receiveTaskHandle;

	std::function<void(const uint8_t*, size_t, sockaddr_storage&&)> onReceive;

	static constexpr auto WIFI_CONNECTED_BIT = BIT(0);
	static constexpr auto WIFI_CONNECTION_FAILED_BIT = BIT(1);

	static constexpr const char* BROADCAST_ADDRESS = "255.255.255.255";
	static constexpr uint16_t BASE_PORT = 6969;
};

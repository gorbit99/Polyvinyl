#include "wifi-socket.h"

#include <esp_check.h>
#include <esp_wifi.h>

#include <limits>
#include <utility>

static const char* TAG = "WIFISOCKET";

bool WifiSocket::init(
	std::function<void(const uint8_t*, size_t, sockaddr_storage&& sourceAddress)>&&
		onReceive
) {
	this->onReceive = onReceive;
	eventGroup = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_netif_init());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t wifiInitConfig = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifiInitConfig));

	esp_event_handler_instance_t anyEventHandler;
	esp_event_handler_instance_t gotIpEventHandler;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT,
		ESP_EVENT_ANY_ID,
		&eventHandler,
		static_cast<void*>(this),
		&anyEventHandler
	));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT,
		IP_EVENT_STA_GOT_IP,
		&eventHandler,
		static_cast<void*>(this),
		&gotIpEventHandler
	));

	wifi_config_t wifiConfig = {.sta = {
        .ssid = CONFIG_HARDCODED_SSID,
        .password = CONFIG_HARDCODED_PASSWORD,
        .threshold = {
            .rssi = std::numeric_limits<int8_t>::min(),
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
        .sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED,
        .sae_h2e_identifier = "",
    }};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));
	ESP_ERROR_CHECK(esp_wifi_start());

	uint8_t mac[6];
	esp_wifi_get_mac(WIFI_IF_STA, mac);

	ESP_LOGI(TAG, "Wi-Fi initialized!");

	EventBits_t bits = xEventGroupWaitBits(
		eventGroup,
		WIFI_CONNECTED_BIT | WIFI_CONNECTION_FAILED_BIT,
		false,
		false,
		portMAX_DELAY
	);

	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "Connected to Wi-Fi successfully!");
	} else if (bits & WIFI_CONNECTION_FAILED_BIT) {
		ESP_LOGI(TAG, "Failed to connect to Wi-Fi!");
		return false;
	} else {
		ESP_LOGI(TAG, "Unexpected event!");
		return false;
	}

	setServerAddress(BROADCAST_ADDRESS, BASE_PORT);
	return true;
}

void WifiSocket::eventHandler(
	void* userArg,
	esp_event_base_t eventBase,
	int32_t eventId,
	void* eventData
) {
	auto* self = static_cast<WifiSocket*>(userArg);

	if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) {
		ESP_LOGI(TAG, "Connecting...");
		esp_wifi_connect();
		return;
	}

	if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED) {
		if (self->connectionRetries < CONFIG_WIFI_CONNECTION_RETRY_COUNT) {
			self->connectionRetries++;
			esp_wifi_connect();
			ESP_LOGI(TAG, "Connection failed, retrying...");
			return;
		}

		xEventGroupSetBits(self->eventGroup, WIFI_CONNECTION_FAILED_BIT);
		return;
	}

	if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) {
		auto* event = static_cast<ip_event_got_ip_t*>(eventData);
		ESP_LOGI(TAG, "Got ip: " IPSTR, IP2STR(&event->ip_info.ip));
		xEventGroupSetBits(self->eventGroup, WIFI_CONNECTED_BIT);
		return;
	}
}

void WifiSocket::sendData(const uint8_t* data, size_t length) {
	auto error = sendto(
		socketHandle,
		data,
		length,
		0,
		reinterpret_cast<sockaddr*>(&destinationAddress),
		sizeof(destinationAddress)
	);
	if (error < 0) {
		ESP_LOGE(TAG, "Error occured during transmission! Error: %s", strerror(errno));
		return;
	}
}

void WifiSocket::setServerAddress(const char* newAddress, uint16_t port) {
	if (receiveTaskHandle) {
		vTaskDelete(receiveTaskHandle);
		receiveTaskHandle = nullptr;
	}

	if (socketHandle > 0) {
		shutdown(socketHandle, 0);
		close(socketHandle);
		socketHandle = 0;
	}

	destinationAddress = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = {
			.s_addr = inet_addr(newAddress),
		},
	};

	socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (socketHandle < 0) {
		ESP_LOGE(TAG, "Couldn't create socket! Error: %d", errno);
	}

	ESP_LOGI(TAG, "Created socket successfully!");

	timeval timeout{
		.tv_sec = 0,
		.tv_usec = 0,
	};
	setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	xTaskCreate(udpReceiveTask, "udp_receive", 4096, this, 5, &receiveTaskHandle);
}

void WifiSocket::udpReceiveTask(void* userArg) {
	auto* self = reinterpret_cast<WifiSocket*>(userArg);
	uint8_t receiveBuffer[128];

	while (true) {
		sockaddr_storage sourceAddress;
		socklen_t addressLength = sizeof(sourceAddress);

		auto length = recvfrom(
			self->socketHandle,
			receiveBuffer,
			sizeof(receiveBuffer),
			0,
			reinterpret_cast<sockaddr*>(&sourceAddress),
			&addressLength
		);

		if (length < 0) {
			ESP_LOGE(TAG, "Failed to receive from socket! Error: %s", strerror(errno));
			abort();
		}

		self->onReceive(
			receiveBuffer,
			static_cast<size_t>(length),
			std::move(sourceAddress)
		);
	}
}

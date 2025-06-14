#pragma once

#include <esp_event_base.h>
#include <esp_wifi_types_generic.h>
#include <networking/connection.h>

#include <atomic>
#include <cstdint>
#include <string_view>
#include <vector>

#include "freertos/idf_additions.h"
#include "lwip/sockets.h"
#include "networking/packet-builder.h"
#include "networking/packet-container.h"
#include "networking/packet-parser.h"
#include "networking/wifi-socket.h"

class WifiConnection final : public Connection {
public:
	~WifiConnection() override = default;

	void init() override;
	void sendData(const uint8_t* data, size_t length) override;

private:
	void handlePacket(PacketContainer&& rawPacket, sockaddr_storage&& sourceAddress);
	void createOnlineSendTimers();
	void startOnlineSendTimers();
	void stopOnlineSendTimers();
	std::vector<TimerHandle_t> onlineSendTimers;
	TimerHandle_t serverSearchTimerHandle = nullptr;

	static void serverSearchTimer(TimerHandle_t handle);
	static void sensorInfoTimer(TimerHandle_t handle);
	static void sensorDataTimer(TimerHandle_t handle);
	static void rssiSendTimer(TimerHandle_t handle);
	static void sensorTempFlagTimer(TimerHandle_t handle);

	std::atomic<bool> shouldSendTemp = false;

	PacketParser packetParser;
	PacketBuilder packetBuilder;
	WifiSocket socket;
};

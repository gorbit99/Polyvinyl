#include <esp_check.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <nvs_flash.h>

#include <memory>

#include "esp_event.h"
#include "interfaces/i2c-driver.h"
#include "networking/connection.h"
#include "networking/wifi-connection.h"
#include "sensors/sensor-driver.h"
#include "sensors/sensor-reader.h"

void setupNvs() {
	auto result = nvs_flash_init();

	if (result == ESP_ERR_NVS_NO_FREE_PAGES
		|| result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());

		result = nvs_flash_init();
	}

	ESP_ERROR_CHECK(result);
}

void setupEventLoop() { ESP_ERROR_CHECK(esp_event_loop_create_default()); }

std::unique_ptr<Connection> connection = std::make_unique<WifiConnection>();

SensorDriver icmDriver;

extern "C" void app_main() {
	setupNvs();
	setupEventLoop();

	// TODO: check return value
	connection->init();

	I2CDriver::getInstance().init();
	SensorReader::getInstance().init();

	icmDriver.init();
}

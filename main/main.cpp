#include <esp_check.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <nvs_flash.h>

#include <memory>

#include "esp_event.h"
#include "freertos/idf_additions.h"
#include "interfaces/i2c-driver.h"
#include "networking/connection.h"
#include "networking/wifi-connection.h"
#include "peripherals/battery.h"
#include "sensors/sensor-driver.h"
#include "sensors/sensor-manager.h"
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

extern "C" void app_main() {
	setupNvs();
	setupEventLoop();

	I2CDriver::getInstance().init();
	SensorReader::getInstance().init();
	SensorManager::getInstance().init();
	ADCDriver::getInstance().init();
	Battery::getInstance().init();

	// TODO: check return value
	connection->init();
}

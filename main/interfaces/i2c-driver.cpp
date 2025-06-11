#include "i2c-driver.h"

#include <esp_check.h>

#include "./config.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "hal/i2c_types.h"

I2CDriver& I2CDriver::getInstance() { return instance; }

void I2CDriver::init() {
	i2c_master_bus_config_t busConfig{
		.i2c_port = 0,
		.sda_io_num = static_cast<gpio_num_t>(CONFIG_SDA_PIN),
		.scl_io_num = static_cast<gpio_num_t>(CONFIG_SCL_PIN),
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority = 0,
	};

	ESP_ERROR_CHECK(i2c_new_master_bus(&busConfig, &busHandle));
}

i2c_master_dev_handle_t I2CDriver::registerDevice(uint8_t deviceId) const {
	i2c_device_config_t deviceConfig{
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = deviceId,
		.scl_speed_hz = I2C_FREQUENCY,
		.scl_wait_us = 0,
	};

	i2c_master_dev_handle_t deviceHandle;
	i2c_master_bus_add_device(busHandle, &deviceConfig, &deviceHandle);

	return deviceHandle;
}

void I2CDriver::deleteDevice(i2c_master_dev_handle_t deviceHanlde) const {
	i2c_master_bus_rm_device(deviceHanlde);
}

bool I2CDriver::isDeviceDetected(uint8_t deviceAddress) {
	auto result = i2c_master_probe(busHandle, deviceAddress, -1);

	return result == ESP_OK;
}

I2CDriver I2CDriver::instance;

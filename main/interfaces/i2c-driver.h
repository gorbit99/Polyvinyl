#pragma once

#include <driver/i2c_master.h>

#include <cstdint>

#include "driver/i2c_types.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

class I2CDriver {
public:
	static I2CDriver& getInstance();

	void init();
	i2c_master_dev_handle_t registerDevice(uint8_t deviceId) const;
	void deleteDevice(i2c_master_dev_handle_t deviceHanlde) const;

	bool isDeviceDetected(uint8_t deviceAddress);

	void read(i2c_master_dev_handle_t device, uint8_t* data, size_t size) const {
		i2c_master_receive(device, data, size, -1);
	}

	uint8_t read(i2c_master_dev_handle_t device) const {
		uint8_t value;
		read(device, &value, 1);
		return value;
	}

	void
	write(i2c_master_dev_handle_t device, const uint8_t* data, size_t length) const {
		i2c_master_transmit(device, data, length, -1);
	}

	void write(i2c_master_dev_handle_t device, uint8_t data) const {
		write(device, &data, 1);
	}

	void writeRead(
		i2c_master_dev_handle_t device,
		const uint8_t* writeData,
		size_t writeSize,
		uint8_t* readData,
		size_t readSize
	) {
		i2c_master_transmit_receive(
			device,
			writeData,
			writeSize,
			reinterpret_cast<uint8_t*>(readData),
			readSize,
			-1
		);
	}

	void multiWrite(
		i2c_master_dev_handle_t device,
		i2c_master_transmit_multi_buffer_info_t* multiTransmitData,
		size_t entryCount
	) {
		i2c_master_multi_buffer_transmit(device, multiTransmitData, entryCount, -1);
	}

private:
	I2CDriver() = default;
	I2CDriver(const I2CDriver&) = default;
	I2CDriver(I2CDriver&&) = default;
	I2CDriver& operator=(const I2CDriver& other) = default;
	I2CDriver& operator=(I2CDriver&& other) = default;

	i2c_master_bus_handle_t busHandle;

	static I2CDriver instance;
};

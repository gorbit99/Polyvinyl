#include "i2c-register-interface.h"

#include "interfaces/i2c-driver.h"

I2CRegisterInterface::I2CRegisterInterface(uint8_t deviceId) {
	deviceHandle = I2CDriver::getInstance().registerDevice(deviceId);
}

I2CRegisterInterface::~I2CRegisterInterface() {
	if (deviceHandle != 0) {
		I2CDriver::getInstance().deleteDevice(deviceHandle);
	}
}

I2CRegisterInterface::I2CRegisterInterface(I2CRegisterInterface&& other) {
	deviceHandle = other.deviceHandle;
	other.deviceHandle = 0;
}

I2CRegisterInterface& I2CRegisterInterface::operator=(I2CRegisterInterface&& other) {
	deviceHandle = other.deviceHandle;
	other.deviceHandle = 0;
	return *this;
}

uint8_t I2CRegisterInterface::readByte(uint8_t address) {
	uint8_t result;
	I2CDriver::getInstance().writeRead(deviceHandle, &address, 1, &result, 1);
	return result;
}

void I2CRegisterInterface::readBytes(uint8_t address, uint8_t* data, size_t size) {
	I2CDriver::getInstance().writeRead(deviceHandle, &address, 1, data, size);
}

void I2CRegisterInterface::writeByte(uint8_t address, uint8_t data) {
	uint8_t buffer[] = {address, data};
	I2CDriver::getInstance().write(deviceHandle, buffer, 2);
}

void I2CRegisterInterface::writeBytes(
	uint8_t address,
	const uint8_t* data,
	size_t size
) {
	i2c_master_transmit_multi_buffer_info_t transmitData[] = {
		{
			.write_buffer = &address,
			.buffer_size = 1,
		},
		{
			.write_buffer = const_cast<uint8_t*>(data),
			.buffer_size = size,
		},
	};
	I2CDriver::getInstance().multiWrite(deviceHandle, transmitData, 2);
}

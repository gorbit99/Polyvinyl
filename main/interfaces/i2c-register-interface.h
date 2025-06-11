#pragma once

#include <cstdint>

#include "driver/i2c_types.h"
#include "register-interface.h"

class I2CRegisterInterface final : public RegisterInterface {
public:
	I2CRegisterInterface(uint8_t deviceId);
	~I2CRegisterInterface();
	I2CRegisterInterface(const I2CRegisterInterface&) = delete;
	I2CRegisterInterface(I2CRegisterInterface&& other);
	I2CRegisterInterface& operator=(const I2CRegisterInterface&) = delete;
	I2CRegisterInterface& operator=(I2CRegisterInterface&& other);

	uint8_t readByte(uint8_t address) override;
	void readBytes(uint8_t address, uint8_t* data, size_t size) override;
	void writeByte(uint8_t address, uint8_t data) override;
	void writeBytes(uint8_t address, const uint8_t* data, size_t size) override;

private:
	i2c_master_dev_handle_t deviceHandle;
};

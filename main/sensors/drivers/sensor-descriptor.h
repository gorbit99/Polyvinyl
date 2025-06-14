#pragma once

#include <cstdint>
#include <optional>
#include <utility>
#include <variant>
#include <vector>
#include <vqf.hpp>

#include "interfaces/register-interface.h"
#include "sensor-type.h"

struct SampleInterface {
	std::function<void(float*)> provideGyroSample;
	std::function<void(float*)> provideAccelSample;
	std::function<void(float)> provideTempSample;
};

struct IMUVariant {
    uint8_t whoAmI;
    SensorType sensorType;
};

struct SensorDescriptor {
	const char* name;

	uint8_t deviceIdBase;

	uint8_t whoAmIRegister;
	std::variant<IMUVariant, std::vector<IMUVariant>> expectedWhoAmI;

	std::function<bool(SensorType, RegisterInterface&)> setup;

	uint8_t packetSize;
	uint8_t dataCountRegister;
	uint8_t dataRegister;
	uint16_t dataCountMask;
	std::function<void(const uint8_t*, SampleInterface&)> parsePacket;
	float gyroRateHz;
	float accelRateHz;
	std::optional<float> tempRateHz;

	VQFParams vqfParams;
};

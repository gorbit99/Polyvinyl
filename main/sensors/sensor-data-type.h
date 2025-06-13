#pragma once

#include <cstdint>

enum class SensorDataType : uint8_t {
	Rotation = 0,
	FlexResistance = 1,
	FlexAngle = 2,
};

#pragma once

#include <cstdint>

struct SensorConfigBits {
	bool magEnabled : 1;
	bool magSupported : 1;
	bool calibrationEnabled : 1;
	bool calibrationSupported : 1;
	bool tempGradientCalibrationEnabled : 1;
	bool tempGradientCalibrationSupported : 1;

	uint8_t padding;
};

static_assert(sizeof(SensorConfigBits) == 2);

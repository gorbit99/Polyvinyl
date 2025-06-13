#pragma once

#include <vqf.hpp>

#include "portmacro.h"

class RestCalibrationDetector {
public:
	void update(bool restDetected);

	bool getWasCalibrated() const;

private:
	static constexpr float REST_DETECTION_SECONDS = 3;

	bool calibrated = false;

	bool atRest = false;
	TickType_t restStart;
};

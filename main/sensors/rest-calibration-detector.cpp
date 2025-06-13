#include "rest-calibration-detector.h"

#include "freertos/idf_additions.h"

void RestCalibrationDetector::update(bool restDetected) {
	if (calibrated) {
		return;
	}

	if (atRest && !restDetected) {
		atRest = false;
		return;
	}

	if (!atRest && !restDetected) {
		return;
	}

	if (!atRest && restDetected) {
		atRest = true;
		restStart = xTaskGetTickCount();
		return;
	}

	auto timeDiff = xTaskGetTickCount() - restStart;
	if (pdMS_TO_TICKS(static_cast<uint64_t>(REST_DETECTION_SECONDS * 1000))
		<= timeDiff) {
		calibrated = true;
	}
}

bool RestCalibrationDetector::getWasCalibrated() const { return calibrated; }

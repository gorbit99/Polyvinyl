#pragma once

#include <vector>

#include "sensor-driver.h"

class SensorManager {
public:
	void init();
	std::vector<SensorDriver>& getSensors();

	static SensorManager& getInstance();

private:
	SensorManager() = default;

	std::vector<SensorDriver> sensors;

	static SensorManager instance;
};

#include "sensor-manager.h"

void SensorManager::init() {
	sensors.emplace_back();
	sensors[0].init();
}

std::vector<SensorDriver>& SensorManager::getSensors() { return sensors; }

SensorManager& SensorManager::getInstance() { return instance; }

SensorManager SensorManager::instance;

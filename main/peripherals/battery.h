#pragma once

#include <functional>
#include <optional>

#include "interfaces/adc-driver.h"

class Battery {
public:
	static Battery& getInstance();
	void onBatteryReading(std::function<void(float, float)>&& callback);
	void init();

private:
	Battery() = default;
	float voltageToLevel(float voltage) const;

	std::optional<std::function<void(float, float)>> batteryReadingCallback;

	static void batteryReadTask(void* userArg);

	static Battery instance;
};

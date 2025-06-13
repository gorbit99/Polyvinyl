#include "battery.h"

#include <freertos/FreeRTOS.h>

#include <algorithm>

#include "freertos/idf_additions.h"
#include "interfaces/adc-driver.h"
#include "portmacro.h"
#include "sdkconfig.h"

Battery& Battery::getInstance() { return instance; }

void Battery::onBatteryReading(std::function<void(float, float)>&& callback) {
	batteryReadingCallback = callback;
}

void Battery::init() {
	xTaskCreate(batteryReadTask, "battery", 4096, this, 1, nullptr);
}

void Battery::batteryReadTask(void* userArg) {
	auto* self = reinterpret_cast<Battery*>(userArg);

	ADCChannel adcChannel = ADCDriver::getInstance().getChannel(
		CONFIG_BATTERY_PIN,
		CONFIG_BATTERY_DIVIDER_R1,
		CONFIG_BATTERY_DIVIDER_R2 + CONFIG_BATTERY_SHIELD_RESISTANCE
	);

	auto lastWakeTime = xTaskGetTickCount();

	while (true) {
		float voltage = adcChannel.read();
		float level = self->voltageToLevel(voltage);

		if (self->batteryReadingCallback) {
			(*self->batteryReadingCallback)(voltage, level);
		}

		vTaskDelayUntil(
			&lastWakeTime,
			1000 * CONFIG_BATTERY_READ_RATE_SECONDS / portTICK_PERIOD_MS
		);
	}
}

float Battery::voltageToLevel(float voltage) const {
	float level;
	if (voltage > 3.975f) {
		level = (voltage - 2.920f) * 0.8f;
	} else if (voltage > 3.678f) {
		level = (voltage - 3.300f) * 1.25f;
	} else if (voltage > 3.489f) {
		level = (voltage - 3.400f) * 1.7f;
	} else if (voltage > 3.360f) {
		level = (voltage - 3.300f) * 0.8f;
	} else {
		level = (voltage - 3.200f) * 0.3f;
	}

	level = (level - 0.05f) / 0.95f;

	return std::clamp(level, 0.0f, 1.0f);

	return level;
}

Battery Battery::instance;

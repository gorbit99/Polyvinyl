#pragma once

#include <esp_adc/adc_oneshot.h>

#include <cstdint>

#include "hal/adc_types.h"

class ADCChannel {
public:
	float read() const;

private:
	ADCChannel(
		adc_channel_t channel,
		float voltageMultiplier,
		adc_cali_handle_t calibrationHandle
	);

	adc_channel_t channel;
	float voltageMultiplier;
	adc_cali_handle_t calibrationHandle;

	friend class ADCDriver;
};

class ADCDriver {
public:
	static ADCDriver& getInstance();
	void init();

	ADCChannel getChannel(uint8_t pin, uint32_t dividerR1, uint32_t dividerR2) const;

private:
	ADCDriver() = default;

	int read(adc_channel_t channel) const;

	adc_oneshot_unit_handle_t adc1Handle;

	static ADCDriver instance;

	friend class ADCChannel;
};

#include "adc-driver.h"

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/adc_types.h"

static const char* TAG = "ADC";

ADCChannel::ADCChannel(
	adc_channel_t channel,
	float voltageMultiplier,
	adc_cali_handle_t calibrationHandle
)
	: channel{channel}
	, voltageMultiplier{voltageMultiplier}
	, calibrationHandle{calibrationHandle} {}

float ADCChannel::read() const {
	int adcRead = ADCDriver::getInstance().read(channel);
	int millivolts;

	ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibrationHandle, adcRead, &millivolts));

	return millivolts / 1000.0f * voltageMultiplier;
}

ADCDriver& ADCDriver::getInstance() { return instance; }

void ADCDriver::init() {
	adc_oneshot_unit_init_cfg_t adcUnitConfig{
		.unit_id = ADC_UNIT_1,
		.ulp_mode = ADC_ULP_MODE_DISABLE,
	};
	ESP_ERROR_CHECK(adc_oneshot_new_unit(&adcUnitConfig, &adc1Handle));
}

ADCChannel
ADCDriver::getChannel(uint8_t pin, uint32_t dividerR1, uint32_t dividerR2) const {
	adc_oneshot_chan_cfg_t channelConfig{
		.atten = ADC_ATTEN_DB_12,
		.bitwidth = static_cast<adc_bitwidth_t>(ADC_BITWIDTH_12),
	};

	adc_channel_t channel;
	adc_unit_t adcUnit;
	ESP_ERROR_CHECK(adc_oneshot_io_to_channel(pin, &adcUnit, &channel));

	if (adcUnit != ADC_UNIT_1) {
		ESP_LOGE(
			TAG,
			"Pin %d is associated with an unsupported adc channel (%d)!",
			pin,
			adcUnit
		);
		abort();
	}

	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1Handle, channel, &channelConfig));

	adc_cali_curve_fitting_config_t calibrationConfig{
		.unit_id = ADC_UNIT_1,
		.chan = channel,
		.atten = ADC_ATTEN_DB_12,
		.bitwidth = static_cast<adc_bitwidth_t>(ADC_BITWIDTH_12),
	};
	adc_cali_handle_t calibrationHandle;
	ESP_ERROR_CHECK(
		adc_cali_create_scheme_curve_fitting(&calibrationConfig, &calibrationHandle)
	);

	return {
		channel,
		1 + static_cast<float>(dividerR2) / static_cast<float>(dividerR1),
		calibrationHandle
	};
}

int ADCDriver::read(adc_channel_t channel) const {
	int adcRead;
	ESP_ERROR_CHECK(adc_oneshot_read(adc1Handle, channel, &adcRead));

	return adcRead;
}

ADCDriver ADCDriver::instance;

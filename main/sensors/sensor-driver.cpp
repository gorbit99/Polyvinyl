#include "sensor-driver.h"

#include <esp_log.h>

#include <algorithm>
#include <cstdint>

#include "drivers/icm45686.h"
#include "freertos/idf_additions.h"
#include "interfaces/i2c-driver.h"
#include "interfaces/i2c-register-interface.h"
#include "sensors/drivers/sensor-descriptor.h"
#include "sensors/sensor-reader.h"

static const char* TAG = "SENSORDRIVER";

std::vector<SensorDescriptor> SensorDriver::supportedSensors = {
	ICM45686::DESCRIPTOR,
};

bool SensorDriver::init() {
	for (auto& sensor : supportedSensors) {
		auto result = checkSensor(sensor);

		if (result == SensorStatus::NotFound) {
			continue;
		}

		if (result == SensorStatus::Errored) {
			status = SensorStatus::Errored;
			break;
		}

		foundSensor = sensor;
		status = SensorStatus::Ok;
		vqf = VQF{sensor.vqfParams, 1 / sensor.gyroRateHz, 1 / sensor.accelRateHz};
		setupSensorRead();
		break;
	}

	return status == SensorStatus::Ok;
}

SensorDriver::SensorStatus SensorDriver::checkSensor(SensorDescriptor& sensor) {
	// TODO: handle secondary device address
	ESP_LOGI(TAG, "Trying %s", sensor.name);

	if (!I2CDriver::getInstance().isDeviceDetected(sensor.deviceIdBase)) {
		ESP_LOGD(TAG, "%s not on I2C bus", sensor.name);
		return SensorStatus::NotFound;
	}

	auto interface = std::make_unique<I2CRegisterInterface>(sensor.deviceIdBase);

	auto whoAmI = interface->readByte(sensor.whoAmIRegister);

	if (std::holds_alternative<uint8_t>(sensor.expectedWhoAmI)) {
		if (std::get<uint8_t>(sensor.expectedWhoAmI) != whoAmI) {
			ESP_LOGD(TAG, "%s didn't pass the whoAmI check", sensor.name);
			return SensorStatus::NotFound;
		}
	} else if (std::holds_alternative<std::vector<uint8_t>>(sensor.expectedWhoAmI)) {
		auto& values = std::get<std::vector<uint8_t>>(sensor.expectedWhoAmI);

		bool match = std::find(values.begin(), values.end(), whoAmI) != values.end();

		if (!match) {
			ESP_LOGD(TAG, "%s didn't pass the whoAmI check", sensor.name);
			return SensorStatus::NotFound;
		}
	}

	ESP_LOGI(TAG, "Found %s! Initializing", sensor.name);
	if (!sensor.setup(*interface)) {
		ESP_LOGE(TAG, "%s couldn't be initialized!", sensor.name);
		return SensorStatus::Errored;
	}

	this->interface = std::move(interface);

	return SensorStatus::Ok;
}

void SensorDriver::setupSensorRead() {
	assert(status == SensorStatus::Ok);

	ESP_LOGI(TAG, "Setting up sensor read for %s", foundSensor.name);

	packetQueue = SensorReader::getInstance().registerSensor({
		.interface = *interface,
		.dataCountRegister = foundSensor.dataCountRegister,
		.dataRegister = foundSensor.dataRegister,
		.entrySize = foundSensor.packetSize,
		.dataCountMask = foundSensor.dataCountMask,
	});

	sprintf(packetTaskName, "%s_packet", foundSensor.name);
	xTaskCreate(packetTask, packetTaskName, 4096, this, 1, nullptr);
}

void SensorDriver::packetTask(void* userArg) {
	auto* self = reinterpret_cast<SensorDriver*>(userArg);
	uint8_t* dataBuffer = new uint8_t[self->foundSensor.packetSize];

	SampleInterface sampleInterface{
		.provideGyroSample = [&](float* sample) { self->vqf.updateGyr(sample); },
		.provideAccelSample = [&](float* sample) { self->vqf.updateAcc(sample); },
		.provideTempSample = [](auto) { /* TODO: */ }
	};

	while (true) {
		xQueueReceive(self->packetQueue, dataBuffer, portMAX_DELAY);

		self->foundSensor.parsePacket(dataBuffer, sampleInterface);
	}
}

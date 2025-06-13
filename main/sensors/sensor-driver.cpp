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

		if (result == Status::NotFound) {
			continue;
		}

		if (result == Status::Errored) {
			status = Status::Errored;
			break;
		}

		foundSensor = sensor;

		status = Status::Ok;
		vqf = VQF{sensor.vqfParams, 1 / sensor.gyroRateHz, 1 / sensor.accelRateHz};
		setupSensorRead();
		break;
	}

	vqfMutex = xSemaphoreCreateMutex();

	return status == Status::Ok;
}

bool SensorDriver::getHasNewFusionState() const { return hasNewFusionState; }

void SensorDriver::getFusionState(float quat[4], float accel[3]) {
	xSemaphoreTake(vqfMutex, portMAX_DELAY);
	vqf.getQuat6D(quat);
	xSemaphoreGive(vqfMutex);

	float gravity[3]{
		2 * (quat[1] * quat[3] - quat[0] * quat[2]),
		2 * (quat[0] * quat[1] + quat[2] * quat[3]),
		quat[0] * quat[0] - quat[1] * quat[1] - quat[2] * quat[2] + quat[3] * quat[3],
	};

	accel[0] = acceleration[0] - gravity[0] * Constants::EARTH_GRAVITY;
	accel[1] = acceleration[1] - gravity[1] * Constants::EARTH_GRAVITY;
	accel[2] = acceleration[2] - gravity[2] * Constants::EARTH_GRAVITY;

	hasNewFusionState = false;
}

float SensorDriver::getTemperature() {
	float result = temperatureSum / temperatureSampleCount;
	temperatureSampleCount = 0;
	temperatureSum = 0;
	return result;
}

SensorDriver::Status SensorDriver::checkSensor(SensorDescriptor& sensor) {
	// TODO: handle secondary device address
	ESP_LOGI(TAG, "Trying %s", sensor.name);

	if (!I2CDriver::getInstance().isDeviceDetected(sensor.deviceIdBase)) {
		ESP_LOGD(TAG, "%s not on I2C bus", sensor.name);
		return Status::NotFound;
	}

	auto interface = std::make_unique<I2CRegisterInterface>(sensor.deviceIdBase);

	auto whoAmI = interface->readByte(sensor.whoAmIRegister);

	if (std::holds_alternative<uint8_t>(sensor.expectedWhoAmI)) {
		if (std::get<uint8_t>(sensor.expectedWhoAmI) != whoAmI) {
			ESP_LOGD(TAG, "%s didn't pass the whoAmI check", sensor.name);
			return Status::NotFound;
		}
	} else if (std::holds_alternative<std::vector<uint8_t>>(sensor.expectedWhoAmI)) {
		auto& values = std::get<std::vector<uint8_t>>(sensor.expectedWhoAmI);

		bool match = std::find(values.begin(), values.end(), whoAmI) != values.end();

		if (!match) {
			ESP_LOGD(TAG, "%s didn't pass the whoAmI check", sensor.name);
			return Status::NotFound;
		}
	}

	ESP_LOGI(TAG, "Found %s! Initializing", sensor.name);
	if (!sensor.setup(*interface)) {
		ESP_LOGE(TAG, "%s couldn't be initialized!", sensor.name);
		return Status::Errored;
	}

	this->interface = std::move(interface);

	return Status::Ok;
}

void SensorDriver::setupSensorRead() {
	assert(status == Status::Ok);

	ESP_LOGI(TAG, "Setting up sensor read for %s", foundSensor.name);

	packetQueue = SensorReader::getInstance().registerSensor({
		.interface = *interface,
		.dataCountRegister = foundSensor.dataCountRegister,
		.dataRegister = foundSensor.dataRegister,
		.entrySize = foundSensor.packetSize,
		.dataCountMask = foundSensor.dataCountMask,
	});

	sprintf(packetTaskName, "%s_packet", foundSensor.name);
	xTaskCreate(packetTask, packetTaskName, 4096, this, 4, nullptr);
}

void SensorDriver::packetTask(void* userArg) {
	auto* self = reinterpret_cast<SensorDriver*>(userArg);
	uint8_t* dataBuffer = new uint8_t[self->foundSensor.packetSize];

	SampleInterface sampleInterface{
		.provideGyroSample =
			[self](float sample[3]) {
				sample[0] *= Constants::DEG_TO_RAD;
				sample[1] *= Constants::DEG_TO_RAD;
				sample[2] *= Constants::DEG_TO_RAD;
				xSemaphoreTake(self->vqfMutex, portMAX_DELAY);
				self->vqf.updateGyr(sample);
				xSemaphoreGive(self->vqfMutex);
				self->hasNewFusionState = true;
				
			},
		.provideAccelSample =
			[self](float sample[3]) {
				sample[0] *= Constants::EARTH_GRAVITY;
				sample[1] *= Constants::EARTH_GRAVITY;
				sample[2] *= Constants::EARTH_GRAVITY;
				memcpy(self->acceleration, sample, sizeof(self->acceleration));
				xSemaphoreTake(self->vqfMutex, portMAX_DELAY);
				self->vqf.updateAcc(sample);
				xSemaphoreGive(self->vqfMutex);
				self->hasNewFusionState = true;
			},
		.provideTempSample =
			[self](float temperature) {
				self->temperatureSum += temperature;
				self->temperatureSampleCount++;
			}
	};

	while (true) {
		xQueueReceive(self->packetQueue, dataBuffer, portMAX_DELAY);

		self->foundSensor.parsePacket(dataBuffer, sampleInterface);

		self->restCalibrationDetector.update(self->vqf.getRestDetected());
	}
}

SensorDriver::Status SensorDriver::getStatus() const { return status; }

SensorType SensorDriver::getType() const { return foundSensor.sensorType; }

bool SensorDriver::getRestCalibrated() const {
	return restCalibrationDetector.getWasCalibrated();
}

SensorPosition SensorDriver::getPosition() const { return position; }

SensorDataType SensorDriver::getDataType() const { return SensorDataType::Rotation; }

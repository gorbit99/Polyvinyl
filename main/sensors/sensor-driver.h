#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <variant>
#include <vector>
#include <vqf.hpp>

#include "drivers/sensor-descriptor.h"
#include "freertos/idf_additions.h"
#include "interfaces/register-interface.h"
#include "sensors/drivers/sensor-type.h"
#include "sensors/rest-calibration-detector.h"
#include "sensors/sensor-data-type.h"
#include "sensors/sensor-position.h"

class SensorDriver {
public:
	bool init();
	bool getHasNewFusionState() const;
	void getFusionState(float quat[4], float accel[3]);
	float getTemperature();

	enum class Status : uint8_t {
		NotFound = 0,
		Ok = 1,
		Errored = 2,
	};

	Status getStatus() const;
	SensorType getType() const;
	bool getRestCalibrated() const;
	SensorPosition getPosition() const;
	SensorDataType getDataType() const;

private:
	Status checkSensor(SensorDescriptor& sensor);
	void setupSensorRead();

	static void packetTask(void* userArg);
	QueueHandle_t packetQueue;
	char packetTaskName[configMAX_TASK_NAME_LEN];

	SensorDescriptor foundSensor = {.sensorType = SensorType::Empty};
	Status status = Status::NotFound;
	std::unique_ptr<RegisterInterface> interface;

	VQF vqf;
	SemaphoreHandle_t vqfMutex;
	bool hasNewFusionState = true;
	float acceleration[3];
	float temperatureSum = 0;
	size_t temperatureSampleCount = 0;

	RestCalibrationDetector restCalibrationDetector;

	SensorPosition position;

	static std::vector<SensorDescriptor> supportedSensors;
};

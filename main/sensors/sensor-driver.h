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

class SensorDriver {
public:
	bool init();

private:
	enum class SensorStatus {
		NotFound,
		Errored,
		Ok,
	};

	SensorStatus checkSensor(SensorDescriptor& sensor);
	void setupSensorRead();

	static void packetTask(void* userArg);
	QueueHandle_t packetQueue;
	char packetTaskName[configMAX_TASK_NAME_LEN];

	SensorDescriptor foundSensor = {};
	SensorStatus status = SensorStatus::NotFound;
	std::unique_ptr<RegisterInterface> interface;

	VQF vqf;

	static std::vector<SensorDescriptor> supportedSensors;
};

#include "sensor-reader.h"

#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <freertos/projdefs.h>

#include <cmath>
#include <cstdint>

#include "interfaces/register-interface.h"

static const char* TAG = "SENSORREADER";

void SensorReader::init() {
	readEntriesMutex = xSemaphoreCreateMutex();

	xTaskCreate(readTask, "sensor_read_task", 4096, this, 4, nullptr);
}

QueueHandle_t SensorReader::registerSensor(ReadDescriptor&& descriptor) {
	QueueHandle_t queueHandle = xQueueCreate(20, descriptor.entrySize);

	xSemaphoreTake(readEntriesMutex, portMAX_DELAY);

	if (readBuffer.size() < descriptor.entrySize * MAX_PACKET_READ_COUNT) {
		readBuffer.resize(descriptor.entrySize * MAX_PACKET_READ_COUNT);
	}

	readEntries.emplace_back(
		descriptor.interface,
		descriptor.dataCountRegister,
		descriptor.dataRegister,
		queueHandle,
		descriptor.entrySize,
		descriptor.dataCountMask
	);

	xSemaphoreGive(readEntriesMutex);

	return queueHandle;
}

void SensorReader::readTask(void* userArg) {
	auto* self = reinterpret_cast<SensorReader*>(userArg);

	while (true) {
		xSemaphoreTake(self->readEntriesMutex, portMAX_DELAY);

		for (auto& entry : self->readEntries) {
			self->readFrom(entry);
		}

		xSemaphoreGive(self->readEntriesMutex);

		vTaskDelay(1);
	}
}

void SensorReader::readFrom(ReadEntry& entry) {
	uint16_t packetCount;
	entry.interface.readBytes(
		entry.dataCountRegister,
		reinterpret_cast<uint8_t*>(&packetCount),
		2
	);
	packetCount &= entry.dataCountMask;

	if (packetCount == 0) {
		return;
	}

	packetCount = std::min(packetCount, static_cast<uint16_t>(MAX_PACKET_READ_COUNT));

	entry.interface.readBytes(
		entry.dataRegister,
		readBuffer.data(),
		entry.entrySize * packetCount
	);

	for (size_t i = 0; i < packetCount; i++) {
		auto result
			= xQueueSendToBack(entry.dataQueue, &readBuffer[i * entry.entrySize], 0);

		if (result == errQUEUE_FULL) {
			ESP_LOGE(TAG, "Data queue full! Dropping samples");
			break;
		}
	}
}

SensorReader& SensorReader::getInstance() { return instance; }

SensorReader SensorReader::instance;

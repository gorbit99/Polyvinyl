#pragma once

#include <freertos/FreeRTOS.h>

#include <cstdint>
#include <vector>

#include "interfaces/register-interface.h"

class SensorReader {
public:
	struct ReadDescriptor {
		RegisterInterface& interface;
		uint8_t dataCountRegister;
		uint8_t dataRegister;
		uint8_t entrySize;
		uint16_t dataCountMask;
	};

	void init();
	QueueHandle_t registerSensor(ReadDescriptor&& descriptor);

	static SensorReader& getInstance();

private:
	struct ReadEntry {
		RegisterInterface& interface;
		uint8_t dataCountRegister;
		uint8_t dataRegister;
		QueueHandle_t dataQueue;
		uint8_t entrySize;
		uint16_t dataCountMask;
	};

	void readFrom(ReadEntry& entry);

	static void readTask(void* userArg);

	std::vector<ReadEntry> readEntries;
	SemaphoreHandle_t readEntriesMutex;
	std::vector<uint8_t> readBuffer;

	static SensorReader instance;

	static constexpr size_t MAX_PACKET_READ_COUNT = 8;
};

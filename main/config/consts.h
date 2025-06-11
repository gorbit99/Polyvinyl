#pragma once

#include <cstdint>

enum class BoardType : uint32_t {
	Unknown = 0,
	SlimeVRLegacy = 1,
	SlimeVRDev = 2,
	NodeMcu = 3,
	Custom = 4,
	WROOM32 = 5,
	WEMOSD1Mini = 6,
	TTGOTBase = 7,
	ESP01 = 8,
	SlimeVR = 9,  // SlimeVR v1.0 & v1.1
	LolinC3Mini = 10,
	Beetle32C3 = 11,
	ESP32C3DevKitM1 = 12,
	OWOTrack = 13,  // Only used by owoTrack mobile app
	Wrangler = 14,  // Only used by wrangler app
	Mocopi = 15,  // Used by mocopi/moslime
	WEMOSWROOM02 = 16,
	XiaoEsp32C3 = 17,
	Haritora = 18,  // Used by Haritora/SlimeTora
	ESP32C6DevKitC1 = 19,
	GloveIMUSlimeVRDev = 20,  // IMU Glove
	Gestures = 21,  // Used by Gestures
	SlimeVR_V1_2 = 22,  // SlimeVR v1.2
	ESP32S3Supermini = 23,
	DevReserved = 250,  // Reserved, should not be used in any release firmware
};

enum class MCUType : uint32_t {
	Unknown = 0,
	ESP8266 = 1,
	ESP32 = 2,
	OWOTrackAndroid = 3,
	Wrangler = 4,
	OWOTrackiOS = 5,
	ESP32C3 = 6,
	Mocopi = 7,
	Haritora = 8,
	Reserved = 250,
};

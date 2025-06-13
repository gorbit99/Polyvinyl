#pragma once

#include <algorithm>
#include <bit>
#include <cstdint>

#include "config/consts.h"
#include "sensors/drivers/sensor-type.h"
#include "sensors/sensor-data-type.h"
#include "sensors/sensor-driver.h"
#include "sensors/sensor-position.h"

template <typename T>
struct BigEndian {
	explicit(false) BigEndian(T value)
		: value{static_cast<T>(swapBytes(value))} {}

	operator T() const { return swapBytes(value); }

	static T swapBytes(T data) {
		auto* bytes = reinterpret_cast<uint8_t*>(&data);
		std::reverse(bytes, bytes + sizeof(T));
		return data;
	}

private:
	T value;
};

enum class SendPacketId : uint32_t {
	HeartBeat = 0,
	//  Rotation = 1,
	//  Gyro = 2,
	Handshake = 3,
	Accel = 4,
	// Mag = 5,
	// RawCalibrationData = 6,
	// CalibrationFinished = 7,
	// RawMagnetometer = 9,
	Serial = 11,
	BatteryLevel = 12,
	Tap = 13,
	Error = 14,
	SensorInfo = 15,
	// Rotation2 = 16,
	RotationData = 17,
	MagnetometerAccuracy = 18,
	SignalStrength = 19,
	Temperature = 20,
	// UserAction = 21,
	FeatureFlags = 22,
	// RotationAcceleration = 23,
	AcknowledgeConfigChange = 24,
	FlexData = 26,
	Bundle = 100,
	Inspection = 105,
};

enum class ReceivePacketId : uint8_t {
	HeartBeatReply = 1,
	Vibrate = 2,
	Handshake = 3,
	Command = 4,
	Config = 8,
	PingPong = 10,
	SensorInfo = 15,
	FeatureFlags = 22,
	SetConfigFlag = 25,
};

#pragma pack(push, 1)

struct HeartBeatReplyPacket {};

struct VibratePacket {
	// TODO: ?
};

struct HandshakeReplyPacket {};

struct AccelPacket {
	BigEndian<float> x;
	BigEndian<float> y;
	BigEndian<float> z;
	uint8_t sensorId;
};

struct BatteryLevelPacket {
	BigEndian<float> voltage;
	BigEndian<float> percentage;
};

struct SensorInfoPacket {
	uint8_t sensorId;
	SensorDriver::Status sensorStatus;
	SensorType sensorType;
	BigEndian<uint16_t> sensorConfigData;
	bool hasCompletedRestCalibration;
	SensorPosition sensorPosition;
	SensorDataType sensorDataType;
	// ADD NEW FIELDS ABOVE THIS COMMENT ^^^^^^^^
	// WARNING! Only for debug purposes and SHOULD ALWAYS BE LAST IN THE PACKET.
	// It WILL BE REMOVED IN THE FUTURE
	// Send TPS
	BigEndian<float> tpsCounterAveragedTps;
	BigEndian<float> dataCounterAveragedTps;
};

enum class RotationDataType : uint8_t {
	Normal = 1,
	Correction = 2,
};

struct RotationDataPacket {
	uint8_t sensorId;
	RotationDataType dataType;
	BigEndian<float> x;
	BigEndian<float> y;
	BigEndian<float> z;
	BigEndian<float> w;
	uint8_t accuracyInfo;
};

struct SignalStrengthPacket {
	uint8_t sensorId;
	uint8_t signalStrength;
};

struct TemperaturePacket {
	uint8_t sensorId;
	BigEndian<float> temperature;
};

#pragma pack(pop)

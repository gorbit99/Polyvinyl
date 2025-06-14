#pragma once

#include <cstring>

#include "config-icm45686.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "register-helper.h"
#include "sensor-descriptor.h"
#include "utils/consts.h"

namespace ICM45686 {

namespace {

struct RegisterMap {
	struct RegMisc2 : Register<0x7f> {
		uint8_t iRegDone : 1 = 1;
		uint8_t softRst : 1;
	};
	struct FifoConfig0 : Register<0x1d> {
		enum class FifoDepth : uint8_t {
			Depth2k = 0b000111,
			Depth8k = 0b011111,
		};
		FifoDepth fifoDepth : 6;
		enum class FifoMode : uint8_t {
			Bypass = 0,
			Stream = 1,
			StopOnFull = 2,
		};
		FifoMode fifoMode : 2;
	};
	struct FifoConfig3 : Register<0x21> {
		bool fifoIfEn : 1;
		bool fifoAccelEn : 1;
		bool fifoGyroEn : 1;
		bool fifoHiresEn : 1;
		bool fifoEs0En : 1;
		bool fifoEs1En : 1;
	};
	struct GyroConfig0 : Register<0x1c> {
		GyroOdr gyroOdr : 4;
		GyroFullScale gyroUiFsSel : 4;
	};
	struct AccelConfig0 : Register<0x1b> {
		AccelOdr accelOdr : 4;
		AccelFullScale accelUiFsSel : 3;
	};
	struct PwrMgmt0 : Register<0x10> {
		enum class AccelMode : uint8_t {
			Off = 0b00,
			LowPower = 0b10,
			LowNoise = 0b11,
		};
		AccelMode accelMode : 2;
		enum class GyroMode : uint8_t {
			Off = 0b00,
			Standby = 0b01,
			LowPower = 0b10,
			LowNoise = 0b11,
		};
		GyroMode gyroMode : 2;
	};
	struct FifoCount0 : Register<0x12> {};
	struct FifoData : Register<0x14> {};
};

#pragma pack(push, 1)

struct Packet {
	enum class HeaderFlags : uint8_t { gyroOdr = 0x1, accelOdr = 0x2, fsync };

	static constexpr uint16_t INVALID_SAMPLE = 0x8000;

	uint8_t header;
	int16_t accel[3];
	int16_t gyro[3];
	int16_t temp;
	uint16_t timeStep;
	uint8_t lsb[3];
};

#pragma pack(pop)

inline float getGyroValue(Packet& packet, size_t axis) {
	int32_t raw
		= static_cast<int32_t>(packet.gyro[axis]) << 4 | (packet.lsb[axis] & 0x0f);
	return raw * Consts::GYRO_SENSITIVITY;
}

inline float getAccelValue(Packet& packet, size_t axis) {
	int32_t raw = static_cast<int32_t>(packet.accel[axis]) << 4
				| (packet.lsb[axis] & 0xf0) >> 4;
	return raw * Consts::ACCEL_SENSITIVITY;
}

}  // namespace

SensorDescriptor DESCRIPTOR{
	.name = "ICM-45686",
	.deviceIdBase = 0x68,
	.whoAmIRegister = 0x72,
	.expectedWhoAmI = IMUVariant{0xe9, SensorType::ICM45686},
	.setup =
		[](SensorType sensorType, RegisterInterface& interface) {
			interface.writeRegister(RegisterMap::RegMisc2{.softRst = 1});
			vTaskDelay(35 / portTICK_PERIOD_MS);
			while (interface.readRegister<typename RegisterMap::RegMisc2>().softRst)
				;

			interface.writeRegister(
				RegisterMap::GyroConfig0{
					.gyroOdr = GYRO_ODR,
					.gyroUiFsSel = Consts::GYRO_FULL_SCALE,
				}
			);
			interface.writeRegister(
				RegisterMap::AccelConfig0{
					.accelOdr = ACCEL_ODR,
					.accelUiFsSel = Consts::ACCEL_FULL_SCALE,
				}
			);
			interface.writeRegister(
				RegisterMap::PwrMgmt0{
					.accelMode = RegisterMap::PwrMgmt0::AccelMode::LowNoise,
					.gyroMode = RegisterMap::PwrMgmt0::GyroMode::LowNoise,
				}
			);
			interface.writeRegister(
				RegisterMap::FifoConfig0{
					.fifoDepth = RegisterMap::FifoConfig0::FifoDepth::Depth8k,
					.fifoMode = RegisterMap::FifoConfig0::FifoMode::StopOnFull,
				}
			);
			interface.writeRegister(
				RegisterMap::FifoConfig3{
					.fifoIfEn = true,
					.fifoAccelEn = true,
					.fifoGyroEn = true,
					.fifoHiresEn = true,
					.fifoEs0En = false,
					.fifoEs1En = false,
				}
			);

			return true;
		},
	.packetSize = static_cast<uint8_t>(sizeof(Packet)),
	.dataCountRegister = RegisterMap::FifoCount0::reg,
	.dataRegister = RegisterMap::FifoData::reg,
	.dataCountMask = 0xffff,
	.parsePacket =
		[](const uint8_t* data, SampleInterface& sampleInterface) {
			Packet packet;
			memcpy(&packet, data, sizeof(Packet));

			float gyroSample[]{
				getGyroValue(packet, 0),
				getGyroValue(packet, 1),
				getGyroValue(packet, 2),
			};
			sampleInterface.provideGyroSample(gyroSample);

			if (packet.accel[0] != static_cast<int16_t>(Packet::INVALID_SAMPLE)) {
				float accelSample[]{
					getAccelValue(packet, 0),
					getAccelValue(packet, 1),
					getAccelValue(packet, 2),
				};
				sampleInterface.provideAccelSample(accelSample);
			}

			if (packet.temp != static_cast<int16_t>(Packet::INVALID_SAMPLE)) {
				float temp = static_cast<float>(packet.temp) / Consts::TEMP_SENSITIVITY
						   + Consts::TEMP_ZERO;
				sampleInterface.provideTempSample(temp);
			}
		},
	.gyroRateHz = Consts::GYRO_ODR_HZ,
	.accelRateHz = Consts::ACCEL_ODR_HZ,
	.tempRateHz = Consts::TEMP_ODR_HZ,

    .vqfParams = {
        .tauAcc = 7.171490,
		.biasSigmaInit = 0.337976,
		.biasForgettingTime = 352.235500,
		.biasClip = 5.0,
		.biasSigmaMotion = 0.985346,
		.biasVerticalForgettingFactor = 0.007959,
		.biasSigmaRest = 0.028897,
		.restMinT = 4.648680,
		.restFilterTau = 1.900166,
		.restThGyr = 2.620598,
		.restThAcc = 2.142593,
    },
};

};  // namespace ICM45686

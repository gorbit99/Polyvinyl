#pragma once

#include <cmath>
#include <cstdint>

namespace ICM45686 {
namespace {

enum class GyroOdr : uint8_t {
	Odr6400 = 0b0011,
	Odr3200 = 0b0100,
	Odr1600 = 0b0101,
	Odr800 = 0b0110,
	Odr400 = 0b0111,
	Odr200 = 0b1000,
	Odr100 = 0b1001,
	Odr50 = 0b1010,
	Odr25 = 0b1011,
	Odr12_5 = 0b1100,
	Odr6_25 = 0b1101,
	Odr3_125 = 0b1110,
	Odr1_5625 = 0b1111,
};

#ifdef CONFIG_ICM45686_GYRO_ODR_6400
constexpr auto GYRO_ODR = GyroOdr::Odr6400;
#elifdef CONFIG_ICM45686_GYRO_ODR_3200
constexpr auto GYRO_ODR = GyroOdr::Odr3200;
#elifdef CONFIG_ICM45686_GYRO_ODR_1600
constexpr auto GYRO_ODR = GyroOdr::Odr1600;
#elifdef CONFIG_ICM45686_GYRO_ODR_800
constexpr auto GYRO_ODR = GyroOdr::Odr800;
#elifdef CONFIG_ICM45686_GYRO_ODR_400
constexpr auto GYRO_ODR = GyroOdr::Odr400;
#elifdef CONFIG_ICM45686_GYRO_ODR_200
constexpr auto GYRO_ODR = GyroOdr::Odr200;
#elifdef CONFIG_ICM45686_GYRO_ODR_100
constexpr auto GYRO_ODR = GyroOdr::Odr100;
#elifdef CONFIG_ICM45686_GYRO_ODR_50
constexpr auto GYRO_ODR = GyroOdr::Odr50;
#elifdef CONFIG_ICM45686_GYRO_ODR_25
constexpr auto GYRO_ODR = GyroOdr::Odr25;
#elifdef CONFIG_ICM45686_GYRO_ODR_12_5
constexpr auto GYRO_ODR = GyroOdr::Odr12_5;
#elifdef CONFIG_ICM45686_GYRO_ODR_6_25
constexpr auto GYRO_ODR = GyroOdr::Odr6_25;
#elifdef CONFIG_ICM45686_GYRO_ODR_3_125
constexpr auto GYRO_ODR = GyroOdr::Odr3_125;
#elifdef CONFIG_ICM45686_GYRO_ODR_1_5625
constexpr auto GYRO_ODR = GyroOdr::Odr1_5625;
#endif

enum class AccelOdr : uint8_t {
	Odr6400 = 0b0011,
	Odr3200 = 0b0100,
	Odr1600 = 0b0101,
	Odr800 = 0b0110,
	Odr400 = 0b0111,
	Odr200 = 0b1000,
	Odr100 = 0b1001,
	Odr50 = 0b1010,
	Odr25 = 0b1011,
	Odr12_5 = 0b1100,
	Odr6_25 = 0b1101,
	Odr3_125 = 0b1110,
	Odr1_5625 = 0b1111,
};

#ifdef CONFIG_ICM45686_ACCEL_ODR_6400
constexpr auto ACCEL_ODR = AccelOdr::Odr6400;
#elifdef CONFIG_ICM45686_ACCEL_ODR_3200
constexpr auto ACCEL_ODR = AccelOdr::Odr3200;
#elifdef CONFIG_ICM45686_ACCEL_ODR_1600
constexpr auto ACCEL_ODR = AccelOdr::Odr1600;
#elifdef CONFIG_ICM45686_ACCEL_ODR_800
constexpr auto ACCEL_ODR = AccelOdr::Odr800;
#elifdef CONFIG_ICM45686_ACCEL_ODR_400
constexpr auto ACCEL_ODR = AccelOdr::Odr400;
#elifdef CONFIG_ICM45686_ACCEL_ODR_200
constexpr auto ACCEL_ODR = AccelOdr::Odr200;
#elifdef CONFIG_ICM45686_ACCEL_ODR_100
constexpr auto ACCEL_ODR = AccelOdr::Odr100;
#elifdef CONFIG_ICM45686_ACCEL_ODR_50
constexpr auto ACCEL_ODR = AccelOdr::Odr50;
#elifdef CONFIG_ICM45686_ACCEL_ODR_25
constexpr auto ACCEL_ODR = AccelOdr::Odr25;
#elifdef CONFIG_ICM45686_ACCEL_ODR_12_5
constexpr auto ACCEL_ODR = AccelOdr::Odr12_5;
#elifdef CONFIG_ICM45686_ACCEL_ODR_6_25
constexpr auto ACCEL_ODR = AccelOdr::Odr6_25;
#elifdef CONFIG_ICM45686_ACCEL_ODR_3_125
constexpr auto ACCEL_ODR = AccelOdr::Odr3_125;
#elifdef CONFIG_ICM45686_ACCEL_ODR_1_5625
constexpr auto ACCEL_ODR = AccelOdr::Odr1_5625;
#endif

enum class GyroFullScale : uint8_t {
	Fs4000 = 0b0000,
	Fs2000 = 0b0001,
	Fs1000 = 0b0010,
	Fs500 = 0b0011,
	Fs250 = 0b0100,
	Fs125 = 0b0101,
	Fs62_5 = 0b0110,
	Fs31_25 = 0b0111,
	Fs15_625 = 0b1000,
};

enum class AccelFullScale : uint8_t {
	Fs32g = 0b000,
	Fs16g = 0b001,
	Fs8g = 0b010,
	Fs4g = 0b011,
	Fs2g = 0b100,
};

struct Consts {
	static constexpr auto GYRO_ODR_HZ = [] constexpr -> float {
		switch (GYRO_ODR) {
			using enum GyroOdr;
			case Odr6400:
				return 6400;
			case Odr3200:
				return 3200;
			case Odr1600:
				return 1600;
			case Odr800:
				return 800;
			case Odr400:
				return 400;
			case Odr200:
				return 200;
			case Odr100:
				return 100;
			case Odr50:
				return 50;
			case Odr25:
				return 25;
			case Odr12_5:
				return 12.5f;
			case Odr6_25:
				return 6.25f;
			case Odr3_125:
				return 3.125f;
			case Odr1_5625:
				return 1.5625f;
		}
	}();
	static constexpr auto ACCEL_ODR_HZ = [] constexpr -> float {
		switch (ACCEL_ODR) {
			using enum AccelOdr;
			case Odr6400:
				return 6400;
			case Odr3200:
				return 3200;
			case Odr1600:
				return 1600;
			case Odr800:
				return 800;
			case Odr400:
				return 400;
			case Odr200:
				return 200;
			case Odr100:
				return 100;
			case Odr50:
				return 50;
			case Odr25:
				return 25;
			case Odr12_5:
				return 12.5f;
			case Odr6_25:
				return 6.25f;
			case Odr3_125:
				return 3.125f;
			case Odr1_5625:
				return 1.5625f;
		}
	}();
	static constexpr auto GYRO_FULL_SCALE = GyroFullScale::Fs4000;
	static constexpr auto ACCEL_FULL_SCALE = AccelFullScale::Fs32g;
	static constexpr auto GYRO_FULL_SCALE_DPS = [] constexpr -> float {
		switch (GYRO_FULL_SCALE) {
			using enum GyroFullScale;
			case Fs4000:
				return 4000;
			case Fs2000:
				return 2000;
			case Fs1000:
				return 1000;
			case Fs500:
				return 500;
			case Fs250:
				return 250;
			case Fs125:
				return 125;
			case Fs62_5:
				return 62.5f;
			case Fs31_25:
				return 31.25f;
			case Fs15_625:
				return 15.625f;
		}
	}();
	static constexpr auto ACCEL_FULL_SCALE_G = [] constexpr -> float {
		switch (ACCEL_FULL_SCALE) {
			using enum AccelFullScale;
			case Fs32g:
				return 32;
			case Fs16g:
				return 16;
			case Fs8g:
				return 8;
			case Fs4g:
				return 4;
			case Fs2g:
				return 2;
		}
	}();

	static constexpr float TEMP_ZERO = 25.0f;
	static constexpr float TEMP_SENSITIVITY = 128.0f;

	static constexpr float TEMP_ODR_HZ = std::max(GYRO_ODR_HZ, ACCEL_ODR_HZ);

	static constexpr uint8_t GYRO_BITS = 20;
	static constexpr uint8_t ACCEL_BITS = 19;

	static constexpr float GYRO_SENSITIVITY = GYRO_FULL_SCALE_DPS / (1 << GYRO_BITS);
	static constexpr float ACCEL_SENSITIVITY = ACCEL_FULL_SCALE_G / (1 << ACCEL_BITS);
};

}  // namespace
}  // namespace ICM45686

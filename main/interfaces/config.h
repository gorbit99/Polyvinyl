#pragma once

#include <cstdint>

#ifdef CONFIG_I2C_FREQUENCY_100K
constexpr uint32_t I2C_FREQUENCY = 100'000;
#elifdef CONFIG_I2C_FREQUENCY_400K
constexpr uint32_t I2C_FREQUENCY = 400'000;
#endif

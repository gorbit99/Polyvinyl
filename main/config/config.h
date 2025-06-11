#pragma once

#include "consts.h"

#ifdef CONFIG_BOARD_UNKNOWN
constexpr BoardType BOARD_TYPE = BoardType::Unknown;
#elifdef CONFIG_BOARD_SLIMEVR_LEGACY
constexpr BoardType BOARD_TYPE = BoardType::SlimeVRLegacy;
#elifdef CONFIG_BOARD_SLIMEVR_DEV
constexpr BoardType BOARD_TYPE = BoardType::SlimeVRDev;
#elifdef CONFIG_BOARD_NODEMCU
constexpr BoardType BOARD_TYPE = BoardType::NodeMcu;
#elifdef CONFIG_BOARD_CUSTOM
constexpr BoardType BOARD_TYPE = BoardType::Custom;
#elifdef CONFIG_BOARD_WROOM32
constexpr BoardType BOARD_TYPE = BoardType::WROOM32;
#elifdef CONFIG_BOARD_WEMOS_D1_MINI
constexpr BoardType BOARD_TYPE = BoardType::WEMOSD1Mini;
#elifdef CONFIG_BOARD_TTGO_TBASE
constexpr BoardType BOARD_TYPE = BoardType::TTGOTBase;
#elifdef CONFIG_BOARD_ESP01
constexpr BoardType BOARD_TYPE = BoardType::ESP01;
#elifdef CONFIG_BOARD_SLIMEVR
constexpr BoardType BOARD_TYPE = BoardType::SlimeVR;
#elifdef CONFIG_BOARD_LOLIN_C3_MINI
constexpr BoardType BOARD_TYPE = BoardType::LolinC3Mini;
#elifdef CONFIG_BOARD_BEETLE32C3
constexpr BoardType BOARD_TYPE = BoardType::Beetle32C3;
#elifdef CONFIG_BOARD_ESP32C3_DEVKIT_M1
constexpr BoardType BOARD_TYPE = BoardType::ESP32C3DevKitM1;
#elifdef CONFIG_BOARD_WEMOS_WROOM_02
constexpr BoardType BOARD_TYPE = BoardType::WEMOSWROOM02;
#elifdef CONFIG_BOARD_XIAO_ESP32C3
constexpr BoardType BOARD_TYPE = BoardType::XiaoEsp32C3;
#elifdef CONFIG_BOARD_ESP32C6_DEVKIT_C1
constexpr BoardType BOARD_TYPE = BoardType::ESP32C6DevKitC1;
#elifdef CONFIG_BOARD_GLOVE_IMU_SLIMEVR_DEV
constexpr BoardType BOARD_TYPE = BoardType::GloveIMUSlimeVRDev;
#elifdef CONFIG_BOARD_SLIMEVR_V1_2
constexpr BoardType BOARD_TYPE = BoardType::SlimeVR_V1_2;
#elifdef CONFIG_BOARD_ESP32S3_SUPERMINI
constexpr BoardType BOARD_TYPE = BoardType::ESP32S3Supermini;
#endif

#ifdef CONFIG_MCU_UNKNOWN
constexpr MCUType MCU_TYPE = MCUType::Unknown;
#elifdef CONFIG_MCU_ESP8266
constexpr MCUType MCU_TYPE = MCUType::ESP8266;
#elifdef CONFIG_MCU_ESP32
constexpr MCUType MCU_TYPE = MCUType::ESP32;
#elifdef CONFIG_MCU_ESP32C3
constexpr MCUType MCU_TYPE = MCUType::ESP32C3;
#endif

constexpr uint32_t PROTOCOL_VERSION = 20;

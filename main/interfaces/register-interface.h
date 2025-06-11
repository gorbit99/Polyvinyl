#pragma once

#include <concepts>
#include <cstdint>
#include <cstdlib>

template <typename Reg>
concept IsRegister = requires {
	{ Reg::reg } -> std::convertible_to<uint8_t>;
};

template <typename Reg>
concept IsRegisterWithDefault = IsRegister<Reg> && requires {
	{ Reg::value } -> std::convertible_to<uint8_t>;
};

class RegisterInterface {
public:
	template <IsRegister Reg, uint8_t Offset = 0>
	inline Reg readRegister() {
		Reg value;
		readBytes(Reg::reg + Offset, reinterpret_cast<uint8_t*>(&value), sizeof(Reg));
		return value;
	}

	template <IsRegister Reg, uint8_t Offset = 0>
	inline void writeRegister(Reg value) {
		writeBytes(Reg::reg + Offset, reinterpret_cast<uint8_t*>(&value), sizeof(Reg));
	}

	template <IsRegisterWithDefault Reg, uint8_t Offset = 0>
	inline void writeRegister() {
		writeRegister<Reg, Offset>(Reg::value);
	}

	virtual uint8_t readByte(uint8_t address) = 0;
	virtual void readBytes(uint8_t address, uint8_t* data, size_t size) = 0;
	virtual void writeByte(uint8_t address, uint8_t data) = 0;
	virtual void writeBytes(uint8_t address, const uint8_t* data, size_t size) = 0;
};

#pragma once

template <uint8_t Address>
struct Register {
	static constexpr uint8_t reg = Address;
};

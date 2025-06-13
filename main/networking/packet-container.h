#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "config/config.h"
#include "packets.h"

class PacketContainer {
public:
	PacketContainer(SendPacketId packetId, uint64_t packetIndex);
	PacketContainer(uint8_t* data, size_t length);

	template <typename T>
	void insert(T&& data) {
		auto start = bytes.size();
		bytes.resize(start + sizeof(T));
		memcpy(bytes.data() + start, &data, sizeof(T));
	}

	template <typename T>
	void insert(T* data, size_t count) {
		auto start = bytes.size();
		bytes.resize(start + sizeof(T) * count);
		memcpy(bytes.data() + start, data, sizeof(T) * count);
	}

	void insert(std::string&& data) {
		assert(data.size() <= 256);
		auto start = bytes.size();
		bytes.resize(start + 1 + data.size());
		bytes[start] = static_cast<uint8_t>(data.size());
		memcpy(bytes.data() + start + 1, data.data(), data.size());
	}

	template <typename T>
	T take() {
		T value;
		memcpy(&value, bytes.data(), sizeof(T));
		bytes.erase(bytes.begin() + sizeof(T));
		return value;
	}

	std::string takeString() {
		uint8_t length = bytes[0];
		std::string value;
		value.resize(length);
		memcpy(value.data(), bytes.data() + 1, length);
		bytes.erase(bytes.begin() + 1 + length);
		return value;
	}

	template <typename T>
	void take(T* value, size_t length) {
		memcpy(value, bytes.data(), sizeof(T) * length);
		bytes.erase(bytes.begin() + sizeof(T) * length);
	}

	size_t size() const;
	const uint8_t* data() const;

private:
	std::vector<uint8_t> bytes;
};

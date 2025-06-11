#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "utils/event-observer.h"

class Connection {
public:
	virtual ~Connection() = default;

	enum class ConnectionStatus {
		NotConnected,
		ConnectedToWifi,
		ConnectedToServer,
		Disconnected,
	};

	virtual void init() = 0;
	virtual void sendData(const uint8_t* data, size_t length) = 0;

	void onDataReceived(std::function<void(const uint8_t*, size_t)>&& callback);

	ConnectionStatus getConnectionStatus() const;

protected:
	void setConnectionStatus(ConnectionStatus status);

	EventObserver<const uint8_t*, size_t> dataReceivedEvent;

private:
	ConnectionStatus connectionStatus = ConnectionStatus::NotConnected;
};

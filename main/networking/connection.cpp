#include "connection.h"

Connection::ConnectionStatus Connection::getConnectionStatus() const {
	return connectionStatus;
}

void Connection::setConnectionStatus(Connection::ConnectionStatus status) {
	connectionStatus = status;
}

void Connection::onDataReceived(
	std::function<void(const uint8_t*, size_t)>&& callback
) {
	dataReceivedEvent.registerCallback(std::move(callback));
}

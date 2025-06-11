#pragma once

#include <functional>
#include <vector>

template <typename... CallbackParams>
class EventObserver {
public:
	void registerCallback(std::function<void(CallbackParams...)>&& callback) {
		callbacks.push_back(callback);
	}

	void emit(CallbackParams... params) const {
		for (auto& callback : callbacks) {
			callback(params...);
		}
	}

private:
	std::vector<std::function<void(CallbackParams...)>> callbacks;
};

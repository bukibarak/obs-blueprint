#pragma once
#include <string>
#include <unordered_map>

#include "multicast-delegate.h"

struct PinGraphicsOptions {
	bool showAsComboBox = false;
	std::unordered_map<std::string, std::string> comboBoxOptions{};
	multicastDelegate_ZeroParam onOptionsChanged;
};

struct NodeGraphicsOptions {
	bool isCompactNode = false;
	bool showName = true;
};

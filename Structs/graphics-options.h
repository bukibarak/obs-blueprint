#pragma once
#include <string>
#include <unordered_map>

struct PinGraphicsOptions {
	bool showAsComboBox = false;
	std::unordered_map<std::string, std::string> comboBoxOptions{};

	static const PinGraphicsOptions DEFAULT;
};

struct NodeGraphicsOptions {
	bool isCompactNode = false;
	bool showName = true;
};

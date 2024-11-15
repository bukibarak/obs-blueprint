#include "pin-helper.h"

#include <iomanip>
#include <sstream>

#include "Core/obs-blueprint-pin.h"
#include "GUI/obs-graphics-pin.h"

std::string PinConverter::AsString(OBSBlueprintPin *pin)
{
	PinType type = pin->getPinType();

	if(type == BYTE_PIN)	return std::to_string(pin->getValue<uint8_t>());
	if(type == INT_PIN)		return std::to_string(pin->getValue<int32_t>());
	if(type == CHAR_PIN)	return {1, pin->getValue<char>()};
	if(type == STRING_PIN)	return pin->getValue<std::string>();
	if(type == FLOAT_PIN) {
		std::ostringstream oss;
		oss << std::setprecision(8) << std::noshowpoint << pin->getValue<float>();
		return oss.str();
	}
	if(type == COLOR_PIN) {
		std::stringstream ss;
		ss << std::hex << std::uppercase << pin->getValue<uint32_t>();
		return ss.str();
	}

	return "";
}

void PinConverter::FromString(OBSBlueprintPin *pin,
	const std::string &strValue)
{
	PinType type = pin->getPinType();

	if(type == BYTE_PIN) {
		pin->setValue<uint8_t>(std::stoi(strValue));
	}
	else if(type == INT_PIN) {
		pin->setValue(std::stoi(strValue));
	}
	else if(type == CHAR_PIN) {
		pin->setValue(strValue[0]);
	}
	else if(type == STRING_PIN) {
		pin->setValue(strValue);
	}
	else if(type == FLOAT_PIN) {
		pin->setValue(std::stof(strValue));
	}
	else if(type == COLOR_PIN) {
		pin->setValue(std::stoul(strValue, nullptr, 16));
	}
}


const QColor & PinColors::Get(const OBSGraphicsPin *pin)
{
	return Get(pin->getBlueprintPin());
}

const QColor & PinColors::Get(const OBSBlueprintPin *pin)
{
	return Get(pin->getPinType());
}

const QColor & PinColors::Get(const PinType &type)
{
	if(const auto it = Color.find(type); it != Color.end()) return it->second;
	return Default;
}

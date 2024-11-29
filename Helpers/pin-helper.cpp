#include "pin-helper.h"

#include <iomanip>
#include <QPainter>
#include <sstream>

#include "Structs/video-frame.h"

std::string TypeConverter::AsString(OBSBlueprintPin *pin)
{

	switch (pin->getPinType()) {
	case EXECUTION_PIN:
		return "EXECUTE";
	case BOOLEAN_PIN:
		return std::to_string(pin->getValue<bool>());
	case BYTE_PIN:
		return std::to_string(pin->getValue<uint8_t>());
	case INT_PIN:
		return std::to_string(pin->getValue<int32_t>());
	case FLOAT_PIN: {
		std::stringstream ss;
		ss << std::setprecision(8) << std::noshowpoint << pin->getValue<float>();
		return ss.str();
	}

	case CHAR_PIN:
		return {1, pin->getValue<char>()};
	case STRING_PIN:
		return pin->getValue<std::string>();
	case COLOR_PIN: {
		std::stringstream ss;
		ss << std::hex << std::uppercase << pin->getValue<uint32_t>();
		return ss.str();
	}

	case VIDEO_PIN: {
		std::stringstream ss;
		video_frame* frame = pin->getValuePtr<video_frame>();
		ss << frame->width << "x" << frame->height;
		return ss.str();
	}
	case AUDIOVIDEO_PIN:
		// TODO
	case AUDIO_PIN:
		// TODO
	default:
		return "";
	}
}

std::string TypeConverter::AsString(OBSBlueprintVariable *variable)
{
	std::stringstream ss;
	switch (variable->getPinType()) {
	case VIDEO_PIN:
		ss << variable->getValue<video_frame>().width << "x" << variable->getValue<video_frame>().height;
		return ss.str();
	case BOOLEAN_PIN:
		return std::to_string(variable->getValue<bool>());
	case BYTE_PIN:
		return std::to_string(variable->getValue<uint8_t>());
	case INT_PIN:
		return std::to_string(variable->getValue<int32_t>());
	case FLOAT_PIN:
		ss << std::setprecision(8) << std::noshowpoint << variable->getValue<float>();
		return ss.str();
	case CHAR_PIN:
		return {1, variable->getValue<char>()};
	case STRING_PIN:
		return variable->getValue<std::string>();
	case COLOR_PIN:
		ss << std::hex << std::uppercase << variable->getValue<uint32_t>();
		return ss.str();
	default:
		return "";
	}
}

void TypeConverter::FromString(OBSBlueprintPin *pin,
                               const std::string &strValue)
{
	switch (const PinType& type = pin->getPinType()) {
	case EXECUTION_PIN:
		if(strValue == "EXECUTE" && pin->getValuePtr<std::function<void()>>()) {
			pin->getValue<std::function<void()>>().operator()();
		}
		break;
	case BOOLEAN_PIN:
		pin->setValue<bool>(strValue != "0");
		break;
	case BYTE_PIN:
		pin->setValue<uint8_t>(std::stoi(strValue));
		break;
	case INT_PIN:
		pin->setValue<int32_t>(std::stoi(strValue));
		break;
	case FLOAT_PIN:
		pin->setValue<float>(std::stof(strValue));
		break;
	case CHAR_PIN:
		pin->setValue<char>(strValue[0]);
		break;
	case STRING_PIN:
		pin->setValue<std::string>(strValue);
		break;
	case COLOR_PIN:
		pin->setValue<uint32_t>(std::stoul(strValue, nullptr, 16));
		break;
	default:
		GError("Unable to set pin value '%s' from string, undefined type %s", pin->getDisplayName(), PinName[type]);
	}
}

void TypeConverter::FromString(OBSBlueprintVariable *variable,
	const std::string &strValue)
{
	switch (const PinType& type = variable->getPinType()) {
	case BOOLEAN_PIN:
		variable->setValue<bool>(strValue != "0");
		break;
	case BYTE_PIN:
		variable->setValue<uint8_t>(std::stoi(strValue));
		break;
	case INT_PIN:
		variable->setValue<int32_t>(std::stoi(strValue));
		break;
	case FLOAT_PIN:
		variable->setValue<float>(std::stof(strValue));
		break;
	case CHAR_PIN:
		variable->setValue<char>(strValue[0]);
		break;
	case STRING_PIN:
		variable->setValue<std::string>(strValue);
		break;
	case COLOR_PIN:
		variable->setValue<uint32_t>(std::stoul(strValue, nullptr, 16));
		break;
	default:
		GError("Unable to set variable value '%s' from string, undefined type %s", variable->getDisplayName(), PinName[type]);
	}
}

const QPixmap & PinColors::ConnectedIcon()
{
	if(ConnectedPixmap.isNull()) {
		ConnectedPixmap = QPixmap(10,10);
		ConnectedPixmap.fill(Qt::transparent);
		QPainter painter(&ConnectedPixmap);
		painter.setBrush(Qt::green);
		painter.drawEllipse(0,0,10,10);
	}
	return ConnectedPixmap;
}

const QPixmap & PinColors::DisconnectedIcon()
{
	if(DisconnectedPixmap.isNull()) {
		DisconnectedPixmap = QPixmap(10,10);
		DisconnectedPixmap.fill(Qt::transparent);
		QPainter painter(&DisconnectedPixmap);
		painter.setBrush(Qt::red);
		painter.drawEllipse(0,0,10,10);
	}
	return DisconnectedPixmap;
}

std::unordered_map<PinType, QPixmap> PinColors::Pixmap{};
std::unordered_map<PinType, QIcon> PinColors::Icon{};
QPixmap PinColors::ConnectedPixmap{};
QPixmap PinColors::DisconnectedPixmap{};


#include "pin-helper.h"

#include <iomanip>
#include <QIcon>
#include <QPainter>

#include "Core/obs-blueprint-pin.h"
#include "Core/obs-blueprint-variable.h"
#include "GUI/obs-graphics-pin.h"
#include "Structs/obs-frame.h"

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
		OBSFrame* frame = pin->getValuePtr<OBSFrame>();
		ss << frame->width() << "x" << frame->height();
		return ss.str();
	}
	case AUDIOVIDEO_PIN:
		// TODO
	case AUDIO_PIN:
		// TODO
	case UNKNOWN_PIN:
	case ANY_PIN:
	default:
		return "";
	}
}

std::string TypeConverter::AsString(OBSBlueprintVariable *variable)
{

	switch (variable->getPinType()) {
	case VIDEO_PIN: {
		std::stringstream ss;
		OBSFrame* frame = variable->getValuePtr<OBSFrame>();
		ss << frame->width() << "x" << frame->height();
		return ss.str();
	}
	case BOOLEAN_PIN:
		return std::to_string(variable->getValue<bool>());
	case BYTE_PIN:
		return std::to_string(variable->getValue<uint8_t>());
	case INT_PIN:
		return std::to_string(variable->getValue<int32_t>());
	case FLOAT_PIN: {
		std::stringstream ss;
		ss << std::setprecision(8) << std::noshowpoint << variable->getValue<float>();
		return ss.str();
	}
	case CHAR_PIN:
		return {1, variable->getValue<char>()};
	case STRING_PIN:
		return variable->getValue<std::string>();
	case COLOR_PIN: {
		std::stringstream ss;
		ss << std::hex << std::uppercase << variable->getValue<uint32_t>();
		return ss.str();
	}

	case AUDIOVIDEO_PIN:
		// TODO
	case AUDIO_PIN:
		// TODO
	case EXECUTION_PIN:
		// TODO
	case ANY_PIN:
	case UNKNOWN_PIN:
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
	case UNKNOWN_PIN:
	case AUDIOVIDEO_PIN:
	case AUDIO_PIN:
	case VIDEO_PIN:
	case ANY_PIN:
	default:
		GError("Unable to set pin value '%s' from string, undefined type %s", pin->getDisplayName(), EnumStr::PinType[type]);
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
	case UNKNOWN_PIN:
	case AUDIOVIDEO_PIN:
	case AUDIO_PIN:
	case VIDEO_PIN:
	case EXECUTION_PIN:
	case ANY_PIN:
	default:
		GError("Unable to set variable value '%s' from string, undefined type %s", variable->getDisplayName(), EnumStr::PinType[type]);
	}
}

/***********************************************************************************************************************
 **********************************************************************************************************************/

const QColor & PinColors::Get(const PinType &type)
{
	if(const auto it = Color.find(type); it != Color.end()) return it->second;
	return Default;
}

const QColor & PinColors::Get(const OBSBlueprintPin *pin)
{
	return Get(pin->getPinType());
}

const QColor & PinColors::Get(const OBSGraphicsPin *pin)
{
	return Get(pin->getBlueprintPin());
}

const QIcon & PinColors::GetIcon(const PinType &type)
{
	if(const auto it = Icon.find(type); it != Icon.end()) {
		return it->second;
	}
	if(const auto it = Pixmap.find(type); it != Pixmap.end()) {
		Icon[type] = QIcon(it->second);
		return Icon[type];
	}
	Pixmap[type] = QPixmap(10,10);
	Pixmap[type].fill(Get(type));
	Icon[type] = QIcon(Pixmap[type]);
	return Icon[type];
}

const QPixmap & PinColors::GetPixmap(const PinType &type)
{
	if(const auto it = Pixmap.find(type); it != Pixmap.end()) {
		return it->second;
	}
	Pixmap[type] = QPixmap(10,10);
	Pixmap[type].fill(Get(type));
	return Pixmap[type];
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

const std::unordered_map<PinType, QColor> PinColors::Color {
	{UNKNOWN_PIN, QColor::fromRgbF(0.750000f,0.600000f,0.400000f)},
	{AUDIOVIDEO_PIN, QColor::fromRgbF(0.100000f,0.000000f,0.500000f)},
	{AUDIO_PIN, QColor::fromRgbF(1.000000f,0.040000f,0.040000f)},
	{VIDEO_PIN, QColor::fromRgbF(0.000000f,0.400000f,0.910000f)},
	{EXECUTION_PIN, QColor::fromRgbF(1.000000f,1.000000f,1.000000f)},
	{BOOLEAN_PIN, QColor::fromRgbF(0.300000f,0.000000f,0.000000f)},
	{BYTE_PIN, QColor::fromRgbF(0.000000f,0.160000f,0.131270f)},
	{INT_PIN, QColor::fromRgbF(0.013575f,0.770000f,0.42960f)},
	{FLOAT_PIN, QColor::fromRgbF(0.039216f,0.666667f,0.000000f)},
	{CHAR_PIN, QColor::fromRgbF(0.607717f,0.224984f,1.000000f)},
	{STRING_PIN, QColor::fromRgbF(1.000000f,0.000000f,0.660537f)},
	{COLOR_PIN, QColor::fromRgbF(0.000000f,0.100000f,0.600000f)},
	{ANY_PIN, QColor::fromRgbF(0.220000f,0.195800f,0.195800f)},
};
std::unordered_map<PinType, QPixmap> PinColors::Pixmap{};
std::unordered_map<PinType, QIcon> PinColors::Icon{};
QPixmap PinColors::ConnectedPixmap{};
QPixmap PinColors::DisconnectedPixmap{};


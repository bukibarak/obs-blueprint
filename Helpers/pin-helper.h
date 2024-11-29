#pragma once
#include <QIcon>

#include "Core/obs-blueprint-variable.h"
#include "GUI/obs-graphics-pin.h"

namespace TypeConverter {
	std::string AsString(OBSBlueprintPin* pin);
	std::string AsString(OBSBlueprintVariable* variable);
	void FromString(OBSBlueprintPin* pin, const std::string& strValue);
	void FromString(OBSBlueprintVariable* variable, const std::string& strValue);
}

class PinColors {
public:
	inline static const std::unordered_map<PinType, QColor> Color{
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

	inline static const QColor Default{Qt::black};

	static const QColor& Get(const PinType& type)
	{
		if(const auto it = Color.find(type); it != Color.end()) return it->second;
		return Default;
	}
	static const QColor& Get(const OBSBlueprintPin* pin)
	{
		return Get(pin->getPinType());
	}
	static const QColor& Get(const OBSGraphicsPin* pin)
	{
		return Get(pin->getBlueprintPin());
	}

	static const QIcon& GetIcon(const PinType& type)
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

	static const QPixmap& GetPixmap(const PinType& type)
	{
		if(const auto it = Pixmap.find(type); it != Pixmap.end()) {
			return it->second;
		}
		Pixmap[type] = QPixmap(10,10);
		Pixmap[type].fill(Get(type));
		return Pixmap[type];
	}

	static const QPixmap& ConnectedIcon();
	static const QPixmap& DisconnectedIcon();

private:
	static std::unordered_map<PinType, QPixmap> Pixmap;
	static std::unordered_map<PinType, QIcon> Icon;

	static QPixmap ConnectedPixmap;
	static QPixmap DisconnectedPixmap;
};



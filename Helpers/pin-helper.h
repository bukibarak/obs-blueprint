#pragma once
#include <unordered_map>
#include <QColor>
#include <string>

#include "Core/pin-type.h"

class OBSGraphicsPin;
class OBSBlueprintPin;

namespace PinConverter {
	std::string AsString(OBSBlueprintPin* pin);
	void FromString(OBSBlueprintPin* pin, const std::string& strValue);
}

namespace PinColors {
	static inline const std::unordered_map<PinType, QColor> Color{
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

	static inline const QColor Default{Qt::black};

	const QColor& Get(const OBSGraphicsPin* pin);
	const QColor& Get(const OBSBlueprintPin* pin);
	inline const QColor& Get(const PinType& type);
}

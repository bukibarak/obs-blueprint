#pragma once
#include <string>
#include <unordered_map>
#include <QColor>

class OBSGraphicsPin;
enum PinType : unsigned char;
class QPixmap;
class QIcon;
class OBSBlueprintPin;
class OBSBlueprintVariable;

/** Includes helpers functions to convert pins or variables value from/to string. */
namespace TypeConverter {
/**
 * Convert the value of a \c OBSBlueprintPin to \c std::string \n
 * When it cannot be converted, return empty string.
 * @param pin The pin.
 * @return The pin value converted to \c std::string, or empty string if it cannot be converted.
 */
std::string AsString(OBSBlueprintPin* pin);

/**
* Convert the value of a \c OBSBlueprintVariable to \c std::string \n
 * When it cannot be converted, return empty string.
 * @param variable The variable.
 * @return The variable value converted to \c std::string, or empty string if it cannot be converted.
 */
std::string AsString(OBSBlueprintVariable* variable);

/**
 * Convert and store the value of a \c std::string to the \c OBSBlueprintPin \n
 * Does nothing if the value cannot be converted to the pin variable type.
 * @param pin The pin.
 * @param strValue The value to be stored in the pin, if possible.
 */
void FromString(OBSBlueprintPin* pin, const std::string& strValue);

/**
 * Convert and store the value of a \c std::string to the \c OBSBlueprintVariable \n
 *  Does nothing if the value cannot be converted to the variable type.
 * @param variable The variable.
 * @param strValue The value to be stored in the variable, if possible.
 */
void FromString(OBSBlueprintVariable* variable, const std::string& strValue);
}

class PinColors {
public:

	/** Get the \c QColor associated to the provided type. @see \c PinColors::Color */
	static const QColor& Get(const PinType& type);

	/** Get the \c QColor associated to the provided \c OBSBlueprintPin type. @see \c PinColors::Color */
	static const QColor& Get(const OBSBlueprintPin* pin);

	/** Get the \c QColor associated to the provided \c OBSGraphicsPin type. @see \c PinColors::Color */
	static const QColor& Get(const OBSGraphicsPin* pin);

	/** Get the \c QColor as a 10x10 \c QIcon associated to the provided type. @see \c PinColors::Color */
	static const QIcon& GetIcon(const PinType& type);

	/** Get the \c QColor as a 10x10 \c QPixmap associated to the provided type. @see \c PinColors::Color */
	static const QPixmap& GetPixmap(const PinType& type);


	/** Get the \c QPixmap icon used when the pin is connected (10x10 green circle). */
	static const QPixmap& ConnectedIcon();

	/** Get the \c QPixmap icon used when the pin is \b not connected (10x10 red circle). */
	static const QPixmap& DisconnectedIcon();

	/** Default color used when PinType is not associated to a color. @see \c PinColors::Color */
	inline static const QColor Default{Qt::black};

	/** Map of all \c QColor used according to the pin type. \n I use the same color as Unreal Engine Blueprint colors. */
	static const std::unordered_map<PinType, QColor> Color;

private:
	static std::unordered_map<PinType, QPixmap> Pixmap;
	static std::unordered_map<PinType, QIcon> Icon;

	static QPixmap ConnectedPixmap;
	static QPixmap DisconnectedPixmap;
};



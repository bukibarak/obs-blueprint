#pragma once
#include <QGraphicsObject>
#include "Core/obs-blueprint-pin.h"

class GUINode;

#define GUI_PIN_SIZE 50
#define GUI_PIN_CORNER_SIZE 7

class GUIPin : public QGraphicsObject {
public:
	enum { Type = UserType + 1 };
	GUIPin(OBSBlueprintInputPin *pin, QGraphicsItem* parent = nullptr);
	GUIPin(OBSBlueprintOutputPin *pin, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

	OBSBlueprintPin* getBlueprintPin() const;
	OBSBlueprintInputPin* getInputPin() const {return inputPin;}
	OBSBlueprintOutputPin* getOutputPin() const {return outputPin;}
	bool isInputPin() const {return inputPin != nullptr;}

	static bool IsVerticalPin(PinType type) {return type == AUDIO_PIN || type == VIDEO_PIN || type == AUDIOVIDEO_PIN;}
	static const QColor& GetPinColor(const PinType& type);

	GUINode* getParentNode() const {return parentNode;}

private:

	GUINode* parentNode;

	void drawPinContent(QPainter *painter, PinType pinType);
	void drawTopPinCorners(QPainter *painter);
	void drawBottomPinCorners(QPainter *painter);
	void drawLeftPinCorners(QPainter *painter);
	void drawRightPinCorners(QPainter *painter);

	const QRect leftCorner{0, 0, GUI_PIN_CORNER_SIZE, GUI_PIN_SIZE};
	const QRect rightCorner{GUI_PIN_SIZE - GUI_PIN_CORNER_SIZE, 0, GUI_PIN_CORNER_SIZE, GUI_PIN_SIZE};
	const QRect topCorner{0, 0, GUI_PIN_SIZE, GUI_PIN_CORNER_SIZE};
	const QRect bottomCorner{0, GUI_PIN_SIZE - GUI_PIN_CORNER_SIZE, GUI_PIN_SIZE, GUI_PIN_CORNER_SIZE};
	OBSBlueprintInputPin *inputPin = nullptr;
	OBSBlueprintOutputPin *outputPin = nullptr;

	inline static std::map<PinType, QColor> pinColors{
		{UNKNOWN_PIN, Qt::gray},
		{AUDIOVIDEO_PIN, Qt::magenta},
		{AUDIO_PIN, Qt::darkRed},
		{VIDEO_PIN, Qt::blue},
		{BYTE_PIN, Qt::darkCyan},
		{INT_PIN, Qt::cyan},
		{FLOAT_PIN, Qt::green},
		{CHAR_PIN, Qt::darkYellow},
		{STRING_PIN, Qt::yellow},
		{COLOR_PIN, Qt::white},
	};
	inline static QColor defaultPinColor{Qt::black};
};

inline const QColor& GUIPin::GetPinColor(const PinType& type)
{
	if(const auto it = pinColors.find(type); it != pinColors.end()) return it->second;
	return defaultPinColor;
}

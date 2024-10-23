#pragma once
#include <QGraphicsObject>

#include "Core/obs-blueprint-pin.h"

#define GUI_PIN_SIZE 50
#define GUI_PIN_CORNER_SIZE 5

class GUIPin : public QGraphicsObject {
public:
	GUIPin(OBSBlueprintInputPin *pin, QGraphicsItem* parent = nullptr);
	GUIPin(OBSBlueprintOutputPin *pin, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	static bool IsVerticalPin(PinType type) {return type == AUDIO_PIN || type == VIDEO_PIN || type == AUDIOVIDEO_PIN;}

private:

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
};

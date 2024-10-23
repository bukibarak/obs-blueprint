#include "gui-pin.h"

#include <QPainter>

GUIPin::GUIPin(OBSBlueprintInputPin *pin, QGraphicsItem *parent) : QGraphicsObject(parent),  inputPin(pin)
{

}

GUIPin::GUIPin(OBSBlueprintOutputPin *pin, QGraphicsItem *parent) : QGraphicsObject(parent),  outputPin(pin)
{
}

QRectF GUIPin::boundingRect() const
{
	return {0, 0, GUI_PIN_SIZE, GUI_PIN_SIZE};
}

void GUIPin::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	bool isInputPin = inputPin != nullptr;
	OBSBlueprintPin* pin = inputPin;
	if(!isInputPin) pin = outputPin;
	PinType pinType = pin->getPinType();

	drawPinContent(painter, pinType);
	painter->setBrush(pin->isConnected() ? Qt::green : Qt::red);
	if(isInputPin && IsVerticalPin(pinType)) drawTopPinCorners(painter);
	else if(isInputPin) drawLeftPinCorners(painter);
	else if(IsVerticalPin(pinType)) drawBottomPinCorners(painter);
	else drawRightPinCorners(painter);
}

void GUIPin::drawPinContent(QPainter *painter, PinType pinType)
{
	QColor color;
	switch (pinType) {
	case UNKNOWN_PIN:
		color = Qt::gray;
		break;
	case AUDIOVIDEO_PIN:
		color = Qt::darkMagenta;
		break;
	case AUDIO_PIN:
		color = Qt::darkRed;
		break;
	case VIDEO_PIN:
		color = Qt::blue;
		break;
	case BYTE_PIN:
		color = Qt::darkCyan;
		break;
	case INT_PIN:
		color = Qt::cyan;
		break;
	case FLOAT_PIN:
		color = Qt::green;
		break;
	case CHAR_PIN:
		color = Qt::darkYellow;
		break;
	case STRING_PIN:
		color = Qt::yellow;
		break;
	case COLOR_PIN:
		color = Qt::white;
		break;
	}
	painter->fillRect(QRect(0, 0, GUI_PIN_SIZE, GUI_PIN_SIZE), color);
}

void GUIPin::drawTopPinCorners(QPainter *painter)
{
	painter->fillRect(leftCorner, painter->brush());
	painter->fillRect(rightCorner, painter->brush());
	painter->fillRect(bottomCorner, painter->brush());
}

void GUIPin::drawBottomPinCorners(QPainter *painter)
{
	painter->fillRect(leftCorner, painter->brush());
	painter->fillRect(rightCorner, painter->brush());
	painter->fillRect(topCorner, painter->brush());
}

void GUIPin::drawLeftPinCorners(QPainter *painter)
{
	painter->fillRect(rightCorner, painter->brush());
	painter->fillRect(topCorner, painter->brush());
	painter->fillRect(bottomCorner, painter->brush());
}

void GUIPin::drawRightPinCorners(QPainter *painter)
{
	painter->fillRect(leftCorner, painter->brush());
	painter->fillRect(topCorner, painter->brush());
	painter->fillRect(bottomCorner, painter->brush());
}

#include "gui-pin.h"

#include <QPainter>

#include "gui-node.h"

GUIPin::GUIPin(OBSBlueprintInputPin *pin, QGraphicsItem *parent) : QGraphicsObject(parent),  parentNode(dynamic_cast<GUINode*>(parent)), inputPin(pin)
{
}

GUIPin::GUIPin(OBSBlueprintOutputPin *pin, QGraphicsItem *parent) : QGraphicsObject(parent),  parentNode(dynamic_cast<GUINode*>(parent)), outputPin(pin)
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
	OBSBlueprintPin* pin = getBlueprintPin();
	PinType pinType = pin->getPinType();

	drawPinContent(painter, pinType);
	painter->setBrush(pin->isConnected() ? Qt::green : Qt::red);
	if(isInputPin && IsVerticalPin(pinType)) drawTopPinCorners(painter);
	else if(isInputPin) drawLeftPinCorners(painter);
	else if(IsVerticalPin(pinType)) drawBottomPinCorners(painter);
	else drawRightPinCorners(painter);
}

OBSBlueprintPin * GUIPin::getBlueprintPin() const
{
	if(inputPin != nullptr) return inputPin;
	return outputPin;
}


void GUIPin::connect(GUIConnector *connector)
{
	this->connector = connector;
	update();
}

void GUIPin::disconnect()
{
	connector = nullptr;
	update();
}

void GUIPin::drawPinContent(QPainter *painter, PinType pinType)
{
	painter->fillRect(QRect(0, 0, GUI_PIN_SIZE, GUI_PIN_SIZE), GetPinColor(pinType));
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

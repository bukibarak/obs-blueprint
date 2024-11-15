#include "obs-graphics-pin.h"

#include <QPainter>

#include "obs-graphics-node.h"
#include "Helpers/pin-helper.h"

OBSGraphicsPin::OBSGraphicsPin(OBSBlueprintInputPin *pin, QGraphicsItem *parent) : QGraphicsObject(parent),  parentNode(dynamic_cast<OBSGraphicsNode*>(parent)), inputPin(pin)
{
}

OBSGraphicsPin::OBSGraphicsPin(OBSBlueprintOutputPin *pin, QGraphicsItem *parent) : QGraphicsObject(parent),  parentNode(dynamic_cast<OBSGraphicsNode*>(parent)), outputPin(pin)
{
}

QRectF OBSGraphicsPin::boundingRect() const
{
	return {0, 0, GUI_PIN_SIZE, GUI_PIN_SIZE};
}

void OBSGraphicsPin::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	bool isInputPin = inputPin != nullptr;
	bool isConnected = isInputPin ? inputPin->isConnected() : outputPin->isConnected();
	PinType pinType = getBlueprintPin()->getPinType();

	drawPinContent(painter, pinType);
	painter->setBrush(isConnected ? Qt::green : Qt::red);
	if(isInputPin && IsVerticalPin(pinType)) drawTopPinCorners(painter);
	else if(isInputPin) drawLeftPinCorners(painter);
	else if(IsVerticalPin(pinType)) drawBottomPinCorners(painter);
	else drawRightPinCorners(painter);
}

OBSBlueprintPin * OBSGraphicsPin::getBlueprintPin() const
{
	if(inputPin != nullptr) return inputPin;
	return outputPin;
}


void OBSGraphicsPin::connect(OBSGraphicsConnector *connector)
{
	connectors.push_back(connector);
	if(connectors.size() == 1) {
		if(onConnectionStateChanged) onConnectionStateChanged->operator()(true);
		update();
	}
}

void OBSGraphicsPin::disconnect(OBSGraphicsConnector* connector)
{
	connectors.removeOne(connector);
	if(connectors.isEmpty()) {
		if(onConnectionStateChanged) onConnectionStateChanged->operator()(false);
		update();
	}
}

void OBSGraphicsPin::drawPinContent(QPainter *painter, PinType pinType)
{
	painter->fillRect(QRect(0, 0, GUI_PIN_SIZE, GUI_PIN_SIZE), PinColors::Get(pinType));
}

void OBSGraphicsPin::drawTopPinCorners(QPainter *painter)
{
	painter->fillRect(leftCorner, painter->brush());
	painter->fillRect(rightCorner, painter->brush());
	painter->fillRect(bottomCorner, painter->brush());
}

void OBSGraphicsPin::drawBottomPinCorners(QPainter *painter)
{
	painter->fillRect(leftCorner, painter->brush());
	painter->fillRect(rightCorner, painter->brush());
	painter->fillRect(topCorner, painter->brush());
}

void OBSGraphicsPin::drawLeftPinCorners(QPainter *painter)
{
	painter->fillRect(rightCorner, painter->brush());
	painter->fillRect(topCorner, painter->brush());
	painter->fillRect(bottomCorner, painter->brush());
}

void OBSGraphicsPin::drawRightPinCorners(QPainter *painter)
{
	painter->fillRect(leftCorner, painter->brush());
	painter->fillRect(topCorner, painter->brush());
	painter->fillRect(bottomCorner, painter->brush());
}

#include "gui-connector.h"

#include <QPainter>

#include "gui-pin.h"

GUIConnector::GUIConnector(OBSBlueprintConnector *connector, GUIPin *fromPin,
                           GUIPin *toPin, QGraphicsItem *parent) : QGraphicsObject(parent), connector(connector), from(fromPin), to(toPin)
{
	redrawConnector();

	pen = QPen(GUIPin::GetPinColor(fromPin->getBlueprintPin()->getPinType()));
	pen.setWidth(10);
}

void GUIConnector::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	qreal xMin = left->scenePos().x() + GUI_PIN_SIZE/2;
	qreal xMax = right->scenePos().x() + GUI_PIN_SIZE/2;

	qreal yMin = top->scenePos().y() + GUI_PIN_SIZE/2;
	qreal yMax = bottom->scenePos().y() + GUI_PIN_SIZE/2;


	painter->setPen(pen);
	if(xMin == xMax && yMin == yMax) {
		painter->drawLine(0, 0, 0, 0);
	}
	else if(xMin == xMax) {
		painter->drawLine(0, 0, 0, qAbs(yMax - yMin));
	}
	else if(yMin == yMax) {
		painter->drawLine(0, 0, qAbs(xMax - xMin), 0);
	}
	else {
		if(left == top) {
			painter->drawLine(0, 0, qAbs(xMax - xMin), qAbs(yMax - yMin));
		}
		else {
			painter->drawLine(0, qAbs(yMax - yMin), qAbs(xMax - xMin), 0);
		}
	}
}

void GUIConnector::redrawConnector()
{
	prepareGeometryChange();
	left = from->scenePos().x() < to->scenePos().x() ? from : to;
	right = left == from ? to : from;
	qreal xMin = left->scenePos().x() + GUI_PIN_SIZE/2;
	qreal xMax = right->scenePos().x() + GUI_PIN_SIZE/2;

	top = from->scenePos().y() < to->scenePos().y() ? from : to;
	bottom = top == from ? to : from;
	qreal yMin = top->scenePos().y() + GUI_PIN_SIZE/2;
	qreal yMax = bottom->scenePos().y() + GUI_PIN_SIZE/2;

	if(xMin == xMax && yMin == yMax) {
		// Both pins are in the same position, dunno what to do...
	}
	else if(xMin == xMax) {
		bounds = QRectF(0, 0, 1, qAbs(yMax - yMin));
	}
	else if(yMin == yMax) {
		bounds = QRectF(0, 0, qAbs(xMax - xMin), 1);
	}
	else {
		bounds = QRectF(0, 0, abs(xMax - xMin), abs(yMax - yMin));
	}

	setPos(xMin, yMin);
}

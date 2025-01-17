#include "obs-graphics-connector.h"

#include <QPainter>

#include "obs-graphics-pin.h"
#include "Helpers/pin-helper.h"

OBSGraphicsConnector::OBSGraphicsConnector(OBSBlueprintConnector *connector, OBSGraphicsPin *fromPin,
                                           OBSGraphicsPin *toPin, QGraphicsItem *parent) : QGraphicsObject(parent), connector(connector), from(fromPin), to(toPin)
{
	redrawConnector();

	pen = QPen(PinColors::Get(fromPin));
	pen.setWidth(10);
}

void OBSGraphicsConnector::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	qreal xMin = left->scenePos().x() + left->sceneBoundingRect().width()/2;
	qreal xMax = right->scenePos().x() + right->sceneBoundingRect().width()/2;

	qreal yMin = top->scenePos().y() + top->sceneBoundingRect().height()/2;
	qreal yMax = bottom->scenePos().y() + bottom->sceneBoundingRect().height()/2;


	painter->setPen(pen);
	if(qFuzzyCompare(xMin, xMax) && qFuzzyCompare(yMin, yMax)) {
		painter->drawLine(0, 0, 0, 0);
	}
	else if(qFuzzyCompare(xMin, xMax)) {
		painter->drawLine(0, 0, 0, qAbs(yMax - yMin));
	}
	else if(qFuzzyCompare(yMin, yMax)) {
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

OBSGraphicsNode * OBSGraphicsConnector::getOtherNode(const OBSGraphicsNode* self) const
{
	return self == from->getParentNode() ? to->getParentNode() : self == to->getParentNode() ? from->getParentNode() : nullptr;
}

void OBSGraphicsConnector::redrawConnector()
{
	prepareGeometryChange();
	left = from->scenePos().x() + from->sceneBoundingRect().width()/2 < to->scenePos().x() + to->sceneBoundingRect().width()/2 ? from : to;
	right = left == from ? to : from;
	qreal xMin = left->scenePos().x() + left->sceneBoundingRect().width()/2;
	qreal xMax = right->scenePos().x() + right->sceneBoundingRect().width()/2;

	top = from->scenePos().y() + from->sceneBoundingRect().height()/2 < to->scenePos().y() + to->sceneBoundingRect().height()/2 ? from : to;
	bottom = top == from ? to : from;
	qreal yMin = top->scenePos().y() + top->sceneBoundingRect().height()/2;
	qreal yMax = bottom->scenePos().y() + bottom->sceneBoundingRect().height()/2;

	if(qFuzzyCompare(xMin, xMax) && qFuzzyCompare(yMin, yMax)) {
		// Both pins are in the same position, dunno what to do...
	}
	else if(qFuzzyCompare(xMin, xMax)) {
		bounds = QRectF(0, 0, 1, qAbs(yMax - yMin));
	}
	else if(qFuzzyCompare(yMin, yMax)) {
		bounds = QRectF(0, 0, qAbs(xMax - xMin), 1);
	}
	else {
		bounds = QRectF(0, 0, qAbs(xMax - xMin), qAbs(yMax - yMin));
	}

	setPos(xMin, yMin);
}

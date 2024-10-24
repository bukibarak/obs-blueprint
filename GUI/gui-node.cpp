#include "gui-node.h"

#include <QPainter>

#include "Core/obs-blueprint-node.h"

GUINode::GUINode(OBSBlueprintNode *node, QGraphicsItem *parent) : QGraphicsObject(parent), node(node)
{
	int leftPinPlaced = 0;
	int rightPinPlaced = 0;
	std::vector<OBSBlueprintInputPin*> topPins;
	std::vector<OBSBlueprintOutputPin*> bottomPins;


	for(OBSBlueprintInputPin* pin : node->getInputPins()) {
		if(GUIPin::IsVerticalPin(pin->getPinType())) {
			topPins.push_back(pin);
		}
		else {
			QGraphicsObject* guiPin = new GUIPin(pin, this);
			guiPin->setPos(0, GUI_NODE_PINS_MARGIN + leftPinPlaced * GUI_PIN_SIZE * 2);
			++leftPinPlaced;
		}
	}
	for(OBSBlueprintOutputPin* pin : node->getOutputPins()) {
		if(GUIPin::IsVerticalPin(pin->getPinType())) {
			bottomPins.push_back(pin);
		}
		else {
			QGraphicsObject* guiPin = new GUIPin(pin, this);
			guiPin->setPos(GUI_PIN_SIZE + GUI_NODE_WIDTH, GUI_NODE_PINS_MARGIN + rightPinPlaced * GUI_PIN_SIZE * 2);
			++rightPinPlaced;
		}
	}


	if(topPins.size() > 0) {
		int topPinWidth = GUI_PIN_SIZE;
		if(topPins.size() > 1) topPinWidth = ((2 * static_cast<int>(topPins.size())) - 1) * GUI_PIN_SIZE;

		int beginTopPx = ((GUI_NODE_WIDTH + 2*GUI_PIN_SIZE)/2) - (topPinWidth/2);
		for(int i=0; i < topPins.size(); i++) {
			QGraphicsObject* guiPin = new GUIPin(topPins[i], this);
			guiPin->setPos(beginTopPx + i * GUI_PIN_SIZE * 2, 0);
		}
	}

	if(bottomPins.size() > 0) {
		int bottomPinWidth = GUI_PIN_SIZE;
		if(bottomPins.size() > 1) bottomPinWidth = ((2 * static_cast<int>(bottomPins.size())) - 1) * GUI_PIN_SIZE;

		int beginBottomPx = ((GUI_NODE_WIDTH + 2*GUI_PIN_SIZE)/2) - (bottomPinWidth/2);
		for(int i=0; i < bottomPins.size(); i++) {
			QGraphicsObject* guiPin = new GUIPin(bottomPins[i], this);
			guiPin->setPos(beginBottomPx + i * GUI_PIN_SIZE * 2, GUI_PIN_SIZE + GUI_NODE_HEIGHT);
		}
	}


}

QRectF GUINode::boundingRect() const
{
	return {0, 0, GUI_NODE_WIDTH + 2*GUI_PIN_SIZE, GUI_NODE_HEIGHT + 2*GUI_PIN_SIZE};
}

void GUINode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	painter->setBrush(QBrush(Qt::lightGray, Qt::DiagCrossPattern));
	painter->drawRect(QRect(GUI_PIN_SIZE, GUI_PIN_SIZE, GUI_NODE_WIDTH, GUI_NODE_HEIGHT));
	painter->fillRect(QRect(textRect), Qt::darkGray);

	QFont font;
	font.setPixelSize(40);
	font.setBold(true);
	painter->setFont(font);
	QTextOption textOption(Qt::AlignHCenter | Qt::AlignVCenter);
	painter->drawText(textRect, QString::fromStdString(node->getDisplayName()), textOption);

}

OBSBlueprintNode * GUINode::getBlueprintNode() const
{
	return node;
}

void GUINode::GUIOnly_addConnector(GUIConnector *connector)
{
	attachedConnectors.push_back(connector);
}

void GUINode::GUIOnly_removeConnector(GUIConnector *connector)
{
	attachedConnectors.removeOne(connector);
}

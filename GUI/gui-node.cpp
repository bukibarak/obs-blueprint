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
			GUIPin* guiPin = new GUIPin(pin, this);
			guiPin->setPos(0, GUI_NODE_PINS_MARGIN + leftPinPlaced * GUI_PIN_SIZE * 2);
			nodePins.push_back(guiPin);
			leftPins.push_back(guiPin);
			++leftPinPlaced;
		}
	}
	for(OBSBlueprintOutputPin* pin : node->getOutputPins()) {
		if(GUIPin::IsVerticalPin(pin->getPinType())) {
			bottomPins.push_back(pin);
		}
		else {
			GUIPin* guiPin = new GUIPin(pin, this);
			guiPin->setPos(GUI_PIN_SIZE + GUI_NODE_WIDTH, GUI_NODE_PINS_MARGIN + rightPinPlaced * GUI_PIN_SIZE * 2);
			nodePins.push_back(guiPin);
			rightPins.push_back(guiPin);
			++rightPinPlaced;
		}
	}


	if(topPins.size() > 0) {
		int topPinWidth = GUI_PIN_SIZE;
		if(topPins.size() > 1) topPinWidth = ((2 * static_cast<int>(topPins.size())) - 1) * GUI_PIN_SIZE;

		int beginTopPx = ((GUI_NODE_WIDTH + 2*GUI_PIN_SIZE)/2) - (topPinWidth/2);
		for(int i=0; i < topPins.size(); i++) {
			GUIPin* guiPin = new GUIPin(topPins[i], this);
			guiPin->setPos(beginTopPx + i * GUI_PIN_SIZE * 2, 0);
			nodePins.push_back(guiPin);
		}
	}

	if(bottomPins.size() > 0) {
		int bottomPinWidth = GUI_PIN_SIZE;
		if(bottomPins.size() > 1) bottomPinWidth = ((2 * static_cast<int>(bottomPins.size())) - 1) * GUI_PIN_SIZE;

		int beginBottomPx = ((GUI_NODE_WIDTH + 2*GUI_PIN_SIZE)/2) - (bottomPinWidth/2);
		for(int i=0; i < bottomPins.size(); i++) {
			GUIPin* guiPin = new GUIPin(bottomPins[i], this);
			guiPin->setPos(beginBottomPx + i * GUI_PIN_SIZE * 2, GUI_PIN_SIZE + GUI_NODE_HEIGHT);
			nodePins.push_back(guiPin);
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

	painter->setBrush(Qt::white);

	QFont font;
	font.setPixelSize(40);
	font.setBold(true);
	painter->setFont(font);
	QTextOption textOption(Qt::AlignHCenter | Qt::AlignVCenter);
	painter->drawText(textRect, node->getDisplayName(), textOption);

	QFont font2;
	font2.setPixelSize(25);
	QFontMetricsF fm(font2);
	painter->setFont(font2);

	for(GUIPin* pin : leftPins) {
		const char* pinText = pin->getBlueprintPin()->getDisplayName();
		QSizeF textSize = fm.size(Qt::TextSingleLine, pinText) + QSizeF(30, 10);
		QPointF textTopLeft = pin->pos();
		QRectF pinBounds = pin->boundingRect();
		textTopLeft += QPointF(pinBounds.width() + 10, pinBounds.height() / 2);
		textTopLeft -= QPointF(0, textSize.height() / 2);
		QRectF textBounds = QRectF(textTopLeft, textSize);
		painter->fillRect(textBounds, Qt::black);
		painter->drawText(textBounds, pinText, textOption);
	}


	// painter->setFont(font2);
	// painter->setPen(Qt::black);
	// painter->setBrush(Qt::black);
	// for(GUIPin* pin : leftPins) {
	// 	QSizeF pinSize = pin->boundingRect().size();
	// 	QPointF p(pinSize.width() + 100, pinSize.height() / 2);
	// 	painter->drawText(pin->pos() + p, pin->getBlueprintPin()->getDisplayName());
	// }

}

OBSBlueprintNode * GUINode::getBlueprintNode() const
{
	return node;
}

QList<GUIConnector *> GUINode::GUIOnly_getConnectors() const
{
	QList<GUIConnector*> list;
	for(GUIPin* pin: nodePins) {
		if(pin->isConnected()) {
			list.push_back(pin->getConnector());
		}
	}
	return list;
}

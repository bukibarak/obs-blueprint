#include "obs-graphics-node.h"

#include <QPainter>

#include "obs-graphics-pin-input-field.h"
#include "Core/obs-blueprint-node.h"

OBSGraphicsNode::OBSGraphicsNode(OBSBlueprintNode *node, QGraphicsItem *parent) : QGraphicsObject(parent), node(node)
{
	nodeNameFont.setPixelSize(40);
	nodeNameFont.setBold(true);
	pinNameFont.setPixelSize(25);

	int leftPinPlaced = 0;
	int rightPinPlaced = 0;
	std::vector<OBSBlueprintInputPin*> topBlueprintPins;
	std::vector<OBSBlueprintOutputPin*> bottomBlueprintPins;


	for(OBSBlueprintInputPin* pin : node->getInputPins()) {
		if(OBSGraphicsPin::IsVerticalPin(pin->getPinType())) {
			topBlueprintPins.push_back(pin);
		}
		else {
			OBSGraphicsPin* guiPin = new OBSGraphicsPin(pin, this);
			guiPin->setPos(0, leftPinPlaced * GUI_PIN_SIZE * 2);
			nodePins.push_back(guiPin);
			OBSGraphicsPinInputField* widget = node->getGraphicsOptions().isCompactNode ? nullptr : new OBSGraphicsPinInputField(guiPin, this);
			leftMap[guiPin] = widget;
			++leftPinPlaced;
		}
	}
	for(OBSBlueprintOutputPin* pin : node->getOutputPins()) {
		if(OBSGraphicsPin::IsVerticalPin(pin->getPinType())) {
			bottomBlueprintPins.push_back(pin);
		}
		else {
			OBSGraphicsPin* guiPin = new OBSGraphicsPin(pin, this);
			guiPin->setPos(0, rightPinPlaced * GUI_PIN_SIZE * 2);
			nodePins.push_back(guiPin);
			rightPins.push_back(guiPin);
			++rightPinPlaced;
		}
	}


	for(int i=0; i < topBlueprintPins.size(); i++) {
		OBSGraphicsPin* guiPin = new OBSGraphicsPin(topBlueprintPins[i], this);
		guiPin->setPos(i * GUI_PIN_SIZE * 2, 0);
		nodePins.push_back(guiPin);
		topPins.push_back(guiPin);
	}

	for(int i=0; i < bottomBlueprintPins.size(); i++) {
		OBSGraphicsPin* guiPin = new OBSGraphicsPin(bottomBlueprintPins[i], this);
		guiPin->setPos(i * GUI_PIN_SIZE * 2, 0);
		nodePins.push_back(guiPin);
		bottomPins.push_back(guiPin);
	}


	calculateContentRect();


	bool compactNode = node->getGraphicsOptions().isCompactNode;
	bool showName = node->getGraphicsOptions().showName;
	QPointF moveLeft{};
	QPointF moveRight{};
	if(!compactNode) {
		moveLeft = {0, showName ? GUI_NODE_PINS_MARGIN : topPins.empty() ? pinsMargin.height() / 2 : 120};
		moveRight = {nodeContentRect.x() + nodeContentRect.width(), showName ? GUI_NODE_PINS_MARGIN : topPins.empty() ? pinsMargin.height() / 2 : 120};
	}
	else if (showName) {
		qreal leftPart = nodeContentRect.height() / 2;
		qreal leftPinsHeight = calculatePinsLength(leftMap.size());
		qreal beginTopPy = leftPart - (leftPinsHeight / 2);
		moveLeft = {0, beginTopPy};

		qreal rightPinsHeight = calculatePinsLength(rightPins.size());
		qreal beginBottomPy = leftPart - (rightPinsHeight / 2);
		moveRight = {nodeContentRect.x() + nodeContentRect.width(), beginBottomPy};
	}
	else {
		if(topPins.empty()) {
			moveRight = {nodeContentRect.x() + nodeContentRect.width(), 0};
		}
		else {
			moveLeft = {0,120};
			moveRight = {nodeContentRect.x() + nodeContentRect.width(), 120};
		}
	}

	for(OBSGraphicsPin* pin: leftMap.keys()) {
		pin->moveBy(moveLeft.x(), moveLeft.y());
	}
	for(OBSGraphicsPin* pin: rightPins) {
		pin->moveBy(moveRight.x(), moveRight.y());
	}



	qreal leftPart =  nodeContentRect.width()/ 2;
	qreal topPinsWidth = calculatePinsLength(topPins.size());
	qreal beginTopPx =  leftPart - (topPinsWidth / 2);
	for(OBSGraphicsPin* pin: topPins) {
		pin->moveBy(beginTopPx + nodeContentRect.x(), 0);
	}
	qreal bottomPinsWidth = calculatePinsLength(bottomPins.size());
	qreal beginBottomPx = leftPart - (bottomPinsWidth / 2);
	for(OBSGraphicsPin* pin: bottomPins) {
		pin->moveBy(beginBottomPx + nodeContentRect.x(), nodeContentRect.y() + nodeContentRect.height());
	}

}

QRectF OBSGraphicsNode::boundingRect() const
{
	qreal width = nodeContentRect.width();
	if(!leftMap.empty()) width += GUI_PIN_SIZE;
	if(!rightPins.empty()) width += GUI_PIN_SIZE;
	qreal height = nodeContentRect.height();
	if(!topPins.empty()) height += GUI_PIN_SIZE;
	if(!bottomPins.empty()) height += GUI_PIN_SIZE;
	return {0, 0, width, height};
}

void OBSGraphicsNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	painter->setBrush(QBrush(Qt::lightGray, Qt::DiagCrossPattern));
	painter->drawRect(nodeContentRect);

	if(node->getGraphicsOptions().showName) {
		QRectF nodeNameTextRect;
		nodeNameTextRect.setX(nodeContentRect.x() + (nodeNameMargin.width() / 2));
		nodeNameTextRect.setY(nodeContentRect.y() + (nodeNameMargin.height() / 2));
		nodeNameTextRect.setWidth(nodeContentRect.width() - nodeNameMargin.width());
		nodeNameTextRect.setHeight(qMin(nodeContentRect.height() - nodeNameMargin.height(), 130.0));
		painter->fillRect(nodeNameTextRect, Qt::darkGray);
		painter->setBrush(Qt::white);
		painter->setFont(nodeNameFont);
		QTextOption textOption(Qt::AlignHCenter | Qt::AlignVCenter);
		painter->drawText(nodeNameTextRect, node->getDisplayName(), textOption);
	}


	if(!node->getGraphicsOptions().isCompactNode) {
		QFontMetricsF fm(pinNameFont);
		painter->setFont(pinNameFont);
		QTextOption textOption(Qt::AlignHCenter | Qt::AlignVCenter);
		for(const auto [pin, proxy]: leftMap.asKeyValueRange()) {
			const char* pinText = pin->getBlueprintPin()->getDisplayName();
			QSizeF textSize = fm.size(Qt::TextSingleLine, pinText) + pinNamePadding;
			QPointF textTopLeft = pin->pos();
			QRectF pinBounds = pin->boundingRect();
			textTopLeft += QPointF(pinBounds.width() + (pinsMargin.width()/2), pinBounds.height() / 2);
			textTopLeft -= QPointF(0, textSize.height() / 2);
			QRectF textBounds = QRectF(textTopLeft, textSize);
			painter->fillRect(textBounds, Qt::black);
			painter->drawText(textBounds, pinText, textOption);

			proxy->setPos(textTopLeft.x() + textSize.width() + (pinsMargin.width()/2), textTopLeft.y());
		}

		for(auto pin: rightPins) {
			const char* pinText = pin->getBlueprintPin()->getDisplayName();
			QSizeF textSize = fm.size(Qt::TextSingleLine, pinText) + pinNamePadding;
			QPointF textTopLeft = pin->pos();
			QRectF pinBounds = pin->boundingRect();
			textTopLeft += QPointF(0, pinBounds.height() / 2);
			textTopLeft -= QPointF(textSize.width() + 10, textSize.height() / 2);
			QRectF textBounds = QRectF(textTopLeft, textSize);
			painter->fillRect(textBounds, Qt::black);
			painter->drawText(textBounds, pinText, textOption);
		}

		for(auto pin: topPins) {
			const char* pinText = pin->getBlueprintPin()->getDisplayName();
			QSizeF textSize = fm.size(Qt::TextSingleLine, pinText) + pinNamePadding;
			QPointF textTopLeft = pin->pos();
			QRectF pinBounds = pin->boundingRect();
			textTopLeft += QPointF(pinBounds.width() / 2, pinBounds.height() + 10);
			textTopLeft -= QPointF(textSize.width() / 2, 0);
			QRectF textBounds = QRectF(textTopLeft, textSize);
			painter->fillRect(textBounds, Qt::black);
			painter->drawText(textBounds, pinText, textOption);
		}

		for(auto pin: bottomPins) {
			const char* pinText = pin->getBlueprintPin()->getDisplayName();
			QSizeF textSize = fm.size(Qt::TextSingleLine, pinText) + pinNamePadding;
			QPointF textTopLeft = pin->pos();
			QRectF pinBounds = pin->boundingRect();
			textTopLeft += QPointF(pinBounds.width() / 2, 0);
			textTopLeft -= QPointF(textSize.width() / 2, textSize.height() + 10);
			QRectF textBounds = QRectF(textTopLeft, textSize);
			painter->fillRect(textBounds, Qt::black);
			painter->drawText(textBounds, pinText, textOption);
		}
	}
}

OBSBlueprintNode * OBSGraphicsNode::getBlueprintNode() const
{
	return node;
}

QList<OBSGraphicsConnector *> OBSGraphicsNode::GUIOnly_getConnectors() const
{
	QList<OBSGraphicsConnector*> list;
	for(OBSGraphicsPin* pin: nodePins) {
		if(pin->isConnected()) {
			list.append(pin->getConnectors());
		}
	}
	return list;
}

void OBSGraphicsNode::calculateContentRect()
{
	QPointF topLeft{leftMap.empty() ? 0.0 : GUI_PIN_SIZE, topPins.empty() ? 0.0 : GUI_PIN_SIZE};

	// Get size according to pins
	QSizeF size{
			qMax(calculatePinsLength(topPins.size()), calculatePinsLength(bottomPins.size())),
			qMax(calculatePinsLength(leftMap.size()), calculatePinsLength(rightPins.size()))
	};
	size.setWidth(qMax(size.width(), 10.0));
	size.setHeight(qMax(size.height(), 10.0));

	if(!topPins.empty()) size += QSizeF(0, 60); // Top pins text
	if(!bottomPins.empty()) size += QSizeF(0, 60); // Bottom pins text


	if(!node->getGraphicsOptions().isCompactNode) {
		size += pinsMargin;

		if(node->getGraphicsOptions().showName) {
			size += QSizeF(0, GUI_NODE_PINS_MARGIN);
		}
	}

	// Get size according to node text
	if(node->getGraphicsOptions().showName) {
		QFontMetrics fm(nodeNameFont);
		QSizeF nodeTextSize = fm.size(Qt::TextSingleLine, node->getDisplayName()) + nodeNamePadding + nodeNameMargin;
		size.setWidth(qMax(size.width(), nodeTextSize.width()));
		size.setHeight(qMax(size.height(), nodeTextSize.height()));
	}

	// Get size according to pin names and input
	if(!node->getGraphicsOptions().isCompactNode) {
		int rowsPerformed = 0;
		QFontMetrics fm(pinNameFont);
		for(const auto [pin, proxy]: leftMap.asKeyValueRange()) {
			QSizeF leftPinTextSize = fm.size(Qt::TextSingleLine, pin->getBlueprintPin()->getDisplayName()) + pinNamePadding + pinNameMargin;
			QSizeF leftPinInputSize = proxy->size() * proxy->scale() + (2*pinNamePadding);

			qreal leftWidth = leftPinTextSize.width() + leftPinInputSize.width();

			qreal rightWidth = 0;
			if(rowsPerformed < rightPins.size()) {
				OBSGraphicsPin* pin = rightPins[rowsPerformed];
				QSizeF rightPinTextSize = fm.size(Qt::TextSingleLine, pin->getBlueprintPin()->getDisplayName()) + pinNamePadding + pinNameMargin;
				rightWidth = rightPinTextSize.width();
			}
			size.setWidth(qMax(size.width(), leftWidth + rightWidth));
			++rowsPerformed;
		}
		if(rowsPerformed < rightPins.size()) {
			do {
				OBSGraphicsPin* pin = rightPins[rowsPerformed];
				QSizeF rightPinTextSize = fm.size(Qt::TextSingleLine, pin->getBlueprintPin()->getDisplayName()) + pinNamePadding + pinNameMargin;
				size.setWidth(qMax(size.width(), rightPinTextSize.width()));
			} while (++rowsPerformed < rightPins.size());
		}
	}

	// Set node content size
	nodeContentRect = QRectF(topLeft, size);
}

qreal OBSGraphicsNode::calculatePinsLength(size_t size)
{
	return size < 2 ? size*GUI_PIN_SIZE : GUI_PIN_SIZE + 2 * (size - 1) * GUI_PIN_SIZE;
}

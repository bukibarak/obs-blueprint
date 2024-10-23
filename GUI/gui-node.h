#pragma once
#include <QGraphicsObject>

#include "gui-pin.h"
#include "Core/obs-blueprint-node.h"

#define GUI_NODE_WIDTH 500
#define GUI_NODE_HEIGHT 700

class GUINode : public QGraphicsObject {
public:
	GUINode(OBSBlueprintNode* node, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
	OBSBlueprintNode* node;

	QRect textRect{GUI_PIN_SIZE + 10, GUI_PIN_SIZE + 10, GUI_NODE_WIDTH - 20, 130};
	// QList<QLine> cornerLines{
	// 	{
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE + GUI_NODE_HEIGHT
	// 	}, // left line
	// 	{
	// 		GUI_PIN_SIZE + GUI_NODE_WIDTH,
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE + GUI_NODE_WIDTH,
	// 		GUI_PIN_SIZE + GUI_NODE_HEIGHT
	// 	}, // right line
	// 	{
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE + GUI_NODE_WIDTH,
	// 		GUI_PIN_SIZE,
	// 	}, // top line
	// 	{
	// 		GUI_PIN_SIZE,
	// 		GUI_PIN_SIZE + GUI_NODE_HEIGHT,
	// 		GUI_PIN_SIZE + GUI_NODE_WIDTH,
	// 		GUI_PIN_SIZE + GUI_NODE_HEIGHT
	// 	} // bottom line
	// };
};

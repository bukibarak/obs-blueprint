#pragma once
#include <QGraphicsObject>

#include "gui-pin.h"
#include "Core/obs-blueprint-node.h"

class GUIConnector;

#define GUI_NODE_WIDTH 500
#define GUI_NODE_HEIGHT 700
#define GUI_NODE_PINS_MARGIN 250

class GUINode : public QGraphicsObject {
public:
	enum { Type = UserType + 2 };
	GUINode(OBSBlueprintNode* node, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

	OBSBlueprintNode* getBlueprintNode() const;
	const std::string& getDisplayName() const;

	void GUIOnly_addConnector(GUIConnector* connector);
	QList<GUIConnector*> GUIOnly_getConnectors() const {return attachedConnectors; }

private:
	QList<GUIConnector*> attachedConnectors;
	OBSBlueprintNode* node;

	QRect textRect{GUI_PIN_SIZE + 10, GUI_PIN_SIZE + 10, GUI_NODE_WIDTH - 20, 130};
};

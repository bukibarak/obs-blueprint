#pragma once
#include <Qpen>
#include <QGraphicsObject>
#include "Core/obs-blueprint-connector.h"

class GUINode;
class GUIPin;

class GUIConnector : public QGraphicsObject {
public:
	enum { Type = UserType + 3 };
	GUIConnector(OBSBlueprintConnector* connector, GUIPin* fromPin, GUIPin* toPin, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override {return bounds; }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

	OBSBlueprintConnector* getBlueprintConnector() const {return connector;}
	GUIPin* getFromPin() const {return from;}
	GUIPin* getToPin() const {return to;}
	GUIPin* getOther(GUIPin* self) const {return self == from ? to : self == to ? from : nullptr;}
	GUINode* getOtherNode(GUINode* self) const;
	void redrawConnector();

private:
	OBSBlueprintConnector* connector;
	GUIPin* from;
	GUIPin* to;
	GUIPin* left;
	GUIPin* right;
	GUIPin* top;
	GUIPin* bottom;
	QPen pen;
	QRectF bounds{};
};

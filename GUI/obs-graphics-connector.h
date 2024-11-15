#pragma once
#include <Qpen>
#include <QGraphicsObject>
#include "Core/obs-blueprint-connector.h"

class OBSGraphicsNode;
class OBSGraphicsPin;

class OBSGraphicsConnector : public QGraphicsObject {
public:
	enum { Type = UserType + 3 };
	OBSGraphicsConnector(OBSBlueprintConnector* connector, OBSGraphicsPin* fromPin, OBSGraphicsPin* toPin, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override {return bounds; }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

	OBSBlueprintConnector* getBlueprintConnector() const {return connector;}
	OBSGraphicsPin* getFromPin() const {return from;}
	OBSGraphicsPin* getToPin() const {return to;}
	OBSGraphicsPin* getOther(OBSGraphicsPin* self) const {return self == from ? to : self == to ? from : nullptr;}
	OBSGraphicsNode* getOtherNode(OBSGraphicsNode* self) const;
	void redrawConnector();

private:
	OBSBlueprintConnector* connector;
	OBSGraphicsPin* from;
	OBSGraphicsPin* to;
	OBSGraphicsPin* left;
	OBSGraphicsPin* right;
	OBSGraphicsPin* top;
	OBSGraphicsPin* bottom;
	QPen pen;
	QRectF bounds{};
};

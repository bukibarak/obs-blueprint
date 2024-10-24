#pragma once
#include <Qpen>
#include <QGraphicsObject>
#include "Core/obs-blueprint-connector.h"

class GUIPin;

class GUIConnector : public QGraphicsObject {
public:
	enum { Type = UserType + 3 };
	GUIConnector(OBSBlueprintConnector* connector, GUIPin* fromPin, GUIPin* toPin, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override {return bounds; }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

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

#pragma once
#include <QFont>
#include <QGraphicsObject>

#include "obs-graphics-pin.h"

class OBSGraphicsPinInputField;
class OBSGraphicsConnector;

// #define GUI_NODE_WIDTH 500
// #define GUI_NODE_HEIGHT 700
#define GUI_NODE_PINS_MARGIN 170

class OBSGraphicsNode : public QGraphicsObject {
public:
	enum { Type = UserType + 2 };
	OBSGraphicsNode(OBSBlueprintNode* node, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

	OBSBlueprintNode* getBlueprintNode() const;

	const QList<OBSGraphicsPin*>& getGUIPins() const {return nodePins;}
	QSizeF getSize() const { return nodeContentRect.size(); }

	QList<OBSGraphicsConnector*> GUIOnly_getConnectors() const;

	void setSelectionState(bool state);

private:

	bool selected = false;
	const qreal selectPenWidth = 10;

	void calculateContentRect();
	QSizeF calculateBoundingRect() const;
	void alignPins();
	static qreal calculatePinsLength(size_t size);

	OBSBlueprintNode* node;

	QList<OBSGraphicsPin*> nodePins;
	QMap<OBSGraphicsPin*, OBSGraphicsPinInputField*> leftMap;
	QList<OBSGraphicsPin*> rightPins;
	QList<OBSGraphicsPin*> topPins;
	QList<OBSGraphicsPin*> bottomPins;

	QFont nodeNameFont;
	QFont pinNameFont;
	QString name;
	QPointF alignLeft{};
	QPointF alignRight{};
	QPointF alignTop{};
	QPointF alignBottom{};

	//QRect nodeNameTextRect{GUI_PIN_SIZE + 10, GUI_PIN_SIZE + 10, GUI_NODE_WIDTH - 20, 130};
	QRectF nodeContentRect{0,0,0,0};

	const QSizeF pinsMargin{20, 20};
	const QSizeF nodeNameMargin{40, 40};
	const QSizeF nodeNamePadding{50, 20};
	const QSizeF pinNamePadding{30, 10};
	const QSizeF pinNameMargin{10, 0};
};

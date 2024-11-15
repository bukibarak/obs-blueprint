#pragma once
#include <QGraphicsObject>
#include "Core/obs-blueprint-pin.h"

class OBSGraphicsConnector;
class OBSGraphicsNode;

#define GUI_PIN_SIZE 50
#define GUI_PIN_CORNER_SIZE 7

class OBSGraphicsPin : public QGraphicsObject {
public:
	enum { Type = UserType + 1 };
	OBSGraphicsPin(OBSBlueprintInputPin *pin, QGraphicsItem* parent = nullptr);
	OBSGraphicsPin(OBSBlueprintOutputPin *pin, QGraphicsItem* parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	int type() const override {return Type;}

	OBSBlueprintPin* getBlueprintPin() const;
	OBSBlueprintInputPin* getInputPin() const {return inputPin;}
	OBSBlueprintOutputPin* getOutputPin() const {return outputPin;}
	bool isInputPin() const {return inputPin != nullptr;}

	static bool IsVerticalPin(PinType type) {return type == AUDIO_PIN || type == VIDEO_PIN || type == AUDIOVIDEO_PIN;}

	OBSGraphicsNode* getParentNode() const {return parentNode;}
	const QList<OBSGraphicsConnector*>& getConnectors() const {return connectors;}
	OBSGraphicsConnector* getFirstConnector() const { return connectors.isEmpty() ? nullptr : connectors.first(); }

	void connect(OBSGraphicsConnector* connector);
	void disconnect(OBSGraphicsConnector* connector);
	bool isConnected() const {return !connectors.isEmpty(); }

	std::function<void(bool)>* onConnectionStateChanged = nullptr;

private:

	OBSGraphicsNode* parentNode = nullptr;
	QList<OBSGraphicsConnector*> connectors;

	void drawPinContent(QPainter *painter, PinType pinType);
	void drawTopPinCorners(QPainter *painter);
	void drawBottomPinCorners(QPainter *painter);
	void drawLeftPinCorners(QPainter *painter);
	void drawRightPinCorners(QPainter *painter);

	const QRect leftCorner{0, 0, GUI_PIN_CORNER_SIZE, GUI_PIN_SIZE};
	const QRect rightCorner{GUI_PIN_SIZE - GUI_PIN_CORNER_SIZE, 0, GUI_PIN_CORNER_SIZE, GUI_PIN_SIZE};
	const QRect topCorner{0, 0, GUI_PIN_SIZE, GUI_PIN_CORNER_SIZE};
	const QRect bottomCorner{0, GUI_PIN_SIZE - GUI_PIN_CORNER_SIZE, GUI_PIN_SIZE, GUI_PIN_CORNER_SIZE};
	OBSBlueprintInputPin *inputPin = nullptr;
	OBSBlueprintOutputPin *outputPin = nullptr;

};

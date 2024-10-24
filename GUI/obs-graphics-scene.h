#pragma once
#include <QGraphicsScene>

#include "Core/obs-blueprint-node.h"

class GUIPin;
class GUINode;
class GUIConnector;

class OBSGraphicsScene : public QGraphicsScene {
public:
	OBSGraphicsScene(OBSBlueprintGraph* linkedGraph, QObject* parent = nullptr);

	void setView(QGraphicsView* mainView) { view = mainView;}
	void addGUINode(OBSBlueprintNode* node, qreal px, qreal py);
	void addGUIConnector(OBSBlueprintConnector* connector, GUIPin* from, GUIPin* to);
	GUINode* getGUINodeAt(const QPointF& scenePos) const;
	GUIConnector* getGUIConnectorAt(const QPointF& scenePos) const;
	void resetZoomLevel() {	zoomLevel = 0; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseClickEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent(QGraphicsSceneWheelEvent *event) override;
	GUIPin* getGUIPinAt(const QPointF& scenePos) const;

private:

	OBSBlueprintGraph* graph = nullptr;
	GUIPin* graphVideoInputPin = nullptr;

	int32_t zoomLevel = 0;
	QGraphicsView* view = nullptr;
	Qt::MouseButton pressedButton;
	GUIPin* pressedPin = nullptr;
	GUINode* pressedNode = nullptr;
	QPointF pressedPosRelative;
	bool mouseClicked = false;
	bool mouseMoved = false;
	QGraphicsLineItem* dragConnector = nullptr;


};

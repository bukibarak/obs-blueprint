#pragma once
#include <QGraphicsScene>
#include <QResizeEvent>

#include "Core/obs-blueprint-node.h"

class OBSGraphicsView;
class GUIPin;
class GUINode;
class GUIConnector;

class OBSGraphicsScene : public QGraphicsScene {
public:
	OBSGraphicsScene(OBSBlueprintGraph* linkedGraph, QObject* parent = nullptr);

	void initializeFromBlueprintGraph();
	void setView(OBSGraphicsView* mainView);
	void addGUINode(OBSBlueprintNode* node, qreal px, qreal py);
	void removeGUINode(GUINode* node);
	void addGUIConnector(OBSBlueprintConnector* connector, GUIPin* from, GUIPin* to);
	void removeGUIConnector(GUIConnector* connector);
	GUINode* getGUINodeAt(const QPointF& scenePos) const;
	GUIConnector* getGUIConnectorAt(const QPointF& scenePos) const;
	void resetZoomLevel() { zoomLevel = 0; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseClickEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent(QGraphicsSceneWheelEvent *event) override;

	GUIPin* getGUIPinAt(const QPointF& scenePos) const;

private:

	void forceGraphGUIOnBottom();

	OBSGraphicsView* view = nullptr;

	OBSBlueprintGraph* graph = nullptr;
	GUIPin* graphVideoInputPin = nullptr;

	int32_t zoomLevel = 0;

	Qt::MouseButton pressedButton;
	GUIPin* pressedPin = nullptr;
	GUINode* pressedNode = nullptr;
	QPointF pressedPosRelative;
	bool mouseClicked = false;
	bool mouseMoved = false;
	QGraphicsLineItem* dragConnector = nullptr;

	std::map<OBSBlueprintNode*, GUINode*> nodeMap;
	std::map<OBSBlueprintPin*, GUIPin*> pinMap;
	std::map<OBSBlueprintConnector*, GUIConnector*> connectorMap;

	std::function<void(QResizeEvent*)> resizeFunc = [this] (QResizeEvent* event) {forceGraphGUIOnBottom();};
};

#pragma once
#include <QGraphicsScene>
#include <QResizeEvent>

#include "Core/obs-blueprint-node.h"

class OBSGraphicsPinInputField;
class OBSGraphicsView;
class OBSGraphicsPin;
class OBSGraphicsNode;
class OBSGraphicsConnector;

class OBSGraphicsScene : public QGraphicsScene {
public:
	OBSGraphicsScene(OBSBlueprintGraph* linkedGraph, QObject* parent = nullptr);

	void initializeFromBlueprintGraph();
	void setView(OBSGraphicsView* mainView);
	OBSGraphicsNode* addGUINode(OBSBlueprintNode* node, qreal px, qreal py);
	void removeGUINode(OBSGraphicsNode* node);
	OBSGraphicsConnector* addGUIConnector(OBSBlueprintConnector* connector, OBSGraphicsPin* from, OBSGraphicsPin* to);
	void removeGUIConnector(OBSGraphicsConnector* connector);

	void resetZoomLevel() { zoomLevel = 0; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseClickEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void wheelEvent(QGraphicsSceneWheelEvent *event) override;

	OBSGraphicsPinInputField* getGraphicsInputFieldAt(const QPointF& scenePos) const;
	OBSGraphicsConnector* getGraphicsConnectorAt(const QPointF& scenePos) const;
	OBSGraphicsNode* getGraphicsNodeAt(const QPointF& scenePos) const;
	OBSGraphicsPin* getGraphicsPinAt(const QPointF& scenePos) const;

private:

	bool tryConnectPins(OBSGraphicsPin* A, OBSGraphicsPin* B);
	OBSGraphicsNode *showContextMenu(const QPointF &scenePos);
	void forceGraphGUIOnBottom();

	OBSGraphicsView* view = nullptr;

	OBSBlueprintGraph* graph = nullptr;
	OBSGraphicsPin* graphVideoInputPin = nullptr;

	int32_t zoomLevel = 0;

	Qt::MouseButton pressedButton;
	OBSGraphicsPin* pressedPin = nullptr;
	OBSGraphicsNode* pressedNode = nullptr;
	OBSGraphicsPinInputField* pressedInputField = nullptr;
	QPointF pressedPosRelative;
	bool mouseClicked = false;
	bool mouseMoved = false;
	QGraphicsLineItem* dragConnector = nullptr;

	QHash<OBSBlueprintNode*, OBSGraphicsNode*> nodeMap;
	QHash<OBSBlueprintPin*, OBSGraphicsPin*> pinMap;
	QHash<OBSBlueprintConnector*, OBSGraphicsConnector*> connectorMap;

	std::function<void(QResizeEvent*)> resizeFunc = [this] (QResizeEvent*) {forceGraphGUIOnBottom();};
};

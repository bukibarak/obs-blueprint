#pragma once
#include <QGraphicsScene>
#include <QResizeEvent>

#include "Core/obs-blueprint-node.h"

struct GUIContext;
class OBSGraphicsPinInputField;
class OBSGraphicsPin;
class OBSGraphicsNode;
class OBSGraphicsConnector;

class OBSGraphicsScene : public QGraphicsScene {
public:
	OBSGraphicsScene(GUIContext& context, QObject* parent = nullptr);

	void initializeFromBlueprintGraph();
	OBSGraphicsNode* addGUINode(OBSBlueprintNode* node, qreal px, qreal py, bool addToBlueprintGraph = true);
	void removeGUINode(OBSGraphicsNode* node, bool removeFromBlueprintGraph = true);
	OBSGraphicsConnector* addGUIConnector(OBSGraphicsPin* from, OBSGraphicsPin* to, OBSBlueprintConnector* existing = nullptr);
	void removeGUIConnector(OBSGraphicsConnector* connector, bool removeFromBlueprintGraph = true);

	void resetZoomLevel() { zoomLevel = 0; }
	void resetSelected();

	bool isDropAccepted(const QPointF& scenePos) const;

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
	void forceGraphGUIOnBottom() const;

	GUIContext& ctx;


	OBSGraphicsPin* graphVideoInputPin = nullptr;

	int32_t zoomLevel = 0;

	Qt::MouseButton pressedButton;
	OBSGraphicsPin* pressedPin = nullptr;
	OBSGraphicsNode* pressedNode = nullptr;
	OBSGraphicsNode* selectedNode = nullptr;
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

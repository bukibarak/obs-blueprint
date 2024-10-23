#pragma once
#include <QGraphicsScene>

#include "Core/obs-blueprint-node.h"

class OBSGraphicsScene : public QGraphicsScene {
public:
	OBSGraphicsScene();

	void setView(QGraphicsView* mainView) { view = mainView;}
	void addGUINode(OBSBlueprintNode* node, qreal px, qreal py);
	void resetZoomLevel() {	zoomLevel = 0; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
	int32_t zoomLevel = 0;
	QGraphicsView* view = nullptr;
	Qt::MouseButton pressedButton;
	bool mouseClicked = false;
	bool mouseMoved = false;


};

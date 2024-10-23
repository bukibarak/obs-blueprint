#include "obs-graphics-scene.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QScrollBar>
#include <QGraphicsRectItem>

#include "gui-node.h"


OBSGraphicsScene::OBSGraphicsScene()
{
	#define GRID_HALF_SIZE 50000

	// Add the vertical lines first, paint them red
	for (int x=-GRID_HALF_SIZE; x<=GRID_HALF_SIZE; x+=100) {
		QPen pen(x == 0 ? Qt::white : (x%500 == 0 ? Qt::lightGray : Qt::darkGray));
		addLine(x,-GRID_HALF_SIZE,x,GRID_HALF_SIZE,pen);
		addLine(-GRID_HALF_SIZE, x, GRID_HALF_SIZE, x, pen);

		if(x == 0) {
			addLine(x+1,-GRID_HALF_SIZE,x+1,GRID_HALF_SIZE,pen);
			addLine(x-1,-GRID_HALF_SIZE,x-1,GRID_HALF_SIZE,pen);
			addLine(-GRID_HALF_SIZE, x+1, GRID_HALF_SIZE, x+1, pen);
			addLine(-GRID_HALF_SIZE, x-1, GRID_HALF_SIZE, x-1, pen);
		}
	}

	// Now add the horizontal lines, paint them green
	// for (int y=-GRID_HALF_SIZE; y<=GRID_HALF_SIZE; y+=100) {
	// 	if(y%500 == 0) addLine(-GRID_HALF_SIZE,y,GRID_HALF_SIZE,y,QPen(Qt::lightGray));
	// 	else addLine(-GRID_HALF_SIZE,y,GRID_HALF_SIZE,y,QPen(Qt::darkGray));;
	// }
}

void OBSGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

	QGraphicsScene::mousePressEvent(event);
	if( !mouseClicked) {
		mouseClicked = true;
		mouseMoved = false;
		if(event->button() == Qt::LeftButton) {
			pressedButton = Qt::LeftButton;
			view->setDragMode(QGraphicsView::RubberBandDrag);
			QApplication::setOverrideCursor(Qt::CrossCursor);
		}
		else if(event->button() == Qt::RightButton) {
			pressedButton = Qt::RightButton;
			view->setDragMode(QGraphicsView::ScrollHandDrag);
			QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
		}
	}

	qDebug() << "Mouse button #" << event->button() << "pressed at [" << event->scenePos() << "]";
}

void OBSGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseReleaseEvent(event);

	if(event->button() == pressedButton) {
		if(pressedButton == Qt::LeftButton) {

		}
		else if(pressedButton == Qt::RightButton && !mouseMoved) {
			const QPointF& scenePos = event->scenePos();
			QList<QGraphicsItem*> items = view->items(view->mapFromScene(scenePos));
			if(items.isEmpty()) {
				// addFilledRect(scenePos);
			} else {
				bool removed = false;
				for(auto item : items) {
					if(item->type() != QGraphicsLineItem::Type) {
						removeItem(item);
						removed = true;
						break;
					}
				}
				if(!removed) {

				}
			}
		}
	}


	mouseClicked = false;
	mouseMoved = false;
	QApplication::restoreOverrideCursor();
	view->setDragMode(QGraphicsView::NoDrag);
	qDebug() << "Mouse button #" << event->button() << "released at [" << event->scenePos() << "]";
}

void OBSGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseMoveEvent(event);
	mouseMoved = true;
	if(event->buttons() == Qt::RightButton) {
		QPointF movement = event->lastScreenPos() - event->screenPos();
		view->horizontalScrollBar()->setValue(view->horizontalScrollBar()->value() + static_cast<int>(movement.x()));
		view->verticalScrollBar()->setValue(view->verticalScrollBar()->value() + static_cast<int>(movement.y()));
		qDebug() << "Mouse moved at [" << movement << "]";
	}
}

void OBSGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	if(event->delta() < 0 && zoomLevel > -4) {
		--zoomLevel;
		view->scale(1.0/1.2, 1.0/1.2);
	}
	else if(event->delta() > 0 && zoomLevel < 5) {
		++zoomLevel;
		view->scale(1.2, 1.2);
	}
	qDebug() << "Zoom level: " << zoomLevel;
}

void OBSGraphicsScene::addGUINode(OBSBlueprintNode *node, qreal px,
	qreal py)
{
	GUINode* guiNode = new GUINode(node);
	guiNode->setScale(1.5);
	guiNode->setPos(px, py);
	addItem(guiNode);
}

// void OBSGraphicsScene::addFilledRect(const QPointF &topLeft)
// {
// 	// QGraphicsRectItem* newRect = new QGraphicsRectItem(QRectF(topLeft, QSizeF(200.0, 200.0)));
// 	// newRect->setBrush(QBrush(Qt::red));
// 	// addItem(newRect);
// }

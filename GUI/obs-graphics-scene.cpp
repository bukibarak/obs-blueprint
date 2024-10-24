#include "obs-graphics-scene.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QScrollBar>
#include <QGraphicsRectItem>
#include <QMenu>
#include <QMessageBox>

#include "context-menu-helper.h"
#include "gui-connector.h"
#include "gui-node.h"
#include "Core/obs-blueprint-graph.h"

OBSGraphicsScene::OBSGraphicsScene(OBSBlueprintGraph *linkedGraph,
                                   QObject *parent) : QGraphicsScene(parent), graph(linkedGraph)
{
#define GRID_HALF_SIZE 50000

	// Add the grid lines
	for (int x=-GRID_HALF_SIZE; x<=GRID_HALF_SIZE; x+=100) {
		QPen pen(x == 0 ? Qt::white : x%500 == 0 ? Qt::lightGray : Qt::darkGray);
		addLine(x,-GRID_HALF_SIZE,x,GRID_HALF_SIZE,pen);
		addLine(-GRID_HALF_SIZE, x, GRID_HALF_SIZE, x, pen);

		if(x == 0) {
			addLine(x+1,-GRID_HALF_SIZE,x+1,GRID_HALF_SIZE,pen);
			addLine(x-1,-GRID_HALF_SIZE,x-1,GRID_HALF_SIZE,pen);
			addLine(-GRID_HALF_SIZE, x+1, GRID_HALF_SIZE, x+1, pen);
			addLine(-GRID_HALF_SIZE, x-1, GRID_HALF_SIZE, x-1, pen);
		}
	}

	graphVideoInputPin = new GUIPin(graph->getMainVideoInputPin());
	graphVideoInputPin->setScale(3);
	addItem(graphVideoInputPin);
}

void OBSGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mousePressEvent(event);

	// Prevent simultaneous mouse click events
	if(!mouseClicked) {
		mouseClicked = true;
		mouseMoved = false;
		pressedPin = getGUIPinAt(event->scenePos());
		pressedNode = getGUINodeAt(event->scenePos());
		if(event->button() == Qt::LeftButton) {
			pressedButton = Qt::LeftButton;
			if(pressedPin != nullptr) {
				// Left click begin on pin
				QPen pen(GUIPin::GetPinColor(pressedPin->getBlueprintPin()->getPinType()));
				pen.setWidth(10);
				QPointF p = pressedPin->scenePos();
				p += QPointF(GUI_PIN_SIZE/2, GUI_PIN_SIZE/2);
				dragConnector = addLine(p.x(),p.y(), event->scenePos().x(), event->scenePos().y(), pen);
			}
			else if(pressedNode != nullptr) {
				// Left-click begin on node
				QApplication::setOverrideCursor(Qt::OpenHandCursor);
				pressedPosRelative = event->scenePos() - pressedNode->pos();
			}
			else {
				// Left-click begin on the graph
				view->setDragMode(QGraphicsView::RubberBandDrag);
				QApplication::setOverrideCursor(Qt::CrossCursor);
			}
		}
		else if(event->button() == Qt::RightButton) {
			// Right-click begin on the graph
			pressedButton = Qt::RightButton;
			view->setDragMode(QGraphicsView::ScrollHandDrag);
			QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
		}

		// qDebug() << "Mouse button #" << event->button() << "pressed at [" << event->scenePos() << "]";
	}
}

void OBSGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseReleaseEvent(event);

	if(event->button() == pressedButton) {
		if(!mouseMoved) {
			mouseClickEvent(event);
		} else {
			if(pressedButton == Qt::LeftButton) {
				//TODO left moving click end --> end of rubber band (select all nodes inside)
				if(pressedPin != nullptr) {
					removeItem(dragConnector);
					delete dragConnector;
					GUIPin* destinationPin = getGUIPinAt(event->scenePos());
					if(
							destinationPin != nullptr // Pin found
							&& destinationPin != pressedPin // Cannot connect pin to itself
							&& destinationPin->getBlueprintPin()->getPinType() == pressedPin->getBlueprintPin()->getPinType() // Pins must be the same type
							&& destinationPin->isInputPin() != pressedPin->isInputPin() // 1 pin should be input and the other output
							&& !destinationPin->getBlueprintPin()->isConnected() // destination pin should not already be connected
							&& !pressedPin->getBlueprintPin()->isConnected()) { // pressed pin should not already be connected
						if(pressedPin->isInputPin()) {
							OBSBlueprintConnector* connector = graph->createConnector(destinationPin->getOutputPin(), pressedPin->getInputPin());
							addGUIConnector(connector, destinationPin, pressedPin);
						}
						else {
							OBSBlueprintConnector* connector = graph->createConnector(pressedPin->getOutputPin(), destinationPin->getInputPin());
							addGUIConnector(connector, pressedPin, destinationPin);
						}
					}
				}

			}
			else if (pressedButton == Qt::RightButton) {
				//TODO right moving click end --> do noting?
			}
		}
		// qDebug() << "Mouse button #" << event->button() << "released at [" << event->scenePos() << "]";
	}
	pressedButton = Qt::NoButton;
	pressedPin = nullptr;
	pressedNode = nullptr;
	mouseClicked = false;
	mouseMoved = false;
	QApplication::restoreOverrideCursor();
	view->setDragMode(QGraphicsView::NoDrag);
}

void OBSGraphicsScene::mouseClickEvent(QGraphicsSceneMouseEvent *event)
{
	if(pressedButton == Qt::LeftButton) {
		//TODO left click --> if on node, select it
	}
	else if(pressedButton == Qt::RightButton) {
		QApplication::changeOverrideCursor(Qt::ArrowCursor);
		if(pressedNode != nullptr) {
			// Right-click on node
			QMessageBox::StandardButton response = QMessageBox::question(
						view->viewport(),
						"Delete node?",
						QString::fromStdString("Are you sure that you want to delete the node '" + std::string(pressedNode->getBlueprintNode()->getDisplayName()) + "' ?"),
						QMessageBox::Yes | QMessageBox::No
					);
			if(response == QMessageBox::Yes) {
				graph->deleteNode(pressedNode->getBlueprintNode());
				for(auto connector: pressedNode->GUIOnly_getConnectors()) {
					if(GUINode* other = connector->getOtherNode(pressedNode)) {
						other->GUIOnly_removeConnector(connector); // Remove connector to other node list
						other->update();
					}
					removeItem(connector); // Delete node connectors
				}
				removeItem(pressedNode); // Delete node itself (will delete node pin's too)
			}
		}
		else if(GUIConnector* pressedConnector = getGUIConnectorAt(event->scenePos())) {
			// Right-click on connector
			graph->deleteConnector(pressedConnector->getBlueprintConnector());
			pressedConnector->getFromPin()->getParentNode()->GUIOnly_removeConnector(pressedConnector);
			pressedConnector->getFromPin()->update();
			pressedConnector->getToPin()->getParentNode()->GUIOnly_removeConnector(pressedConnector);
			pressedConnector->getToPin()->update();
			removeItem(pressedConnector);
		}
		else {
			// Right-click on the graph
			QMenu mouseContextMenu;
			auto actions = ContextMenuHelper::InitializeMenu(mouseContextMenu);
			QAction* choosen = mouseContextMenu.exec(event->screenPos());
			if(choosen != nullptr) {
				OBSBlueprintNode* newNode = actions[choosen]();
				graph->addNode(newNode);
				addGUINode(newNode, event->scenePos().x(), event->scenePos().y());
			}
		}
	}
}

void OBSGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseMoveEvent(event);
	mouseMoved = true;

	if(pressedButton == Qt::LeftButton) {
		if(pressedPin != nullptr) {
			// Moving from a pin --> update dragConnector
			QPointF p = pressedPin->scenePos();
			p += QPointF(GUI_PIN_SIZE/2, GUI_PIN_SIZE/2);
			dragConnector->setLine(p.x(), p.y(), event->scenePos().x(), event->scenePos().y());
		}
		else if(pressedNode != nullptr) {
			// Moving from a node --> move node
			QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
			pressedNode->setPos(event->scenePos() - pressedPosRelative);
			for(auto connector : pressedNode->GUIOnly_getConnectors()) {
				connector->redrawConnector();
			}
		}
	}
	else if(pressedButton == Qt::RightButton) {
		// Moving from the graph --> move view scene render bounds (done with scrollbars)
		QPointF movement = event->lastScreenPos() - event->screenPos(); // B - A
		view->horizontalScrollBar()->setValue(view->horizontalScrollBar()->value() + static_cast<int>(movement.x()));
		view->verticalScrollBar()->setValue(view->verticalScrollBar()->value() + static_cast<int>(movement.y()));
		// qDebug() << "Mouse moved at [" << movement << "]";
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

GUIPin* OBSGraphicsScene::getGUIPinAt(const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = view->items(view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == GUIPin::Type) {
			return dynamic_cast<GUIPin*>(item->toGraphicsObject());
		}
	}
	return nullptr;
}

void OBSGraphicsScene::addGUINode(OBSBlueprintNode *node, qreal px,
                                  qreal py)
{
	GUINode* guiNode = new GUINode(node);
	guiNode->setScale(1.5);
	guiNode->setPos(px, py);
	addItem(guiNode);
}

void OBSGraphicsScene::addGUIConnector(OBSBlueprintConnector *connector,
	GUIPin *from, GUIPin *to)
{
	GUIConnector* guiConnector = new GUIConnector(connector, from, to);
	from->getParentNode()->GUIOnly_addConnector(guiConnector);
	from->update();
	to->getParentNode()->GUIOnly_addConnector(guiConnector);
	to->update();
	addItem(guiConnector);
}

GUINode * OBSGraphicsScene::getGUINodeAt(const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = view->items(view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == GUINode::Type) {
			return dynamic_cast<GUINode*>(item->toGraphicsObject());
		}
	}
	return nullptr;
}

GUIConnector * OBSGraphicsScene::getGUIConnectorAt(
	const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = view->items(view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == GUIConnector::Type) {
			return dynamic_cast<GUIConnector*>(item->toGraphicsObject());
		}
	}
	return nullptr;
}

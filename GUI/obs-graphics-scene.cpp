#include "obs-graphics-scene.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QScrollBar>
#include <QGraphicsRectItem>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QWidgetAction>

#include "context-menu-helper.h"
#include "gui-graph.h"
#include "obs-graphics-connector.h"
#include "obs-graphics-node.h"
#include "obs-graphics-pin-input-field.h"
#include "obs-graphics-view.h"
#include "Core/obs-blueprint-graph.h"
#include "Helpers/pin-helper.h"

OBSGraphicsScene::OBSGraphicsScene(GUIContext& context,
                                   QObject *parent) : QGraphicsScene(parent), ctx(context)
{
	ctx.scene = this;
	ctx.view->onResizeEvent += resizeFunc;

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
}

void OBSGraphicsScene::initializeFromBlueprintGraph()
{
	// 1. Add main output pin
	graphVideoInputPin = new OBSGraphicsPin(ctx.graph->getMainVideoInputPin());
	graphVideoInputPin->setScale(3);
	pinMap[ctx.graph->getMainVideoInputPin()] = graphVideoInputPin;
	forceGraphGUIOnBottom();
	addItem(graphVideoInputPin);

	// 2. Add all nodes
	qreal posX = -1000;
	for(auto node : ctx.graph->getNodes()) {
		OBSGraphicsNode* guiNode = addGUINode(node, posX, 0);
		posX += guiNode->boundingRect().width() + 500;
	}

	// 3. Add all connectors
	for(auto connector: ctx.graph->getConnectors()) {
		addGUIConnector(connector, pinMap[connector->getFromPin()], pinMap[connector->getToPin()]);
	}
}

void OBSGraphicsScene::resetSelected()
{
	if(selectedNode) {
		selectedNode->setSelectionState(false);
		ctx.selectedNode = nullptr;
		ctx.onSelectionChanged.execute();
	}
	selectedNode = nullptr;
}

bool OBSGraphicsScene::isDropAccepted(const QPointF &scenePos) const
{
	if(getGraphicsNodeAt(scenePos))
		return false;

	return true;
}

void OBSGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mousePressEvent(event);

	pressedInputField = getGraphicsInputFieldAt(event->scenePos());
	if(pressedInputField != nullptr) {
		// Do nothing if user pressed on input field, Qwidget will manage it
		return;
	}

	// Prevent simultaneous mouse click events
	if(!mouseClicked) {
		mouseClicked = true;
		mouseMoved = false;

		pressedPin = getGraphicsPinAt(event->scenePos());
		pressedNode = getGraphicsNodeAt(event->scenePos());
		if(event->button() == Qt::LeftButton) {
			pressedButton = Qt::LeftButton;

			if(selectedNode != pressedNode) {
				resetSelected();
			}

			if(pressedPin != nullptr) {
				// Left click begin on pin
				QPen pen(PinColors::Get(pressedPin));
				pen.setWidth(10);
				QPointF p = pressedPin->scenePos();
				p += QPointF(pressedPin->sceneBoundingRect().width()/2, pressedPin->sceneBoundingRect().height()/2);
				dragConnector = addLine(p.x(),p.y(), event->scenePos().x(), event->scenePos().y(), pen);
			}
			else if(pressedNode != nullptr) {
				// Left-click begin on node
				QApplication::setOverrideCursor(Qt::OpenHandCursor);
				pressedPosRelative = event->scenePos() - pressedNode->pos();

				selectedNode = pressedNode;
				selectedNode->setSelectionState(true);
				ctx.selectedNode = selectedNode->getBlueprintNode();
				ctx.onSelectionChanged.execute();
			}
			else {
				// Left-click begin on the graph
				ctx.view->setDragMode(QGraphicsView::RubberBandDrag);
				QApplication::setOverrideCursor(Qt::CrossCursor);
				resetSelected();
			}
		}
		else if(event->button() == Qt::RightButton) {
			// Right-click begin on the graph
			pressedButton = Qt::RightButton;
			ctx.view->setDragMode(QGraphicsView::ScrollHandDrag);
			QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
		}

		// qDebug() << "Mouse button #" << event->button() << "pressed at [" << event->scenePos() << "]";
	}
}

void OBSGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseReleaseEvent(event);

	if(pressedInputField != nullptr) {
		// Do nothing if user pressed on input field, Qwidget will manage it
		pressedInputField = nullptr;
		return;
	}

	if(event->button() == pressedButton) {
		if(!mouseMoved) {
			mouseClickEvent(event);
		} else {
			if(pressedButton == Qt::LeftButton) {
				//TODO left moving click end --> end of rubber band (select all nodes inside)
				if(pressedPin != nullptr) {
					OBSGraphicsPin* destinationPin = getGraphicsPinAt(event->scenePos());
					if(destinationPin != nullptr) {
						// Drag from pin to other pin --> create connector if valid
						tryConnectPins(pressedPin, destinationPin);
					}
					else {
						OBSGraphicsNode* created = showContextMenu(event->scenePos());
						if(created) {
							for(auto pin : created->getGUIPins()) {
								// Loop on created node pins to find one to attach to
								if(pin->isInputPin() != pressedPin->isInputPin() && pin->getBlueprintPin()->getPinType() == pressedPin->getBlueprintPin()->getPinType()) {
									destinationPin = pin;
									break;
								}
							}
							if(destinationPin != nullptr) {
								tryConnectPins(pressedPin, destinationPin);
							}
						}
					}

					removeItem(dragConnector);
					delete dragConnector;
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
	pressedInputField = nullptr;
	mouseClicked = false;
	mouseMoved = false;
	QApplication::restoreOverrideCursor();
	ctx.view->setDragMode(QGraphicsView::NoDrag);
}

void OBSGraphicsScene::mouseClickEvent(QGraphicsSceneMouseEvent *event)
{
	if(pressedButton == Qt::LeftButton) {
		// TODO LEFT CLICK ?
	}
	else if(pressedButton == Qt::RightButton) {
		QApplication::changeOverrideCursor(Qt::ArrowCursor);
		if(pressedNode != nullptr) {
			// Right-click on node
			QMessageBox::StandardButton response = QMessageBox::question(
						ctx.view->viewport(),
						"Delete node?",
						QString::fromStdString("Delete the node '" + std::string(pressedNode->getBlueprintNode()->getDisplayName()) + "' ?"),
						QMessageBox::Yes | QMessageBox::No
					);
			if(response == QMessageBox::Yes) {
				removeGUINode(pressedNode);
			}
		}
		else if(OBSGraphicsConnector* pressedConnector = getGraphicsConnectorAt(event->scenePos())) {
			// Right-click on connector
			removeGUIConnector(pressedConnector);
		}
		else {
			// Right-click on the graph
			showContextMenu(event->scenePos());
		}
	}
}

void OBSGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseMoveEvent(event);

	if(pressedInputField != nullptr) {
		// Do nothing if user pressed on input field, Qwidget will manage it
		return;
	}

	mouseMoved = true;
	if(pressedButton == Qt::LeftButton) {
		if(pressedPin != nullptr) {
			// Moving from a pin --> update dragConnector
			QPointF p = pressedPin->scenePos();
			const QSizeF ps = pressedPin->sceneBoundingRect().size() / 2;
			p += QPointF(ps.width(), ps.height());
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
		ctx.view->horizontalScrollBar()->setValue(ctx.view->horizontalScrollBar()->value() + static_cast<int>(movement.x()));
		ctx.view->verticalScrollBar()->setValue(ctx.view->verticalScrollBar()->value() + static_cast<int>(movement.y()));
		forceGraphGUIOnBottom();
		// qDebug() << "Mouse moved at [" << movement << "]";
	}
}

void OBSGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	if(event->delta() < 0 && zoomLevel > -4) {
		--zoomLevel;
		ctx.view->scale(1.0/1.2, 1.0/1.2);
		forceGraphGUIOnBottom();
	}
	else if(event->delta() > 0 && zoomLevel < 5) {
		++zoomLevel;
		ctx.view->scale(1.2, 1.2);
		forceGraphGUIOnBottom();
	}
	qDebug() << "Zoom level: " << zoomLevel;
}

OBSGraphicsNode *OBSGraphicsScene::addGUINode(OBSBlueprintNode *node, qreal px,
                                              qreal py)
{

	OBSGraphicsNode* guiNode = new OBSGraphicsNode(node);
	guiNode->setScale(1.5);
	guiNode->setPos(px, py);

	for(OBSGraphicsPin* nodePin : guiNode->getGUIPins()) {
		pinMap[nodePin->getBlueprintPin()] = nodePin;
	}
	nodeMap[node] = guiNode;
	addItem(guiNode);
	return guiNode;
}

void OBSGraphicsScene::removeGUINode(OBSGraphicsNode *node)
{
	for(auto connector: node->GUIOnly_getConnectors()) {
		removeGUIConnector(connector);
	}
	for(OBSGraphicsPin* nodePin : node->getGUIPins()) {
		pinMap.remove(nodePin->getBlueprintPin());
	}

	ctx.graph->deleteNode(node->getBlueprintNode());

	nodeMap.remove(node->getBlueprintNode());
	removeItem(node); // Delete node itself (will delete node pin's too)
}

OBSGraphicsConnector *OBSGraphicsScene::addGUIConnector(
	OBSBlueprintConnector *connector,
	OBSGraphicsPin *from, OBSGraphicsPin *to)
{
	OBSGraphicsConnector* guiConnector = new OBSGraphicsConnector(connector, from, to);
	from->connect(guiConnector);
	to->connect(guiConnector);

	connectorMap[connector] = guiConnector;
	addItem(guiConnector);
	return guiConnector;
}

void OBSGraphicsScene::removeGUIConnector(OBSGraphicsConnector *connector)
{
	ctx.graph->deleteConnector(connector->getBlueprintConnector());

	if(OBSGraphicsPin* from = connector->getFromPin()) from->disconnect(connector);
	if(OBSGraphicsPin* to = connector->getToPin()) to->disconnect(connector);

	connectorMap.remove(connector->getBlueprintConnector());
	removeItem(connector);
}

OBSGraphicsPinInputField * OBSGraphicsScene::getGraphicsInputFieldAt(
	const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = ctx.view->items(ctx.view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == OBSGraphicsPinInputField::Type) {
			return dynamic_cast<OBSGraphicsPinInputField*>(item->toGraphicsObject());
		}
	}
	return nullptr;
}

OBSGraphicsConnector * OBSGraphicsScene::getGraphicsConnectorAt(
	const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = ctx.view->items(ctx.view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == OBSGraphicsConnector::Type) {
			OBSGraphicsConnector* connector = dynamic_cast<OBSGraphicsConnector*>(item->toGraphicsObject());
			// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
			QPointF p1 = connector->getFromPin()->scenePos();
			QSizeF  b1 = connector->getFromPin()->sceneBoundingRect().size();
			QPointF p2 = connector->getToPin()->scenePos();
			QSizeF  b2 = connector->getToPin()->sceneBoundingRect().size();
			qreal x0 = scenePos.x();
			qreal y0 = scenePos.y();
			qreal x1 = p1.x() + b1.width()/2.0;
			qreal y1 = p1.y() + b1.height()/2.0;
			qreal x2 = p2.x() + b2.width()/2.0;
			qreal y2 = p2.y() + b2.height()/2.0;
			qreal upperPart = qAbs((y2 - y1)*x0 - (x2 - x1)*y0 + x2*y1 - y2*x1);
			qreal lowerPart = qSqrt(qPow(y2 - y1, 2) + qPow(x2 - x1, 2));
			qreal distance = upperPart / lowerPart;

			if(distance < 40) // TODO distance based on zoom level?
				return connector;
		}
	}
	return nullptr;
}

OBSGraphicsNode * OBSGraphicsScene::getGraphicsNodeAt(const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = ctx.view->items(ctx.view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == OBSGraphicsNode::Type) {
			return dynamic_cast<OBSGraphicsNode*>(item->toGraphicsObject());
		}
	}
	return nullptr;
}

OBSGraphicsPin* OBSGraphicsScene::getGraphicsPinAt(const QPointF &scenePos) const
{
	// Get the graphics items at the mouse click position
	QList<QGraphicsItem*> items = ctx.view->items(ctx.view->mapFromScene(scenePos));

	// Check if the item is of type GUINode
	for(auto item: items) {
		if(item->type() == OBSGraphicsPin::Type) {
			return dynamic_cast<OBSGraphicsPin*>(item->toGraphicsObject());
		}
	}
	return nullptr;
}

bool OBSGraphicsScene::tryConnectPins(OBSGraphicsPin *A, OBSGraphicsPin *B)
{
	if(A != B // Cannot connect pin to itself
			&& A->getParentNode() != B->getParentNode() // Pins cannot be on same node
			&& A->getBlueprintPin()->getPinType() == B->getBlueprintPin()->getPinType() // Pins must be the same type
			&& A->isInputPin() != B->isInputPin()) // 1 pin should be input and the other output
	{
		OBSGraphicsPin* fromPin = A->isInputPin() ? B : A;
		OBSGraphicsPin* toPin = A->isInputPin() ? A : B;
		if(!toPin->isConnected()) {
			// Input pin is not connected --> create connector
			OBSBlueprintConnector* connector = ctx.graph->createConnector(fromPin->getOutputPin(), toPin->getInputPin());
			addGUIConnector(connector, fromPin, toPin);
			return true;
		}

		// Input pin is already connected --> show replace message
		std::string fromParentName = fromPin->getBlueprintPin()->getParentNode()->getDisplayName();
		std::string toParentName = toPin->getBlueprintPin()->getParentNode() ? toPin->getBlueprintPin()->getParentNode()->getDisplayName() : "Main video output";
		QMessageBox::StandardButton response = QMessageBox::question(
								ctx.view->viewport(),
								"Replace connector?",
								QString::fromStdString("Do you want to replace the connector between '" + fromParentName + "' and '" + toParentName + "' ?"),
								QMessageBox::Yes | QMessageBox::No
							);
		if(response == QMessageBox::Yes) {
			// Replace connector with newly created
			removeGUIConnector(toPin->getFirstConnector());
			OBSBlueprintConnector* connector = ctx.graph->createConnector(fromPin->getOutputPin(), toPin->getInputPin());
			addGUIConnector(connector, fromPin, toPin);
			return true;
		}
		return false;
	}
	return false;
}

OBSGraphicsNode *OBSGraphicsScene::showContextMenu(const QPointF &scenePos)
{
	QMenu mouseContextMenu;

	QWidgetAction* searchBar = new QWidgetAction(this);
	QLineEdit* searchLineEdit = new QLineEdit();
	searchLineEdit->setContentsMargins(5, 2, 5, 2);
	searchLineEdit->setPlaceholderText("Search...");

	searchBar->setDefaultWidget(searchLineEdit);
	mouseContextMenu.addAction(searchBar);
	mouseContextMenu.addSeparator();

	PinType inputFilter = pressedPin ? (pressedPin->isInputPin() ? ANY_PIN : pressedPin->getBlueprintPin()->getPinType()) : ANY_PIN;
	PinType outputFilter = pressedPin ? (pressedPin->isInputPin() ? pressedPin->getBlueprintPin()->getPinType() : ANY_PIN) : ANY_PIN;
	auto actions = ContextMenuHelper::InitializeMenu(mouseContextMenu, inputFilter, outputFilter);

	auto lambda = [&mouseContextMenu, &actions, searchBar, searchLineEdit, &inputFilter, &outputFilter](const QString& text) {
		mouseContextMenu.clear();
		mouseContextMenu.addAction(searchBar);
		mouseContextMenu.addSeparator();
		actions = ContextMenuHelper::InitializeMenu(mouseContextMenu, inputFilter, outputFilter, text);
		if(actions.second)
			mouseContextMenu.setActiveAction(actions.second);
		else
			mouseContextMenu.setActiveAction(searchBar);

		searchLineEdit->setFocus();
	};

	auto connector = connect(searchLineEdit, &QLineEdit::textChanged, lambda);
	auto connector2 = connect(&mouseContextMenu, &QMenu::aboutToShow, [searchLineEdit]{searchLineEdit->setFocus();});
	QAction* chosen = mouseContextMenu.exec(ctx.view->mapToGlobal(ctx.view->mapFromScene(scenePos)));//event->screenPos());
	disconnect(connector);
	disconnect(connector2);

	if(chosen != nullptr && chosen != searchBar && !chosen->isSeparator()) {
		OBSBlueprintNode* newNode = actions.first[chosen]();
		ctx.graph->addNode(newNode);
		return addGUINode(newNode, scenePos.x(), scenePos.y());
	}

	return nullptr;
}

void OBSGraphicsScene::forceGraphGUIOnBottom()
{
	if(graphVideoInputPin != nullptr && ctx.view != nullptr) {
		QRectF viewport = ctx.view->getSceneViewport();
		qreal pinWidth = graphVideoInputPin->sceneBoundingRect().width();
		qreal pinHeight = graphVideoInputPin->sceneBoundingRect().height();
		graphVideoInputPin->setPos(
			viewport.x() + (( viewport.width()/ 2) - (pinWidth / 2)),
			viewport.y() + (viewport.height() - pinHeight)
		);
		if(graphVideoInputPin->getFirstConnector()) graphVideoInputPin->getFirstConnector()->redrawConnector();
	}
}

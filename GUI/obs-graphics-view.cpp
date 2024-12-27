#include "obs-graphics-view.h"

#include <QGuiApplication>
#include <QMenu>
#include <QScrollBar>

#include "obs-graphics-main-window.h"
#include "obs-graphics-scene.h"
#include "Core/obs-blueprint-variable.h"
#include "Nodes/Variables/node-variable-get.h"
#include "Variables/gui-variables-widget.h"
#include "Variables/obs-graphics-variables-list.h"

OBSGraphicsView::OBSGraphicsView(GUIContext& context, QWidget *parent) : QGraphicsView(parent), ctx(context)
{
	ctx.view = this;

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setDragMode(QGraphicsView::NoDrag);
	setAcceptDrops(true);
}

QRectF OBSGraphicsView::getSceneViewport() const
{
	auto vp = viewport();
	QPointF A = mapToScene(QPoint(0, 0));
	QPointF B = mapToScene(QPoint(vp->width(), vp->height()));
	return {A, B};
}

void OBSGraphicsView::resetScroll()
{
	mouseRightClick = true;
	mouseMove = true;
	horizontalScrollBar()->setValue(-width() / 2);
	verticalScrollBar()->setValue(-height() / 2);
	mouseRightClick = false;
	mouseMove = false;
}

void OBSGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton) mouseRightClick = true;
	QGraphicsView::mousePressEvent(event);
}

void OBSGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	mouseMove = true;
	QGraphicsView::mouseMoveEvent(event);
}

void OBSGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	mouseRightClick = false;
	mouseMove = false;
	QGraphicsView::mouseReleaseEvent(event);
}

void OBSGraphicsView::scrollContentsBy(int dx, int dy)
{
	if(canScrollContent()) {
		QGraphicsView::scrollContentsBy(dx, dy);
	}
}

void OBSGraphicsView::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);
	onResizeEvent.execute(event);
}

void OBSGraphicsView::wheelEvent(QWheelEvent *event)
{
	int valueH = horizontalScrollBar()->value();
	int valueV = verticalScrollBar()->value();
	QGraphicsView::wheelEvent(event);
	horizontalScrollBar()->setValue(valueH);
	verticalScrollBar()->setValue(valueV); // FIX position bug when zooming
}

void OBSGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->source() == ctx.varsList)
		event->accept();
	else
		QGraphicsView::dragEnterEvent(event);
}

void OBSGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
	if(event->source() == ctx.varsList && ctx.scene->isDropAccepted(mapToScene(event->position().toPoint())))
			event->accept();
	else
		QGraphicsView::dragMoveEvent(event);
}

void OBSGraphicsView::dropEvent(QDropEvent *event)
{
	if(event->source() == ctx.varsList && ctx.scene->isDropAccepted(mapToScene(event->position().toPoint()))) {
		event->accept();
		OBSBlueprintVariable* variable = ctx.varsWidget->itemToVariable(ctx.varsList->selectedItem());
		OBSBlueprintNode* node = nullptr;

		Qt::KeyboardModifiers keys = QGuiApplication::queryKeyboardModifiers();
		bool ctrl = keys.testFlag(Qt::KeyboardModifier::ControlModifier);
		bool alt = keys.testFlag(Qt::KeyboardModifier::AltModifier);
		if(ctrl && !alt) {
			// Drop with ctrl pressed --> GET node
			node = new NodeVariableGet(variable);
		}
		else if(!ctrl && alt) {
			// Drop with alt pressed --> SET node
			// TODO: SET NODE
		}
		else {
			// None or both pressed --> show context menu
			QMenu menu;
			QAction* get = menu.addAction("Get '" + QString(variable->getDisplayName()) + "'");
			QAction* set = menu.addAction("Set '" + QString(variable->getDisplayName()) + "'");
			QAction* action = menu.exec(mapToGlobal(event->position()).toPoint());
			if(action == get)
				node = new NodeVariableGet(variable);
			else if(action == set)
				action = nullptr;// TODO: NODE SET
		}

		if(node != nullptr) {
			QPointF pos = mapToScene(event->position().toPoint());
			ctx.scene->addGUINode(node, pos.x(), pos.y());
		}
	}
	else
		QGraphicsView::dropEvent(event);
}

void OBSGraphicsView::focusInEvent(QFocusEvent *event)
{
	QGraphicsView::focusInEvent(event);
	if (ctx.varsList != nullptr)
		ctx.varsList->resetSelected();
}

#include "obs-graphics-view.h"

#include <qevent.h>
#include <QScrollBar>

OBSGraphicsView::OBSGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
	setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	setMinimumWidth(300);
	setMinimumHeight(300);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// setTransformationAnchor(ViewportAnchor::NoAnchor);
	setDragMode(QGraphicsView::NoDrag);
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

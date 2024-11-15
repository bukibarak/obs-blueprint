#include "obs-graphics-view.h"

#include <qevent.h>
#include <QScrollBar>

OBSGraphicsView::OBSGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setDragMode(QGraphicsView::NoDrag);
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
	qDebug() << "onResizeEvent called";
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

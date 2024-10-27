#pragma once
#include <QGraphicsView>

#include "Structs/multicast-delegate.h"

class OBSGraphicsView: public QGraphicsView {
public:
	OBSGraphicsView(QWidget* parent = nullptr);

	QRectF getSceneViewport() const;
	void resetScroll();

	multicastDelegate_OneParam<QResizeEvent*> onResizeEvent;

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void scrollContentsBy(int dx, int dy) override;
	void resizeEvent(QResizeEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

private:

	bool mouseRightClick = false;
	bool mouseMove = false;
	bool canScrollContent() { return mouseRightClick && mouseMove; }
};

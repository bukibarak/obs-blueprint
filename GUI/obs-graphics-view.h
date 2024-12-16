#pragma once
#include <QGraphicsView>

#include "Helpers/global-logger.h"
#include "Structs/multicast-delegate.h"

struct GUIContext;

class OBSGraphicsView: public QGraphicsView {
public:
	OBSGraphicsView(GUIContext& context, QWidget* parent = nullptr);
	~OBSGraphicsView() override { GDebug("[GUI] Deleting QWidget 'OBS Graphics View'...");}

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

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	void focusInEvent(QFocusEvent *event) override;

private:

	GUIContext& ctx;

	bool mouseRightClick = false;
	bool mouseMove = false;
	bool canScrollContent() const { return mouseRightClick && mouseMove; }
};

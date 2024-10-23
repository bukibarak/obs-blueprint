#pragma once
#include <QGraphicsView>

class OBSGraphicsView: public QGraphicsView {
public:
	OBSGraphicsView(QWidget* parent = nullptr);

	void resetScroll();

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void scrollContentsBy(int dx, int dy) override;

private:

	bool mouseRightClick = false;
	bool mouseMove = false;
	bool canScrollContent() { return mouseRightClick && mouseMove; }
};

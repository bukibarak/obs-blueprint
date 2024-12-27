#include "obs-graphics-main-window.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <sstream>

#include "obs-graphics-scene.h"
#include "obs-graphics-view.h"
#include "Details/gui-details-widget.h"
#include "Variables/gui-variables-widget.h"

OBSGraphicsMainWindow::OBSGraphicsMainWindow(OBSBlueprintGraph *g, QWidget *parent, Qt::WindowFlags f)
	: QWidget(parent, f), graph(g)
{
	setWindowTitle("OBS Blueprint Graph Editor");

	ctx.graph = graph;
	ctx.mainWindow = this;

	view = new OBSGraphicsView(ctx, this);
	scene = new OBSGraphicsScene(ctx, this);
	view->setScene(scene);

	varWidget = new GUIVariablesWidget(ctx, this);
	detailsWidget = new GUIDetailsWidget(ctx, this);

	splitter = new QSplitter(this);
	splitter->addWidget(varWidget);
	splitter->addWidget(view);
	splitter->addWidget(detailsWidget);
	varWidget->resize(750, varWidget->height());
	detailsWidget->resize(1000, detailsWidget->height());

	layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(splitter);

	scene->initializeFromBlueprintGraph();

	view->resetTransform();
	view->scale(0.25, 0.25);
	scene->resetZoomLevel();
	view->resetScroll();
}

OBSGraphicsMainWindow::~OBSGraphicsMainWindow()
{
	delete layout;
	delete splitter;
	// delete view; // not needed, deleted when splitter is deleted
	// delete varWidget;
	// delete detailsWidget;
	delete scene;
	if (onDestructorEnd)
		onDestructorEnd();
}

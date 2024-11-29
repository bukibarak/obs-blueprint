#include "gui-graph.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QWindow>
#include <QSplitter>

#include "obs-frontend-api.h"
#include "obs-graphics-scene.h"
#include "obs-graphics-view.h"
#include "Details/gui-details-widget.h"
#include "Variables/gui-variables-widget.h"


GUIGraph::GUIGraph(OBSBlueprintGraph *attachedGraph) : graph(attachedGraph)
{

	ctx.graph = graph;
	ctx.GUIgraph = this;
	window = new QWidget(static_cast<QWidget *>(obs_frontend_get_main_window()), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	view = new OBSGraphicsView(ctx, window);
	scene = new OBSGraphicsScene(ctx);
	view->setScene(scene);

	varWidget = new GUIVariablesWidget(ctx, window);
	detailsWidget = new GUIDetailsWidget(ctx, window);


	QSplitter* splitter = new QSplitter();
	splitter->addWidget(varWidget);
	splitter->addWidget(view);
	splitter->addWidget(detailsWidget);
	varWidget->resize(500, varWidget->height());
	detailsWidget->resize(700, detailsWidget->height());

	QHBoxLayout *layout = new QHBoxLayout(window);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(splitter);


	scene->initializeFromBlueprintGraph();
}

GUIGraph::~GUIGraph()
{
	ctx.onDeletion.execute();
	window->deleteLater(); // WARNING: This might delete all Qt Objects AFTER the graph is deleted. This means, all pointers to OBSBlueprint objects might be invalid!
}


void GUIGraph::show() const
{
	// Hide default OBS properties window, TODO how to do it better? maybe even prevent properties window creation?
	const QWindowList list = QApplication::topLevelWindows();

	// Reverse list loop for better performance
	QList<QWindow*>::const_iterator it = list.constEnd();
	while(it != list.constBegin()) {
		--it;
		if((*it)->objectName() == "OBSBasicPropertiesWindow") {
			(*it)->close();
			break;
		}
	}

	view->resetTransform();
	view->scale(0.25, 0.25);
	scene->resetZoomLevel();
	view->resetScroll();

	window->adjustSize();
	QRect screenRect = QApplication::primaryScreen()->geometry();
	QRect windowRect = window->geometry();
	window->move((screenRect.width() - windowRect.width())/2, (screenRect.height() - windowRect.height())/2);
	window->show();
}

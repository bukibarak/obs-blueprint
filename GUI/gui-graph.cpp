#include "gui-graph.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QWindow>
#include <QSplitter>

#include "gui-variables-widget.h"
#include "obs-frontend-api.h"
#include "obs-graphics-scene.h"
#include "obs-graphics-view.h"


GUIGraph::GUIGraph(OBSBlueprintGraph *attachedGraph) : graph(attachedGraph)
{
	window = new QWidget(static_cast<QWidget *>(obs_frontend_get_main_window()), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	view = new OBSGraphicsView();
	scene = new OBSGraphicsScene(graph);
	scene->setView(view);
	view->setScene(scene);

	QWidget* leftPart = new GUIVariablesWidget(graph, window);

	QSplitter* splitter = new QSplitter();
	splitter->addWidget(leftPart);
	splitter->addWidget(view);

	QHBoxLayout *layout = new QHBoxLayout(window);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(splitter);


	scene->initializeFromBlueprintGraph();
}

GUIGraph::~GUIGraph()
{
	view->deleteLater();
}


void GUIGraph::show()
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

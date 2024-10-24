#include "gui-graph.h"

#include <QApplication>
#include <QGraphicsView>
#include <QScrollBar>
#include <QWindow>

#include "obs-frontend-api.h"


GUIGraph::GUIGraph(OBSBlueprintGraph *attachedGraph) : graph(attachedGraph)
{
	view = new OBSGraphicsView(static_cast<QWidget *>(obs_frontend_get_main_window()));
	scene = new OBSGraphicsScene(graph);
	scene->setView(view);
	view->setScene(scene);

	if(graph->graphNodes.size() != 0) {
		scene->addGUINode(graph->graphNodes.front(), 500, 500);
	}

}

GUIGraph::~GUIGraph()
{
	view->invalidateScene();
	view->close();
	// delete scene;
	// delete view;

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
	view->show();
	view->resetScroll();

}

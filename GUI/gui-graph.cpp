#include "gui-graph.h"

#include <iostream>
#include <QApplication>
#include <QScreen>
#include <QWindow>

#include "obs-frontend-api.h"
#include "obs-graphics-main-window.h"
#include "Core/obs-blueprint-graph.h"
#include "Helpers/global-logger.h"


GUIHandler::GUIHandler(OBSBlueprintGraph *g) : graph(g)
{
	QWidget *parent = static_cast<QWidget *>(obs_frontend_get_main_window());
	window = new OBSGraphicsMainWindow(graph, parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	window->deleteComplete = [this] {
		GDebug("[GUI] >> Main Window DESTROYED! <<");
		window = nullptr;
		cond.notify_all();
	};
}

GUIHandler::~GUIHandler()
{
	if (window != nullptr) {
		std::mutex mtx{};
		std::unique_lock lock{mtx};
		window->deleteLater();
		cond.wait_for(lock, std::chrono::milliseconds(100)); // TODO improve this. For now GUI is still deleted AFTER graph if closed
		if (window != nullptr) {
			window->context().onDeletion.execute();
		}
	}
}


void GUIHandler::show() const
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

	window->adjustSize(); // Size to content

	// Center window on screen
	QRect screenRect = QApplication::primaryScreen()->geometry();
	QRect windowRect = window->geometry();
	window->move((screenRect.width() - windowRect.width())/2, (screenRect.height() - windowRect.height())/2);
	window->show();
}

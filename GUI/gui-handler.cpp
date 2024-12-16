#include "gui-handler.h"

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
	obs_frontend_add_event_callback(OBSEvent, this);
	QWidget *parent = static_cast<QWidget *>(obs_frontend_get_main_window());
	window = new OBSGraphicsMainWindow(graph, parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	window->onDestructorEnd = [this] {
		GDebug("[GUI] OBS Blueprint main window Destroyed!");
		window = nullptr;
		cond.notify_all();
	};
}

GUIHandler::~GUIHandler()
{
	obs_frontend_remove_event_callback(OBSEvent, this);
	if (window != nullptr) {
		GDebug("[GUI] Destroy OBS BP Main window using deleteLater() and lock");
		std::mutex mtx{};
		std::unique_lock lock{mtx};
		window->deleteLater();
		cond.wait_for(lock, std::chrono::seconds(1));
	}
}

void GUIHandler::OBSEvent(enum obs_frontend_event event, void *ptr)
{
	if (event == OBS_FRONTEND_EVENT_SCRIPTING_SHUTDOWN) {
		GDebug("[GUI] Destroy OBS BP Main window using OBS Frontend API Event");
		GUIHandler *handler = static_cast<GUIHandler *>(ptr);
		delete handler->window;
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

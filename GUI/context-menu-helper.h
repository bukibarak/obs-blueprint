#pragma once

#include "Nodes/Video/node-color-source.h"
#include "Nodes/Waves/node-sinus-time.h"
#include "Nodes/Numbers/Float/node-float-abs.h"
#include "Nodes/Numbers/Float/node-float-to-int.h"
#include "Nodes/Numbers/Int/node-int-to-float.h"
#include "Nodes/Video/node-image-souce.h"
#include "Nodes/Video/Transforms/node-video-layer.h"
#include "Nodes/Video/Utilities/node-video-break.h"

struct ContextMenuEntry {
	QString displayText;
	QString path;

	std::function<OBSBlueprintNode*()> create;
	std::vector<PinType> inputPins;
	std::vector<PinType> outputPins;

	template<class T> static ContextMenuEntry Add(
		const QString &displayText, const QString &path = "/")
	{
		ContextMenuEntry entry;
		entry.displayText = displayText;
		entry.path = path;

		auto context = NodeContextInfo::GetDefaults<T>();
		entry.create = context.create;
		entry.inputPins = context.inputPins;
		entry.outputPins = context.outputPins;
		return entry;
	}

private:
	ContextMenuEntry() = default;
};

class ContextMenuHelper {
public:
	static QPair<QMap<QAction *, std::function<OBSBlueprintNode*()>>,
	             QAction *> InitializeMenu(QMenu &rootMenu,
	                                       PinType inputFilter = ANY_PIN,
	                                       PinType outputFilter = ANY_PIN,
	                                       const QString &searchText = "")
	{
		QMap<QAction *, std::function<OBSBlueprintNode*()>> actions;
		QAction *firstAction = nullptr;

		if (searchText.isEmpty()) {
			// If there is no search term, show the context menu with submenus
			std::unordered_map<QString, QMenu *> pathMenus;
			pathMenus["/"] = &rootMenu;
			for (const auto &entry : entries) {

				// Filter entry if necessary
				if((inputFilter != ANY_PIN && std::find(entry.inputPins.begin(), entry.inputPins.end(), inputFilter) == entry.inputPins.end())
					|| outputFilter != ANY_PIN && std::find(entry.outputPins.begin(), entry.outputPins.end(), outputFilter) == entry.outputPins.end())
					continue;

				if (pathMenus.find(entry.path) == pathMenus.end()) {
					// If the menu does not already exist, create it and link it to the submenu
					AddMenusRecursive(pathMenus, entry.path,&rootMenu,0);
				}

				//Add the action to the corresponding menu
				QAction *action = pathMenus[entry.path]->addAction(entry.displayText);
				actions[action] = entry.create;
				if (firstAction == nullptr)
					firstAction = action;
			}
		}
		else {
			// If there is a search term, directly add actions that matches.
			for (const auto &entry : entries) {

				// Filter entry if necessary
				if((inputFilter != ANY_PIN && std::find(entry.inputPins.begin(), entry.inputPins.end(), inputFilter) == entry.inputPins.end())
					|| outputFilter != ANY_PIN && std::find(entry.outputPins.begin(), entry.outputPins.end(), outputFilter) == entry.outputPins.end())
					continue;

				if (entry.displayText.contains(searchText, Qt::CaseInsensitive)) {
					QAction *action = rootMenu.addAction(entry.displayText);
					actions[action] = entry.create;
					if (firstAction == nullptr)
						firstAction = action;
				}
			}
		}

		return {actions, firstAction};
	}

private:
	static void AddMenusRecursive(std::unordered_map<QString, QMenu *> &pathMenus,
	                              const QString &fullPath,
	                              QMenu *parentMenu, qsizetype index)
	{
		if (index == -1)
			return; // Break recursion condition

		QString substring = fullPath.mid(index + 1);
		qsizetype substringIndex = substring.indexOf('/');
		qsizetype nextIndex = substringIndex == -1
			                      ? -1
			                      : substringIndex + index + 1;

		QString menuName = substringIndex == -1
			                   ? substring
			                   : substring.first(substringIndex);
		QString menuIndex = nextIndex == -1
			                    ? fullPath
			                    : fullPath.first(nextIndex);

		if (pathMenus.find(menuIndex) == pathMenus.end()) {
			// Create menu if not existing
			pathMenus[menuIndex] = parentMenu->addMenu(menuName);
		}
		AddMenusRecursive(pathMenus,fullPath,pathMenus[menuIndex],nextIndex);
	}

	inline static std::vector<ContextMenuEntry> entries{
		ContextMenuEntry::Add<NodeColorSource>("Color source"),
		ContextMenuEntry::Add<NodeImageSource>("Image source"),
		ContextMenuEntry::Add<NodeFloatAbs>("Absolute value (float)", "/Numbers/Float"),
		ContextMenuEntry::Add<NodeFloatToInt>("Float to Integer", "/Numbers/Float"),
		ContextMenuEntry::Add<NodeIntToFloat>("Integer to float", "/Numbers/Integer"),
		ContextMenuEntry::Add<NodeVideoLayer>("Layer (video)", "/Video"),
		ContextMenuEntry::Add<NodeVideoBreak>("Break (video)", "/Video"),
		ContextMenuEntry::Add<NodeSinusTime>("Sinus wave", "/Timeline"),
	};
};

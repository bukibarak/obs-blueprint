#pragma once
#include <QMenu>

#include "Core/obs-blueprint-node.h"
#include "Nodes/node-color-source.h"
#include "Nodes/node-sinus-time.h"
#include "Nodes/FloatHelpers/node-float-abs.h"
#include "Nodes/FloatHelpers/node-float-to-int.h"

struct ContextMenuEntry {
	ContextMenuEntry() = delete;
	ContextMenuEntry(QString displayText, const std::function<OBSBlueprintNode*()>& nodeCreationCallback) :
		displayText(std::move(displayText)), createNode(nodeCreationCallback) {}
	ContextMenuEntry(QString path, QString displayText, const std::function<OBSBlueprintNode*()>& nodeCreationCallback)
		: displayText(std::move(displayText)), path(std::move(path)), createNode(nodeCreationCallback) {}

	QString displayText;
	QString path = "/";
	std::function<OBSBlueprintNode*()> createNode;

};

class ContextMenuHelper {
public:

	static std::map<QAction*, std::function<OBSBlueprintNode*()>> InitializeMenu(QMenu& rootMenu)
	{
		std::map<QAction*, std::function<OBSBlueprintNode*()>> actions;
		std::map<QString, QMenu*> pathMenus;
		pathMenus["/"] = &rootMenu;
		for(const auto& entry: entries) {
			if(pathMenus.find(entry.path) == pathMenus.end()) {
				// If the menu does not already exist, create it and link it to the submenu
				AddMenusRecursive(pathMenus, entry.path, &rootMenu, 0);
				// QMenu* parentMenu = &rootMenu;
				// QString menuName;
				// qsizetype index = entry.path.lastIndexOf('/');
				// if(index > 0) {
				// 	menuName = entry.path.mid(index + 1);
				// 	QString parentPath = entry.path.first(index);
				// 	if(const auto& it = pathMenus.find(parentPath); it != pathMenus.end()) {
				// 		parentMenu = it->second;
				// 	}
				// } else {
				// 	menuName = entry.path.mid(1);
				// }
				// pathMenus[entry.path] = parentMenu->addMenu(menuName);
			}

			//Add the action to the corresponding menu
			actions[pathMenus[entry.path]->addAction(entry.displayText)] = entry.createNode;
		}
		return actions;
	}

	// static void AddEntry(const ContextMenuEntry& entry)
	// {
	// 	entries.push_back(entry);
	// }

private:

	static void AddMenusRecursive(std::map<QString, QMenu*>& pathMenus, const QString& fullPath, QMenu* parentMenu, qsizetype index)
	{
		if(index == -1) return; // Break recursion condition

		QString substring = fullPath.mid(index + 1);
		qsizetype substringIndex = substring.indexOf('/');
		qsizetype nextIndex = substringIndex == -1 ? -1 : substringIndex + index + 1;

		QString menuName = substringIndex == -1 ? substring : substring.first(substringIndex);
		QString menuIndex = nextIndex == -1 ? fullPath : fullPath.first(nextIndex);

		if(pathMenus.find(menuIndex) == pathMenus.end()) {
			// Create menu if not existing
			pathMenus[menuIndex] = parentMenu->addMenu(menuName);
		}
		AddMenusRecursive(pathMenus, fullPath, pathMenus[menuIndex], nextIndex);
	}

	inline static std::vector<ContextMenuEntry> entries{
		ContextMenuEntry("/Video", "Color Video source", []{return new NodeColorSource(100, 100, 0xFFFF00FF);}),
		ContextMenuEntry("/Float", "Absolute value", []{return new NodeFloatAbs();}),
		ContextMenuEntry("/Float", "Float to Integer", []{return new NodeFloatToInt();}),
		ContextMenuEntry("/Float", "Sinus wave", []{return new NodeSinusTime(1.0f, 200.0f);}),
	};
};

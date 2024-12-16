#pragma once
#include <QWidget>

#include "Structs/multicast-delegate.h"

class QHBoxLayout;
class QSplitter;
class OBSGraphicsMainWindow;
class OBSBlueprintNode;
class OBSBlueprintVariable;
class OBSGraphicsVariablesList;
class GUIDetailsWidget;
class GUIVariablesWidget;
class OBSGraphicsView;
class OBSGraphicsScene;
class OBSBlueprintGraph;

struct GUIContext {
	OBSBlueprintGraph* graph = nullptr;
	OBSGraphicsMainWindow* mainWindow = nullptr;
	GUIVariablesWidget* varsWidget = nullptr;
	OBSGraphicsVariablesList* varsList = nullptr;
	GUIDetailsWidget* detailsWidget = nullptr;
	OBSGraphicsScene* scene = nullptr;
	OBSGraphicsView* view = nullptr;

	OBSBlueprintVariable* selectedVariable = nullptr;
	OBSBlueprintNode* selectedNode = nullptr;
	multicastDelegate_ZeroParam onSelectionChanged;
	multicastDelegate_ZeroParam onDeletion;
};

class OBSGraphicsMainWindow : public QWidget {
public:
	OBSGraphicsMainWindow(OBSBlueprintGraph* g, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~OBSGraphicsMainWindow() override;

	GUIContext& context() {return ctx;}

	std::function<void()> deleteComplete;

private:

	GUIContext ctx{};

	OBSBlueprintGraph* graph;
	OBSGraphicsScene* scene;
	OBSGraphicsView* view;
	GUIVariablesWidget* varWidget;
	GUIDetailsWidget* detailsWidget;
	QSplitter* splitter;
	QHBoxLayout *layout;
};

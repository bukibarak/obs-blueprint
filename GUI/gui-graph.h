#pragma once
#include "Structs/multicast-delegate.h"


class OBSBlueprintNode;
class OBSBlueprintVariable;
class GUIDetailsWidget;
class GUIGraph;
class OBSGraphicsVariablesList;
class GUIVariablesWidget;
class QWidget;
class OBSGraphicsView;
class OBSGraphicsScene;
class OBSBlueprintGraph;

struct GUIContext {
	OBSBlueprintGraph* graph = nullptr;
	GUIGraph* GUIgraph = nullptr;
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

class GUIGraph {
public:
	GUIGraph() = delete;
	GUIGraph(const GUIGraph&) = delete;
	GUIGraph(GUIGraph&&) = delete;
	~GUIGraph();

	GUIGraph(OBSBlueprintGraph* attachedGraph);

	void show();

private:

	GUIContext ctx{};

	QWidget* window;
	OBSGraphicsScene* scene;
	OBSGraphicsView* view;
	GUIVariablesWidget* varWidget;
	GUIDetailsWidget* detailsWidget;
	OBSBlueprintGraph* graph;

};

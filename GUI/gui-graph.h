#pragma once


class QWidget;
class OBSGraphicsView;
class OBSGraphicsScene;
class OBSBlueprintGraph;

class GUIGraph {
public:
	GUIGraph() = delete;
	GUIGraph(const GUIGraph&) = delete;
	GUIGraph(GUIGraph&&) = delete;
	~GUIGraph();

	GUIGraph(OBSBlueprintGraph* attachedGraph);

	void show();

private:

	QWidget* window;
	OBSGraphicsScene* scene;
	OBSGraphicsView* view;
	OBSBlueprintGraph* graph;

};

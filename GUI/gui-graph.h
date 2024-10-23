#pragma once
#include "obs-graphics-scene.h"
#include "obs-graphics-view.h"
#include "Core/obs-blueprint-graph.h"

class GUIGraph {
public:
	GUIGraph() = delete;
	GUIGraph(const GUIGraph&) = delete;
	GUIGraph(GUIGraph&&) = delete;
	~GUIGraph();

	GUIGraph(OBSBlueprintGraph* attachedGraph);

	void show();

private:

	OBSGraphicsScene* scene;
	OBSGraphicsView* view;
	OBSBlueprintGraph* graph;

};

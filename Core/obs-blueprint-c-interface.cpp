#include "obs-blueprint-c-interface.h"
#include "obs-blueprint-graph.h"

extern "C"
{
	CPP_BlueprintGraphPtr c_blueprint_graph_create()
	{
		return new OBSBlueprintGraph();
	}

	void c_blueprint_graph_destroy(CPP_BlueprintGraphPtr graph)
	{
		delete static_cast<OBSBlueprintGraph *>(graph);
	}

	void c_blueprint_graph_properties_click(CPP_BlueprintGraphPtr graph)
	{
		static_cast<OBSBlueprintGraph *>(graph)->sourcePropertiesClick();
	}

	void c_blueprint_graph_tick(CPP_BlueprintGraphPtr graph, float seconds)
	{
		static_cast<OBSBlueprintGraph *>(graph)->tick(seconds);
	}

	unsigned char* c_blueprint_graph_get_pixels(CPP_BlueprintGraphPtr graph)
	{
		return static_cast<OBSBlueprintGraph *>(graph)->getRenderPixels();
	}

	unsigned int c_blueprint_graph_get_width(CPP_BlueprintGraphPtr graph)
	{
		return static_cast<OBSBlueprintGraph *>(graph)->getWidth();
	}

	unsigned int c_blueprint_graph_get_height(CPP_BlueprintGraphPtr graph)
	{
		return static_cast<OBSBlueprintGraph *>(graph)->getHeight();
	}
}

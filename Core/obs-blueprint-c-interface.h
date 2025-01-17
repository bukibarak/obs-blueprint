#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	typedef void* CPP_BlueprintGraphPtr;


	CPP_BlueprintGraphPtr c_blueprint_graph_create();
	void c_blueprint_graph_destroy(CPP_BlueprintGraphPtr);
	void c_blueprint_graph_properties_click(CPP_BlueprintGraphPtr);
	void c_blueprint_graph_tick(CPP_BlueprintGraphPtr, float);
	unsigned char* c_blueprint_graph_get_pixels(CPP_BlueprintGraphPtr);
	unsigned int c_blueprint_graph_get_width(CPP_BlueprintGraphPtr);
	unsigned int c_blueprint_graph_get_height(CPP_BlueprintGraphPtr);

#ifdef __cplusplus
}
#endif

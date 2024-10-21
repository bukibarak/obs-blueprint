#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	struct pixel;
	typedef void* CPP_BlueprintGraphPtr;


	CPP_BlueprintGraphPtr c_blueprint_graph_create();
	void c_blueprint_graph_destroy(CPP_BlueprintGraphPtr);
	void c_blueprint_graph_tick(CPP_BlueprintGraphPtr, float);
	struct pixel* c_blueprint_graph_get_pixels(CPP_BlueprintGraphPtr);
	unsigned int c_blueprint_graph_get_width(CPP_BlueprintGraphPtr);
	unsigned int c_blueprint_graph_get_height(CPP_BlueprintGraphPtr);

#ifdef __cplusplus
}
#endif

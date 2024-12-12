#include <obs-module.h>
#include "Core/obs-blueprint-c-interface.h"

struct blueprint_source {
	CPP_BlueprintGraphPtr graph;
	struct pixel* pixels;
	uint32_t width;
	uint32_t height;
	gs_texture_t* texture;
	obs_source_t* source;
	bool nullReset;
};

static const char *obs_blueprint_source_name(void* unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("SourceName");
}

static uint32_t obs_blueprint_source_width(void* data)
{
	struct blueprint_source* context = data;
	return context->width;
}

static uint32_t obs_blueprint_source_height(void* data)
{
	struct blueprint_source* context = data;
	return context->height;
}

static obs_properties_t* obs_blueprint_source_properties(void* data)
{
	struct blueprint_source* context = data;
	c_blueprint_graph_properties_click(context->graph);
	return NULL;
}

static void* obs_blueprint_source_create(obs_data_t* settings, obs_source_t* source)
{
	UNUSED_PARAMETER(settings);

	struct blueprint_source* context = bzalloc(sizeof(struct blueprint_source)); // MemAlloc and set all ptr/data to NULL/0
	context->source = source;
	context->graph = c_blueprint_graph_create();

	return context;
}

static void obs_blueprint_source_destroy(void* data)
{
	struct blueprint_source* context = data;
	c_blueprint_graph_destroy(context->graph);
	if (context->texture != NULL) {
		obs_enter_graphics();
		gs_texture_destroy(context->texture);
		obs_leave_graphics();
	}

	bfree(context);
}

static void obs_blueprint_source_draw(gs_texture_t* texture, gs_effect_t* effect)
{
	// TODO maybe improve this, I just copy-paste some things from win-capture/image-source render functions
	const bool previous = gs_framebuffer_srgb_enabled();
	gs_enable_framebuffer_srgb(true);

	gs_blend_state_push();
	gs_blend_function(GS_BLEND_ONE, GS_BLEND_INVSRCALPHA);
	gs_eparam_t *const param = gs_effect_get_param_by_name(effect, "image");
	gs_effect_set_texture_srgb(param, texture);
	gs_draw_sprite(texture, 0, 0, 0);
	gs_blend_state_pop();
	gs_enable_framebuffer_srgb(previous);
}

static void obs_blueprint_source_render(void* data, gs_effect_t* effect)
{
	struct blueprint_source* context = data;
	uint32_t width = c_blueprint_graph_get_width(context->graph);
	uint32_t height = c_blueprint_graph_get_height(context->graph);
	struct pixel* pixels = c_blueprint_graph_get_pixels(context->graph);

	if (pixels == NULL || width == 0 || height == 0) {
		// No pixels found in graph, do not render anything
		if (context->nullReset) {
			context->pixels = NULL;
			context->width = 0;
			context->height = 0;
			if (context->texture != NULL) {
				gs_texture_destroy(context->texture);
			}
			context->texture = NULL;
			context->nullReset = false;
		}
		return;
	}

	if (pixels == context->pixels && width == context->width && height == context->height) {
		// No change detected in graph, no need to update texture
	}
	else if (context->texture != NULL && context->width == width && context->height == height) {
		// Only pixels data have changed, update texture without recreate
		context->pixels = pixels;
		gs_texture_set_image(context->texture, (const uint8_t*)pixels, width * 4, false);
	}
	else {
		// Change detected in graph, recreate texture
		context->pixels = pixels;
		context->width = width;
		context->height = height;
		if (context->texture != NULL) {
			gs_texture_destroy(context->texture);
		}
		obs_enter_graphics();
		const uint8_t* dataPtr = (const uint8_t*)pixels;
		context->texture = gs_texture_create(width, height, GS_BGRA, 1, &dataPtr, GS_DYNAMIC);
		obs_leave_graphics();
	}

	obs_blueprint_source_draw(context->texture, effect);
	context->nullReset = true;
}

static void obs_blueprint_source_tick(void* data, float seconds)
{
	struct blueprint_source* context = data;
	c_blueprint_graph_tick(context->graph, seconds);
}

struct obs_source_info obs_blueprint_source = {
    .id = "obs_blueprint_source",
    .type = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO |
    		OBS_SOURCE_DO_NOT_DUPLICATE | OBS_SOURCE_SRGB,
    .get_name = obs_blueprint_source_name,
    .get_width = obs_blueprint_source_width,
    .get_height = obs_blueprint_source_height,
	.get_properties = obs_blueprint_source_properties,
    .create = obs_blueprint_source_create,
    .destroy = obs_blueprint_source_destroy,
    .video_render = obs_blueprint_source_render,
    .video_tick = obs_blueprint_source_tick
};

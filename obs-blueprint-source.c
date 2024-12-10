#include <obs-module.h>
#include "Core/obs-blueprint-c-interface.h"


static const char *obs_blueprint_source_name(void* unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("SourceName");
}

static uint32_t obs_blueprint_source_width(void* data)
{
	CPP_BlueprintGraphPtr graph = data;
	return c_blueprint_graph_get_width(graph);
}

static uint32_t obs_blueprint_source_height(void* data)
{
	CPP_BlueprintGraphPtr graph = data;
	return c_blueprint_graph_get_height(graph);
}

static obs_properties_t* obs_blueprint_source_properties(void* data)
{
	CPP_BlueprintGraphPtr graph = data;
	c_blueprint_graph_properties_click(graph);

    return NULL;
}

static void* obs_blueprint_source_create(obs_data_t* settings, obs_source_t* source)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(source);

	return c_blueprint_graph_create();
}

static void obs_blueprint_source_destroy(void* data)
{
	CPP_BlueprintGraphPtr graph = data;
	c_blueprint_graph_destroy(graph);
}

static void obs_blueprint_source_render(void* data, gs_effect_t* effect)
{
    //UNUSED_PARAMETER(unused);

	CPP_BlueprintGraphPtr graph = data;

	uint32_t width = c_blueprint_graph_get_width(graph);
	uint32_t height = c_blueprint_graph_get_height(graph);
	struct pixel* pixels = c_blueprint_graph_get_pixels(graph);

	if(width != 0 && height != 0 && pixels != NULL) {
		const uint8_t* obs_blueprint_graph_data = (const uint8_t*)pixels;
		obs_enter_graphics();
		gs_texture_t* texture = gs_texture_create(width, height, GS_BGRA, 1, &obs_blueprint_graph_data, 0);
		obs_leave_graphics();

		const bool previous = gs_framebuffer_srgb_enabled();
		gs_enable_framebuffer_srgb(true);

		gs_blend_state_push();
		gs_blend_function(GS_BLEND_ONE, GS_BLEND_INVSRCALPHA);
		gs_eparam_t *const param = gs_effect_get_param_by_name(effect, "image");
		gs_effect_set_texture_srgb(param, texture);
		gs_draw_sprite(texture, 0, 0, 0);
		gs_blend_state_pop();
		gs_enable_framebuffer_srgb(previous);

		// const char *tech_name = "Draw";
		// float multiplier = 1.f;
		// const bool compatibility = true; // TODO SET COMPATIBILITY ACCORDING TO ??
		// bool linear_sample = compatibility;
		// // switch (gs_get_color_space()) {
		// // case GS_CS_SRGB_16F:
		// // case GS_CS_709_EXTENDED:
		// // 	if (!linear_sample)
		// // 		tech_name = "DrawSrgbDecompress";
		// // 	break;
		// // case GS_CS_709_SCRGB:
		// // 	if (linear_sample)
		// // 		tech_name = "DrawMultiply";
		// // 	else
		// // 		tech_name = "DrawSrgbDecompressMultiply";
		// // 	multiplier = obs_get_video_sdr_white_level() / 80.f;
		// // }
		// gs_effect_t *effect = obs_get_base_effect(OBS_EFFECT_OPAQUE);
		// gs_technique_t *tech =
		// 	gs_effect_get_technique(effect, tech_name);
		// gs_eparam_t *image =
		// 	gs_effect_get_param_by_name(effect, "image");
		//
		// const bool previous = gs_framebuffer_srgb_enabled();
		// gs_enable_framebuffer_srgb(linear_sample);
		// gs_enable_blending(false);
		//
		// if (linear_sample)
		// 	gs_effect_set_texture_srgb(image, texture);
		// else
		// 	gs_effect_set_texture(image, texture);
		//
		// gs_eparam_t *multiplier_param =
		// 	gs_effect_get_param_by_name(effect, "multiplier");
		// gs_effect_set_float(multiplier_param, multiplier);
		//
		// const uint32_t flip = compatibility ? GS_FLIP_V : 0;
		// const size_t passes = gs_technique_begin(tech);
		// for (size_t i = 0; i < passes; i++) {
		// 	if (gs_technique_begin_pass(tech, i)) {
		// 		gs_draw_sprite(texture, 0, 0, 0);
		//
		// 		gs_technique_end_pass(tech);
		// 	}
		// }
		// gs_technique_end(tech);
		//
		// gs_enable_blending(true);
		// gs_enable_framebuffer_srgb(previous);

		gs_texture_destroy(texture);
	}
}

static void obs_blueprint_source_tick(void* data, float seconds)
{
	CPP_BlueprintGraphPtr graph = data;
	c_blueprint_graph_tick(graph, seconds);
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

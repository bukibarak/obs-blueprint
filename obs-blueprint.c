#include <obs-module.h>

/* Defines common functions (required) */
OBS_DECLARE_MODULE()

/* Implements common ini-based locale (optional) */
OBS_MODULE_USE_DEFAULT_LOCALE("obs-blueprint", "en-US")

extern struct obs_source_info  obs_blueprint_source;  /* Defined in obs-blueprint-source.c  */

bool obs_module_load(void)
{
	obs_register_source(&obs_blueprint_source);
	return true;
}

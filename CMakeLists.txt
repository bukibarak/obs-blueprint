
project(obs-blueprint)

set(obs-blueprint_SOURCES
      obs-blueprint.c
      obs-blueprint-source.c)

add_library(obs-blueprint MODULE
      ${obs-blueprint_SOURCES})
target_link_libraries(obs-blueprint
      libobs)

# install_obs_plugin_with_data(obs-blueprint data)
set_target_properties_obs(obs-blueprint PROPERTIES FOLDER plugins PREFIX "")

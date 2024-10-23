
project(obs-blueprint)

# Find Qt
find_package(Qt6 REQUIRED Core Widgets)

file(GLOB_RECURSE obs-blueprint_SRC
  "*.c",
  "*.h",
  "*.cpp"
)

set(obs-blueprint_SOURCES ${obs-blueprint_SRC})

add_library(obs-blueprint MODULE
      ${obs-blueprint_SOURCES})
target_link_libraries(
      obs-blueprint
      OBS::libobs
      OBS::frontend-api
      Qt::Core
      Qt::Widgets)

# install_obs_plugin_with_data(obs-blueprint data)
set_target_properties_obs(obs-blueprint PROPERTIES FOLDER plugins PREFIX "")

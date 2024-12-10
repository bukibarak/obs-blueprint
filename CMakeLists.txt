# cmake_minimum_required(VERSION 3.28...3.30)
cmake_minimum_required(VERSION 3.26)

project(obs-blueprint)

# Find OpenCV
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find Qt
find_package(Qt6 REQUIRED Core Widgets)

# Find all c/cpp files and add them to library
file(GLOB_RECURSE obs-blueprint_SOURCES
  "*.c",
  "*.h",
  "*.cpp"
)
add_library(obs-blueprint MODULE ${obs-blueprint_SOURCES})

# set(obs-blueprint_SOURCES ${obs-blueprint_SRC})

# link dependencies libraries with obs-blueprint plugin
target_link_libraries(obs-blueprint PRIVATE ${OpenCV_LIBS})
target_link_libraries(
      obs-blueprint
      PRIVATE OBS::libobs
              OBS::frontend-api
              Qt::Core
              Qt::Widgets)

# Copy OpenCV dll
MACRO (COPY_DLL libname)
    configure_file("${OpenCV_DIR}/x64/vc17/bin/${libname}.dll" "${CMAKE_BINARY_DIR}/rundir/Debug/obs-plugins/64bit/${libname}.dll" COPYONLY)
ENDMACRO (COPY_DLL)
COPY_DLL(opencv_world4100)
COPY_DLL(opencv_world4100d)

# install_obs_plugin_with_data(obs-blueprint data)
set_target_properties_obs(obs-blueprint PROPERTIES FOLDER plugins PREFIX "")

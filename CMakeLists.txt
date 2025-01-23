# cmake_minimum_required(VERSION 3.28...3.30)
cmake_minimum_required(VERSION 3.26)

project(obs-blueprint)

# Find OpenCV
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find Qt
find_package(Qt6 REQUIRED Core Widgets)

# Find all c++ files within project directories
file(GLOB_RECURSE obs-blueprint_SOURCES
  LIST_DIRECTORIES false
  "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/GUI/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/GUI/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Helpers/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Helpers/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Nodes/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Nodes/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Structs/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Structs/*.cpp"
)

# Append c files to variable
list(APPEND obs-blueprint_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-err2str.h")
list(APPEND obs-blueprint_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-err2str.c")
list(APPEND obs-blueprint_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/obs-blueprint.c")
list(APPEND obs-blueprint_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/obs-blueprint-source.c")



add_library(obs-blueprint MODULE ${obs-blueprint_SOURCES})

# link dependencies libraries with obs-blueprint plugin
target_link_libraries(obs-blueprint PRIVATE ${OpenCV_LIBS})
target_link_libraries(
      obs-blueprint
      PRIVATE OBS::libobs
              OBS::frontend-api
              OBS::libdshowcapture
              OBS::media-playback
              Qt::Core
              Qt::Widgets
              strmiids # required by libdshowcapture
              winmm # required by libdshowcapture
              FFmpeg::swscale) #required by media-playback

# Post-build macro command to help copy DLL depending of build type (Debug, Release) --> Mainly from ChatGPT
MACRO(COPY_DLL libname platform)
    # Si le paramètre debug_only est défini, on ne copie le fichier que pour la configuration Debug
    if("${platform}" STREQUAL "DEBUG")
        add_custom_command(TARGET obs-blueprint POST_BUILD
            # Ne copier que si la configuration est Debug ou RelWithDebInfo
            COMMAND ${CMAKE_COMMAND} -E $<IF:$<CONFIG:Debug,RelWithDebInfo>,copy_if_different,true>
            "${OpenCV_DIR}/x64/vc17/bin/${libname}.dll"
            "${CMAKE_BINARY_DIR}/rundir/$<CONFIG>/obs-plugins/64bit/${libname}.dll"
            COMMENT $<IF:$<CONFIG:Debug,RelWithDebInfo>,"Copy ${libname}.dll into ${CMAKE_BINARY_DIR}/rundir/$<CONFIG>/obs-plugins/64bit/","">
        )
    else()
        # Pour tous les autres fichiers, on les copie normalement (toutes configurations)
        add_custom_command(TARGET obs-blueprint POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OpenCV_DIR}/x64/vc17/bin/${libname}.dll"
            "${CMAKE_BINARY_DIR}/rundir/$<CONFIG>/obs-plugins/64bit/${libname}.dll"
            COMMENT "Copy ${libname}.dll into ${CMAKE_BINARY_DIR}/rundir/$<CONFIG>/obs-plugins/64bit/"
        )
    endif()
ENDMACRO(COPY_DLL)

# copy OpenCV DLLs based on OpenCV version (4100 for 4.10.0, 4110 for 4.11.0, 4120, ...)
string(REPLACE "." "" OpenCV_DLL_VERSION ${OpenCV_VERSION})
COPY_DLL("opencv_world${OpenCV_DLL_VERSION}" ALL)
COPY_DLL("opencv_world${OpenCV_DLL_VERSION}d" DEBUG)

# install_obs_plugin_with_data(obs-blueprint data)
set_target_properties_obs(obs-blueprint PROPERTIES FOLDER plugins PREFIX "")

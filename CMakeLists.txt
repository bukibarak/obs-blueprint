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

#if(NOT TARGET OBS::libdshowcapture)
#  add_subdirectory("${CMAKE_SOURCE_DIR}/deps/libdshowcapture" libdshowcapture)
#endif()

#if(NOT TARGET OBS::winhandle)
#  add_subdirectory("${CMAKE_SOURCE_DIR}/libobs" "${CMAKE_BINARY_DIR}/libobs")
#endif()

# set(obs-blueprint_SOURCES ${obs-blueprint_SRC})

# link dependencies libraries with obs-blueprint plugin
target_link_libraries(obs-blueprint PRIVATE ${OpenCV_LIBS})
target_link_libraries(
      obs-blueprint
      PRIVATE OBS::libobs
              OBS::frontend-api
              OBS::libdshowcapture
              Qt::Core
              Qt::Widgets
              strmiids # required by libdshowcapture
              winmm) # required by libdshowcapture

# Copy OpenCV dll
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
COPY_DLL(opencv_world4110 ALL)
COPY_DLL(opencv_world4110d DEBUG)

# install_obs_plugin_with_data(obs-blueprint data)
set_target_properties_obs(obs-blueprint PROPERTIES FOLDER plugins PREFIX "")

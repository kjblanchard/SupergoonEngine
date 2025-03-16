# Variables
set(LIBRARY_TARGET_NAME SupergoonEngine)
set(COMPILE_OPTIONS)
set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE STRING "Use html" FORCE)
set(EXTERNAL_LIBS
    SDL3::SDL3-static
)

if(UNIX OR APPLE)
    list(APPEND COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wpedantic
        -Wdeprecated
        -Wno-deprecated-redundant-constexpr-static-def
        -Wno-deprecated-literal-operator
        -Wno-macro-redefined
    )
endif(UNIX OR APPLE)

message(STATUS "Starting ${LIBRARY_TARGET_NAME} library initialization")
cmake_minimum_required(VERSION 3.22)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libraries as static" FORCE)

set(SRC_FILES
    src/engine.c
    src/lua.c
    src/easing.c
    src/log.c
    src/clock.c
    src/input/joystick.c
    src/input/keyboard.c
    src/window.c
    src/graphics.c
    src/filesystem.c
    src/events.c
    src/Audio/Bgm.c
    src/Audio/Sfx.c
    src/Audio/Stream.c
    src/Audio/Audio.c
)

if(imgui)
    message(STATUS "ImGui is enabled, adding ImGui-related files.")
    list(APPEND SRC_FILES
        src/Imgui/imgui_impl_sdl3.cpp
        src/Imgui/imgui_impl_sdlrenderer3.cpp
        src/Imgui/imgui_demo.cpp
        src/Imgui/imgui_tables.cpp
        src/Imgui/imgui_widgets.cpp
        src/Imgui/imgui_draw.cpp
        src/Imgui/imgui.cpp
        src/Debug/ImGui.cpp
        src/Debug/Debug.cpp # For showing all the windows
        src/Debug/Console.cpp
        src/Debug/Audio.cpp

        # src/Debug/Audio.cpp
    )
endif()

add_library(${LIBRARY_TARGET_NAME} STATIC
    ${SRC_FILES}
)
set(SKIP_INSTALL_ALL ON CACHE BOOL "We don't want any of these external packages to install anything")
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING " Don't use the d for debug packages" FORCE)
set(CMAKE_DEBUG_TARGET_PROPERTIES "DEBUG_OUTPUT" "" CACHE STRING "Don't use the d for debug packages" FORCE)

target_precompile_headers(${LIBRARY_TARGET_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/Supergoon/pch.h>"
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/Supergoon/pch.hpp>")

if(SYSTEM_PACKAGES) # If we are using system packages, then we should try to find them.
    find_package(SDL3 CONFIG COMPONENTS SDL3-shared)
    find_library(LIB_VORBIS
        PATH_SUFFIXES .a .dll
        NAMES libvorbis vorbis vorbis.a vorbis.dll libvorbis.a
        HINTS /usr/local/lib /c/cmake/lib)

    if(LIB_VORBIS)
        list(APPEND EXTERNAL_LIBS ${LIB_VORBIS})
    endif(LIB_VORBIS)

    find_library(LIB_VORBISFILE
        PATH_SUFFIXES .a .dll
        NAMES libvorbisfile vorbisfile vorbisfile.a vorbisfile.dll
        HINTS /usr/local/lib)

    if(LIB_VORBISFILE)
        list(APPEND EXTERNAL_LIBS ${LIB_VORBISFILE})
    endif(LIB_VORBISFILE)

    find_library(LIB_OGG
        PATH_SUFFIXES .a .dll
        NAMES ogg Ogg
        HINTS /usr/local/lib)

    if(LIB_OGG)
        list(APPEND EXTERNAL_LIBS ${LIB_OGG})
    endif(LIB_OGG)

    find_library(LIB_LUA
        PATH_SUFFIXES .a .dll
        NAMES lua_static lua
        HINTS /usr/local/bin /usr/local/lib)

    if(LIB_LUA)
        list(APPEND EXTERNAL_LIBS ${LIB_LUA})
    endif(LIB_LUA)
endif(SYSTEM_PACKAGES)

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Fetching SDL3...")
    set(SDL_SHARED OFF CACHE BOOL "Build SDL as a shared library")
    set(SDL_STATIC ON CACHE BOOL "Build SDL as a shared library")
    set(SDL_TEST OFF CACHE BOOL "Build SDL as a shared library")
    set(ENABLE_SDL_STATIC OFF CACHE BOOL "Build SDL as a static library")
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.2.0
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(SDL3)
endif(NOT SDL3_FOUND)

if(NOT LIB_OGG)
    message(STATUS "OGG not found. Fetching OGG...")
    FetchContent_Declare(
        LIB_OGG
        GIT_REPOSITORY https://github.com/xiph/ogg
        GIT_TAG v1.3.5
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(LIB_OGG)
    get_target_property(OGG_INCLUDE_DIR ogg INTERFACE_INCLUDE_DIRECTORIES)
    set(OGG_LIBRARY ogg)
    list(APPEND EXTERNAL_LIBS ogg)
endif(NOT LIB_OGG)

if(NOT LIB_VORBIS)
    message(STATUS "VORBIS not found. Fetching VORBIS...")
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Dont do it" FORCE)
    FetchContent_Declare(
        LIB_VORBIS
        GIT_REPOSITORY https://github.com/xiph/vorbis
        GIT_TAG v1.3.7
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(LIB_VORBIS)
    list(APPEND EXTERNAL_LIBS vorbisenc vorbisfile vorbis)
endif(NOT LIB_VORBIS)

# if(NOT LIB_LUA)
# message(STATUS "Lua not found. Fetching Lua...")
# FetchContent_Declare(
# LIB_LUA
# GIT_REPOSITORY https://github.com/walterschell/Lua.git
# GIT_TAG v5.3.3
# EXCLUDE_FROM_ALL
# )
# FetchContent_MakeAvailable(LIB_LUA)
# list(APPEND EXTERNAL_LIBS lua_static)
# endif(NOT LIB_LUA)
if(DEBUG_CMAKE_VARIABLES)
    get_cmake_property(ALL_VARIABLES VARIABLES)

    foreach(VAR ${ALL_VARIABLES})
        message(STATUS "${VAR} = ${${VAR}}")
    endforeach()
endif(DEBUG_CMAKE_VARIABLES)

target_compile_options(${LIBRARY_TARGET_NAME} PUBLIC ${COMPILE_OPTIONS})
target_link_directories(${LIBRARY_TARGET_NAME} PUBLIC /usr/local/lib) # should always include from usr/local/lib

target_link_libraries(${LIBRARY_TARGET_NAME}
    INTERFACE
    ${EXTERNAL_LIBS}
)
target_include_directories(${LIBRARY_TARGET_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include

    /usr/local/include
    PRIVATE
    ${sdl3_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

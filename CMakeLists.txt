# Variables
set(LIBRARY_TARGET_NAME SupergoonEngine)
set(COMPILE_OPTIONS)
set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE STRING "Use html" FORCE)
set(EXTERNAL_LIBS
    SDL3::SDL3-static
)
set(FOLDERS_TO_COPY scripting/)

if(UNIX OR APPLE)
    list(APPEND COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wpedantic
        -Wdeprecated

        # -Wno-deprecated-redundant-constexpr-static-def
        # -Wno-deprecated-literal-operator
        # -Wno-macro-redefined
        # -Wno-conversion
        # Disable conversion if you want to check all the warnings, etc
        -Wno-unused-parameter
        -Wno-sign-compare
    )
endif(UNIX OR APPLE)

message(STATUS "Starting ${LIBRARY_TARGET_NAME} library initialization")
cmake_minimum_required(VERSION 3.22)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libraries as static" FORCE)

set(SRC_FILES
    src/camera.c
    src/lua.c
    src/log.c
    src/clock.c
    src/state.c
    src/map.c
    src/effects.c
    src/gameobject.c
    src/sprite.c
    src/Input/joystick.c
    src/Input/keyboard.c
    src/Input/mouse.c
    src/Input/touch.c
    src/window.c
    src/graphics.c
    src/filesystem.c
    src/tools.c
    src/events.c
    src/engine.c
    src/Primitives/rectangle.c
    src/Audio/Bgm.c
    src/Audio/Sfx.c
    src/Audio/Stream.c
    src/Audio/Audio.c
    src/Animation/Animator.c
    src/Tweening/easing.c
    src/Tweening/tween.c
    src/UI/ui.c
    src/UI/uirect.c
    src/UI/uiobject.c
    src/UI/uiimage.c
    src/UI/uitext.c
    src/UI/uibutton.c
    src/Lua/lui.c
    src/Lua/lscene.c
    src/Lua/log.c
    src/Lua/audio.c
    src/Lua/object.c
    src/Lua/lengine.c
    src/Lua/leffects.c
    src/Lua/linput.c
    src/Lua/lsprite.c
    src/Lua/lanimation.c
    src/Lua/lcamera.c
    src/Lua/scripting.c
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
        src/Debug/Console.cpp
        src/Debug/Audio.cpp
        src/Debug/Graphics.cpp
        src/Debug/Tween.cpp
        src/Debug/Game.cpp
        src/Debug/UI.cpp
        src/Debug/Scene.cpp
        src/Debug/Performance.cpp
        src/Debug/Collision.cpp
        src/Debug/GameObject.cpp
        src/Debug/Debug.cpp # For showing all the windows

        # src/Debug/Audio.cpp
    )
endif()

add_library(${LIBRARY_TARGET_NAME} STATIC
    ${SRC_FILES}
)


set(SKIP_INSTALL_ALL ON CACHE BOOL "We don't want any of these external packages to install anything")
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING " Don't use the d for debug packages" FORCE)
set(CMAKE_DEBUG_TARGET_PROPERTIES "DEBUG_OUTPUT" "" CACHE STRING "Don't use the d for debug packages" FORCE)

# PCH settings.. to compile with xcode, you likely nee dset_source_files and just the CXX one, otherwise use both.

# set_source_files_properties(
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/Supergoon/pch.hpp
#     PROPERTIES
#     LANGUAGE CXX
# )
    # target_precompile_headers(${LIBRARY_TARGET_NAME} PRIVATE
    #     "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/Supergoon/pch.h>"
    #     "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/Supergoon/pch.hpp>")


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

    find_package(Freetype)

    if(FREETYPE_FOUND)
        list(APPEND EXTERNAL_LIBS ${FREETYPE_LIBRARY})
        target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC /usr/local/include/freetype2 /opt/homebrew/opt/freetype/include/freetype2 )
    endif(FREETYPE_FOUND)
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
        GIT_TAG release-3.2.16
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(SDL3)
endif(NOT SDL3_FOUND)

if(NOT LIB_OGG)
    message(STATUS "OGG not found. Fetching OGG...")
    set(BUILD_TESTING OFF CACHE BOOL "do not build tests")
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

if(NOT LIB_LUA)
    message(STATUS "Lua not found. Fetching Lua...")
    set(LUA_ENABLE_TESTING OFF CACHE BOOL "Build SDL as a shared library")
    set(LUA_ENABLE_SHARED OFF CACHE BOOL "Build SDL as a shared library")
    set(LUA_BUILD_COMPILER OFF CACHE BOOL "Build SDL as a shared library")
    set(LUA_BUILD_BINARY OFF CACHE BOOL "Build SDL as a shared library")

    if(NOT FREETYPE_FOUND)
        message(STATUS "freetype not found. Fetching freetype...")
        set(FT_DISABLE_BROTLI ON CACHE BOOL "Do not install when installing" FORCE)
        set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Do not install when installing" FORCE)
        set(FT_DISABLE_ZLIB ON CACHE BOOL "Do not use" FORCE)
        set(FT_DISABLE_BZIP2 ON CACHE BOOL "Do not use" FORCE)
        set(FT_DISABLE_PNG ON CACHE BOOL "Do not use" FORCE)
        set(FT_DISABLE_PNG ON CACHE BOOL "Do not use" FORCE)
        FetchContent_Declare(
            LIB_FREETYPE
            GIT_REPOSITORY https://github.com/freetype/freetype
            GIT_TAG VER-2-13-3
            EXCLUDE_FROM_ALL
        )
        FetchContent_MakeAvailable(LIB_FREETYPE)
        list(APPEND EXTERNAL_LIBS freetype)
    endif(NOT FREETYPE_FOUND)

    if(IOS) # Test for ios sim with lua, as this fails with system and needs this define.. TODO not sure if needed
        add_definitions(-DTARGET_OS_IOS=1)
        add_definitions(-DLUA_USE_IOS=1)
    endif(IOS)

    FetchContent_Declare(
        LIB_LUA
        GIT_REPOSITORY https://github.com/walterschell/Lua.git
        GIT_TAG v5.4.5
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(LIB_LUA)

    # list(APPEND EXTERNAL_LIBS lua_static)
    list(APPEND EXTERNAL_LIBS lua_static)
endif(NOT LIB_LUA)

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

# System is for blocking out the warnings, these are mostly external libs
target_include_directories(${LIBRARY_TARGET_NAME}
    SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    /usr/local/include
    PRIVATE
    # linux builds
    ${sdl3_SOURCE_DIR}/include
    ${freetype_SOURCE_DIR}/include
    ${lib_lua_SOURCE_DIR}/lua-5.4.5/include
    ${lib_vorbis_SOURCE_DIR}/include
    ${lib_ogg_SOURCE_DIR}/include
    ${lib_ogg_BINARY_DIR}/include
)

# don't hide warnings
target_include_directories(${LIBRARY_TARGET_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(DIRECTORY ${FOLDERS_TO_COPY}
    DESTINATION ${INSTALL_DIR}/assets/lua
    COMPONENT assets)


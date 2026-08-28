set(LIBRARY_TARGET_NAME SupergoonEngine)
message(STATUS "Starting ${LIBRARY_TARGET_NAME} engine initialization")
set(COMPILE_OPTIONS)
# Should we show everything, or just a few.
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "Build libraries as static" FORCE)

# Build system Specific
if(CMAKE_GENERATOR STREQUAL "Xcode")
    message(STATUS "Setting Build to Universal Binary")
    set(CMAKE_OSX_ARCHITECTURES
        "x86_64;arm64"
        CACHE STRING "" FORCE)
endif(CMAKE_GENERATOR STREQUAL "Xcode")

if(DEBUG_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT (APPLE AND (CMAKE_SYSTEM_NAME STREQUAL "iOS" OR CMAKE_SYSTEM_NAME STREQUAL "iossimulator")) AND NOT (CMAKE_SYSTEM_NAME STREQUAL "Emscripten") AND NOT ANDROID)
        message(STATUS "Enabling AddressSanitizer in Debug mode")
        set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ASAN_FLAGS}")
    endif()
endif()


# Compile definitions based on backend
if(SDL_BACKEND)
    add_definitions(-Dsdlbackend)
endif(SDL_BACKEND)
if(SDL_GL)
    add_definitions(-Dsdlglbackend)
endif(SDL_GL)
if(ANDROID)
    add_definitions(-DUSE_GLES)
endif()
if(SDL_RENDERER)
    add_definitions(-Dsdlrendererbackend)
endif(SDL_RENDERER)
if(STEAM_ENABLED)
    add_definitions(-Dsteamed)
endif(STEAM_ENABLED)

if(UNIX OR APPLE)
    list(APPEND COMPILE_OPTIONS 
        -Wall 
        -Wextra 
        -Wdeprecated 
        -Wno-unused-parameter 
        -Wno-sign-compare 
        -Wshadow
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
    )
endif(UNIX OR APPLE)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT APPLE)
    set(PREFER_STATIC_DEPS ON)
else()
    set(PREFER_STATIC_DEPS OFF)
endif()

# Prefer static libraries on release builds
if(PREFER_STATIC_DEPS)
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
    elseif(APPLE)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a .dylib)
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
    endif()
endif(PREFER_STATIC_DEPS)

set(SRC_FILES
    src/camera.c
    src/json.c
    src/state.c
    src/Input/keyboard.c
    src/window.c
    src/filesystem.c
    src/events.c
    src/engine.c
    src/Primitives/rectangle.c
    src/Audio/Audio.c
    src/Tweening/easing.c
    src/Animation/Animator.c
    src/Animation/AnimationData.c
    src/sprite.c
    src/Input/mouse.c
    src/map.c
    #SDL Platform is required
    src/Platform/sdl/sdl.c
    src/Platform/sdl/sdlInput.c
    src/Platform/sdl/sdlWindow.c
    # 2d engine
    src/Graphics/graphics.c
    src/Graphics/texture.c
    src/Graphics/shader.c
    # input
    src/Input/joystick.c
    src/text.c
    #Services - Steam - Game Center, etc
    src/services.c

    #Networking
    # src/Networking/client.c
)

if(SDL_GL)
    list(APPEND SRC_FILES src/Platform/opengl/openglGraphics.c src/Platform/opengl/openglShader.c src/Platform/opengl/openglTexture.c)
    if(NOT ANDROID)
        list(APPEND SRC_FILES src/Platform/opengl/glad.c)
    endif()
endif(SDL_GL)

if(SDL_RENDERER)
    list(APPEND SRC_FILES src/Platform/opengl/sdlGraphics.c src/Platform/opengl/sdlShader.c src/Platform/opengl/sdlTexture.c)
endif(SDL_RENDERER)

if(STEAM_ENABLED)
    list(APPEND SRC_FILES src/Platform/steam/steam.cpp)
else()
    list(APPEND SRC_FILES src/Platform/null/null_service.c)
endif(STEAM_ENABLED)

add_library(${LIBRARY_TARGET_NAME} ${SRC_FILES} ${DEBUG_FILES})

set(SKIP_INSTALL_ALL ON CACHE BOOL "We don't want any of these external packages to install anything")
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING " Don't use the d for debug packages" FORCE)
set(CMAKE_DEBUG_TARGET_PROPERTIES "DEBUG_OUTPUT" "" CACHE STRING "Don't use the d for debug packages" FORCE)


if(SYSTEM_PACKAGES) # If we are using system packages, then we should try to find them.
    find_library(LIB_OPENAL openal)
    if(LIB_OPENAL)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_OPENAL})
    endif(LIB_OPENAL)

    find_library(LIB_CGLM cglm
        # PATH_SUFFIXES .a .dll
        # NAMES libcglm cglm libcglm.a cglm.dll libcglm.a
        # HINTS /usr/local/lib /c/cmake/lib
    )
    if(LIB_CGLM)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_CGLM})
    endif(LIB_CGLM)

    find_library(LIB_VORBIS vorbis
        # PATH_SUFFIXES .a .dll
        # NAMES libvorbis vorbis vorbis.a vorbis.dll libvorbis.a
        # HINTS /usr/local/lib /c/cmake/lib
    )
    if(LIB_VORBIS)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_VORBIS})
    endif(LIB_VORBIS)

    find_library( LIB_VORBISFILE vorbisfile
        # PATH_SUFFIXES .a .dll
        # NAMES libvorbisfile vorbisfile vorbisfile.a vorbisfile.dll
        # HINTS /usr/local/lib
    )
    if(LIB_VORBISFILE)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_VORBISFILE})
    endif(LIB_VORBISFILE)

    find_library(LIB_OGG ogg
        # PATH_SUFFIXES .a .dll
        # NAMES ogg Ogg
        # HINTS /usr/local/lib
    )
    if(LIB_OGG)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_OGG})
    endif(LIB_OGG)

    find_library(LIB_SGTOOLS sgtools
        # NAMES sgtools
        # HINTS /opt/homebrew/bin/ /usr/local/bin /usr/local/lib /usr/bin/
    )
    if(LIB_SGTOOLS)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_SGTOOLS})
    endif(LIB_SGTOOLS)

    find_library(LIB_SGFORGE sgforge
        # NAMES sgforge
        # HINTS /opt/homebrew/bin/ /usr/local/bin /usr/local/lib /usr/bin/
    )
    if(LIB_SGFORGE)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_SGFORGE})
    endif(LIB_SGFORGE)

    find_library(LIB_SGSOUND sgsound
        # NAMES sgsound 
        # HINTS /opt/homebrew/bin/ /usr/local/bin /usr/local/lib /usr/bin/
    )
    if(LIB_SGSOUND)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIB_SGSOUND})
    endif(LIB_SGSOUND)

    find_package(SDL3 CONFIG COMPONENTS SDL3-shared)
    if(SDL3_FOUND)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC SDL3::SDL3)
    endif(SDL3_FOUND)

    find_package(JSON-C CONFIG)
    if(JSON-C_FOUND)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC json-c::json-c)
    endif(JSON-C_FOUND)

    find_package(Freetype)
    if(FREETYPE_FOUND)
        # target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC /usr/local/include/freetype2 /opt/homebrew/opt/freetype/include/freetype2 /usr/include/freetype2)
        target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC Freetype::Freetype)
    endif(FREETYPE_FOUND)


endif(SYSTEM_PACKAGES)

# Pull packages and build, use if incase we have found it with finds in system packages
if(NOT LIB_CGLM)
    message(STATUS "CGLM not found. Fetching CGLM...")
    set(CGLM_SHARED OFF CACHE INTERNAL "No shared")
    set(CGLM_STATIC ON CACHE INTERNAL "yes")
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Dont do it" FORCE)
    FetchContent_Declare(
        LIB_CGLM
        GIT_REPOSITORY https://github.com/recp/cglm
        GIT_TAG v0.9.6
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_CGLM)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC cglm)
endif(NOT LIB_CGLM)

if(NOT LIB_SGTOOLS)
    message(STATUS "sgtools not found. Fetching sgtools...")
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Dont do it" FORCE)
    FetchContent_Declare(
        LIB_SGTOOLS
        GIT_REPOSITORY https://github.com/kjblanchard/sgtools
        GIT_TAG master
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_SGTOOLS)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC sgtools)
endif(NOT LIB_SGTOOLS)

if(NOT LIB_SGFORGE)
    message(STATUS "sgforge not found. Fetching sgforge...")
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Dont do it" FORCE)
    set(SGFORGE_INSTALL_TOOLS OFF CACHE INTERNAL "No binary")
    FetchContent_Declare(
        LIB_SGFORGE
        GIT_REPOSITORY https://github.com/kjblanchard/sgforge
        GIT_TAG master
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_SGFORGE)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC sgforge)
endif(NOT LIB_SGFORGE)

if(NOT LIB_SGSOUND)
    message(STATUS "sgsound not found. Fetching sgsound...")
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Dont do it" FORCE)
    set(BUILD_TEST OFF CACHE BOOL "Dont do it" FORCE)
    FetchContent_Declare(
        LIB_SGSOUND
        GIT_REPOSITORY https://github.com/kjblanchard/sgsound
        GIT_TAG master
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_SGSOUND)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC sgsound)
endif(NOT LIB_SGSOUND)

if(NOT JSON-C_FOUND)
    message(STATUS "jsonc not found. Fetching...")
    set(BUILD_APPS OFF CACHE BOOL "Do not build json-c apps" FORCE)
    set(DISABLE_WERROR ON CACHE BOOL "Disable json-c werror" FORCE)
    FetchContent_Declare(
        LIB_JSON-C
        GIT_REPOSITORY https://github.com/json-c/json-c
        GIT_TAG json-c-0.18-20240915
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(LIB_JSON-C)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC json-c)
    target_include_directories( ${LIBRARY_TARGET_NAME} PUBLIC ${lib_json-c_SOURCE_DIR} ${lib_json-c_SOURCE_DIR}/include ${json-c_SOURCE_DIR} ${json-c_BINARY_DIR}  )
endif(NOT JSON-C_FOUND)

if(NOT EMSCRIPTEN AND NOT LIB_OGG)
    message(STATUS "OGG not found. Fetching OGG...")
    set(BUILD_TESTING OFF CACHE BOOL "do not build tests")
    FetchContent_Declare(
        LIB_OGG
        GIT_REPOSITORY https://github.com/xiph/ogg
        GIT_TAG v1.3.5
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_OGG)
    get_target_property(OGG_INCLUDE_DIR ogg INTERFACE_INCLUDE_DIRECTORIES)
    set(OGG_LIBRARY ogg)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ogg)
endif(NOT EMSCRIPTEN AND NOT LIB_OGG)

if(NOT EMSCRIPTEN AND NOT LIB_VORBIS)
    message(STATUS "VORBIS not found. Fetching VORBIS...")
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Dont do it" FORCE)
    FetchContent_Declare(
        LIB_VORBIS
        GIT_REPOSITORY https://github.com/xiph/vorbis
        GIT_TAG v1.3.7
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_VORBIS)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC vorbisenc vorbisfile vorbis)
endif(NOT EMSCRIPTEN AND NOT LIB_VORBIS)

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Fetching SDL3...")
    set(SDL_SHARED OFF CACHE BOOL "Build SDL as a shared library")
    set(SDL_STATIC ON CACHE BOOL "Build SDL as a shared library")
    set(SDL_TEST OFF CACHE BOOL "Build SDL as a shared library")
    set(ENABLE_SDL_STATIC OFF CACHE BOOL "Build SDL as a static library")
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.4.12
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(SDL3)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC SDL3::SDL3-static)
endif(NOT SDL3_FOUND)

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
        EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(LIB_FREETYPE)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC freetype)
endif(NOT FREETYPE_FOUND)

if(NOT EMSCRIPTEN AND NOT LIB_OPENAL)
    message(STATUS "openal not found. Fetching openal...")
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Do not use")
    set(LIBTYPE "STATIC" CACHE INTERNAL "Do not use")
    set(ALSOFT_UTILS OFF CACHE INTERNAL "")
    set(ALSOFT_EXAMPLES OFF CACHE INTERNAL "")
    set(ALSOFT_BACKEND_WAVE OFF CACHE INTERNAL "")
    set(ALSOFT_EMBED_HRTF_DATA ON CACHE INTERNAL "")
    FetchContent_Declare(
        LIB_OPENAL
        GIT_REPOSITORY https://github.com/kcat/openal-soft
        GIT_TAG 1.24.3
        EXCLUDE_FROM_ALL)
    if(ANDROID)
        # Android's bionic has built-in atomics — prevent OpenAL from finding
        # and linking the host macOS libatomic (Mach-O) into the Android build.
        set(HAVE_LIBATOMIC FALSE CACHE INTERNAL "")
        FetchContent_MakeAvailable(LIB_OPENAL)
    else()
        FetchContent_MakeAvailable(LIB_OPENAL)
    endif()
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC OpenAL)
endif(NOT EMSCRIPTEN AND NOT LIB_OPENAL)

if(DEBUG_CMAKE_VARIABLES)
    get_cmake_property(ALL_VARIABLES VARIABLES)
    foreach(VAR ${ALL_VARIABLES})
        message(STATUS "${VAR} = ${${VAR}}")
    endforeach()
    # debugging
    # find_package(Freetype)
    # if(FREETYPE_FOUND)
    #     get_target_property(
    # FT_LIBS
    # Freetype::Freetype
    # INTERFACE_LINK_LIBRARIES)
    #debugging
    # message(STATUS "FT interface libs = ${FT_LIBS}")
    #         message(STATUS "Freetype library: ${FREETYPE_LIBRARY}")
    #         message(STATUS "Freetype libs: ${FREETYPE_LIBRARIES}")
    #         # target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC /usr/local/include/freetype2 /opt/homebrew/opt/freetype/include/freetype2 /usr/include/freetype2)
    #         # list(APPEND EXTERNAL_LIBS ${FREETYPE_LIBRARY})
    #         target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC Freetype::Freetype)
    #     endif(FREETYPE_FOUND)
endif(DEBUG_CMAKE_VARIABLES)

target_compile_options(${LIBRARY_TARGET_NAME} PUBLIC ${COMPILE_OPTIONS})
if(NOT ANDROID)
    target_link_directories(${LIBRARY_TARGET_NAME} PUBLIC /usr/local/lib)
endif()

if(LINK_M)
    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC m)
endif(LINK_M)
if(STEAM_ENABLED)
    set(STEAM_SDK_DIR "${CMAKE_CURRENT_LIST_DIR}/dist/redistributable_bin" CACHE PATH "Path to Steam SDK redistributable_bin directory")

    if(APPLE)
        set(STEAM_LIB_FILE
            "${STEAM_SDK_DIR}/osx/libsteam_api.dylib"
        )
        add_library(Steam::API SHARED IMPORTED GLOBAL)
        set_target_properties(Steam::API PROPERTIES
            IMPORTED_LOCATION "${STEAM_LIB_FILE}"
        )
    elseif(WIN32)
        set(STEAM_LIB_FILE
            "${STEAM_SDK_DIR}/win64/steam_api64.dll"
        )
        set(STEAM_IMPLIB
            "${STEAM_SDK_DIR}/win64/steam_api64.lib"
        )
        add_library(Steam::API SHARED IMPORTED GLOBAL)
        set_target_properties(Steam::API PROPERTIES
            IMPORTED_LOCATION "${STEAM_LIB_FILE}"
            IMPORTED_IMPLIB   "${STEAM_IMPLIB}"
        )
    else()
        set(STEAM_LIB_FILE
            "${STEAM_SDK_DIR}/linux64/libsteam_api.so"
        )
        add_library(Steam::API SHARED IMPORTED GLOBAL)
        set_target_properties(Steam::API PROPERTIES
            IMPORTED_LOCATION "${STEAM_LIB_FILE}"
        )
    endif()

    target_link_libraries(${LIBRARY_TARGET_NAME}
        PUBLIC Steam::API
    )

    set(STEAM_RUNTIME_FILES
        "${STEAM_LIB_FILE}"
        CACHE INTERNAL "Engine runtime files"
    )
endif()

    # System is for blocking out the warnings, these are mostly external libs
if(ANDROID)
    target_include_directories(
        ${LIBRARY_TARGET_NAME} SYSTEM
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
        PRIVATE
        ${lib_ogg_SOURCE_DIR}/include ${lib_ogg_BINARY_DIR}/include)
else()
    target_include_directories(
        ${LIBRARY_TARGET_NAME} SYSTEM
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include /usr/local/include
        PRIVATE
        ${lib_ogg_SOURCE_DIR}/include ${lib_ogg_BINARY_DIR}/include)
endif()

target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(
    ${LIBRARY_TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/include ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}_
    /emsdk/upstream/emscripten/cache/sysroot/include /usr/local/include/freetype2) # Add the freetype2 specific include


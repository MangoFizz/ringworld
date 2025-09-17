cmake_minimum_required(VERSION 3.12)

project("ring-world"
    VERSION 0.1.0
    LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 17)

if(NOT WIN32)
    message(FATAL_ERROR "This can only be built for 32-bit Windows. If you are cross compiling, use MinGW.")
endif()

if(NOT (CMAKE_SIZEOF_VOID_P EQUAL 4))
    math(EXPR BITNESS "${CMAKE_SIZEOF_VOID_P} * 8")
    message(FATAL_ERROR "This can only be built for 32-bit Windows. You are trying to build for ${BITNESS}-bit which is not allowed.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DDEBUG)
    message(CONFIGURE_LOG "Build type is set to Debug; debug information will be included")
endif()

# Prefer static libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

# Find dependencies
find_package(LuaRuntime REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
find_package(Git REQUIRED)
find_package(Backtrace)

set(RINGWORLD_BUILD_STATIC OFF CACHE BOOL "Build a static library instead of the standalone module")
set(RINGWORLD_BUILD_TOOL OFF CACHE BOOL "Build ringworld's tool")
set(RINGWORLD_TOOL_ENABLE_DEBUG_SHADERS OFF CACHE BOOL "Enable debug shaders in the tool")
set(RINGWORLD_ENABLE_EVENTS_BUS OFF CACHE BOOL "Enable the events bus")
set(RINGWORLD_ENABLE_ENHANCEMENTS OFF CACHE BOOL "Enable enhancements")

include("lua/tag_definitions.cmake")
include("src/hook/hooks.cmake")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/impl
)

# If we have Git, try to see if we are in a git repo
if(${GIT_FOUND})
    execute_process(
        COMMAND git rev-parse --git-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if("${GIT_DIR}" STREQUAL "")
        set(IN_GIT_REPO FALSE)
    else()
        get_filename_component(GIT_DIR "${GIT_DIR}" ABSOLUTE)
        set(IN_GIT_REPO TRUE)
    endif()
else()
    set(IN_GIT_REPO FALSE)
endif()

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h"
    COMMAND "${CMAKE_COMMAND}" "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}" "-DGIT_DIR=${GIT_DIR}" "-DOUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/src/version.h" -DPROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR} -DPROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR} -DPROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH} -DIN_GIT_REPO=${IN_GIT_REPO} -P "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cmake"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cmake" "$<$<BOOL:${IN_GIT_REPO}>:${GIT_DIR}/refs/heads>"
    COMMENT "Generating version.h"
)

# Add debug define for debug builds
if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DRINGWORLD_DEBUG)
endif()

if(RINGWORLD_ENABLE_ENHANCEMENTS)
    add_definitions(-DRINGWORLD_ENABLE_ENHANCEMENTS)
endif()

set(RINGWORLD_SOURCES
    src/impl/bitmap/bitmap.c
    src/impl/console/console.c
    src/impl/crypto/md5.c
    src/impl/crypto/xtea.c
    src/impl/cutscene/cutscene.c
    src/impl/debug/assertion.c
    src/impl/debug/debug_symbols.c
    src/impl/debug/debug_symbols.cpp
    src/impl/debug/stacktrace.c
    src/impl/debug/log.c
    src/impl/exception/exception.c
    src/impl/font/bitmap_font.c
    src/impl/font/font.c
    src/impl/font/vector_font.c
    src/impl/game/game_engine.c
    src/impl/game/game_globals.c
    src/impl/game/game_state.c
    src/impl/game/game_time.c
    src/impl/input/input.c
    src/impl/math/color.c
    src/impl/math/math.c
    src/impl/math/wave_functions.c
    src/impl/interface/first_person_weapon.c
    src/impl/interface/hud.c
    src/impl/interface/input_event.c
    src/impl/interface/interface.c
    src/impl/interface/loading_screen.c
    src/impl/interface/pgcr.c
    src/impl/interface/scoreboard.c
    src/impl/interface/ui_cursor.c
    src/impl/interface/ui_virtual_keyboard.c
    src/impl/interface/ui_widget_game_data_inputs.c
    src/impl/interface/ui_widget.c
    src/impl/main/main_globals.c
    src/impl/memory/dynamic_array.c
    src/impl/memory/dynamic_buffer.c
    src/impl/memory/memory.c
    src/impl/memory/pool.c
    src/impl/network/network_game.c
    src/impl/network/network_game_client.c
    src/impl/object/object.c
    src/impl/object/object_type.c
    src/impl/memory/table.c
    src/impl/multiplayer/multiplayer_game_text.c
    src/impl/player/player.c
    src/impl/rasterizer/rasterizer_bitmap_font.c
    src/impl/rasterizer/rasterizer.c
    src/impl/rasterizer/rasterizer_config.c
    src/impl/rasterizer/rasterizer_dx9_render_target.c
    src/impl/rasterizer/rasterizer_dx9_shader_compiler.c
    src/impl/rasterizer/rasterizer_dx9_shader_effect.c
    src/impl/rasterizer/rasterizer_dx9_texture.c
    src/impl/rasterizer/rasterizer_dx9_vertex_shader.c
    src/impl/rasterizer/rasterizer_dx9_vertex.c
    src/impl/rasterizer/rasterizer_dx9.c
    src/impl/rasterizer/rasterizer_font.c
    src/impl/rasterizer/rasterizer_geometry_group.c
    src/impl/rasterizer/rasterizer_model.c
    src/impl/rasterizer/rasterizer_motion_sensor.c
    src/impl/rasterizer/rasterizer_screen_effect.c
    src/impl/rasterizer/rasterizer_screen_geometry.c
    src/impl/rasterizer/rasterizer_shader_transparent_chicago.c
    src/impl/rasterizer/rasterizer_shader_transparent_generic.c
    src/impl/rasterizer/rasterizer_shader_transparent_water.c
    src/impl/rasterizer/rasterizer_shaders_blob.c
    src/impl/rasterizer/rasterizer_vector_font.c
    src/impl/render/render.c
    src/impl/rng/rng.c
    src/impl/saved_games/player_profile.c
    src/impl/scenario/scenario.c
    src/impl/script/hsc.c
    src/impl/shader/shader.c
    src/impl/shell/shell.c
    src/impl/tag/tag.c
    src/impl/terminal/terminal.c
    src/impl/text/string.c
    src/impl/text/text.c
    src/impl/text/unicode_string_list.c
    ${RINGWORLD_HOOKS_SOURCES}
    ${TAG_DEFINITION_HPP_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
)

if(RINGWORLD_ENABLE_EVENTS_BUS)
    add_definitions(-DRINGWORLD_ENABLE_EVENTS_BUS)
    list(APPEND RINGWORLD_SOURCES
        src/event/event.cpp
    )
endif()

if(RINGWORLD_BUILD_STATIC)
    add_library(ringworld STATIC ${RINGWORLD_SOURCES})
else()    
    add_library(ringworld MODULE ${RINGWORLD_SOURCES} src/ringworld.c)
    set_target_properties(ringworld PROPERTIES PREFIX "")
    if(NOT CMAKE_BUILD_TYPE MATCHES Release)
        set(RINGWORLD_LINK_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,--export-all-symbols")
    else()
        set(RINGWORLD_LINK_FLAGS "-static -static-libgcc -static-libstdc++ -s")
    endif()
    set_target_properties(ringworld PROPERTIES LINK_FLAGS "${RINGWORLD_LINK_FLAGS}")

    # Set minimum platform version to Windows 7
    target_compile_definitions(ringworld PRIVATE -D_WIN32_WINNT=0x0601)
endif()

target_link_libraries(ringworld d3dcompiler d3d9 d3dx9 dbghelp dwmapi)

if(Backtrace_FOUND)
    target_link_libraries(ringworld backtrace)
else()
    target_compile_definitions(ringworld PRIVATE -DRINGWORLD_NO_BACKTRACE)
    message(WARNING "Backtrace library not found. Debugging features may be limited.")
endif()

if(RINGWORLD_BUILD_TOOL)
    if(RINGWORLD_TOOL_ENABLE_DEBUG_SHADERS)
        add_definitions(-DRINGWORLD_TOOL_ENABLE_DEBUG_SHADERS)
    endif()
    add_executable(ringworld-tool
        $<TARGET_OBJECTS:ringworld>
        src/tool/d3d_shader.c
        src/tool/effect_collection_file.c
        src/tool/file.c
        src/tool/vsh_file.c
        src/tool/main.c
        src/tool/resources.rc
    )
    set_target_properties(ringworld-tool PROPERTIES LINK_FLAGS "-static -static-libgcc -static-libstdc++")
    target_link_libraries(ringworld-tool d3d9 d3dx9 d3dcompiler shlwapi dbghelp dwmapi)
    target_compile_definitions(ringworld-tool PRIVATE -D_WIN32_WINNT=0x0601)
    
    if(Backtrace_FOUND)
        target_link_libraries(ringworld-tool backtrace)
    else()
        target_compile_definitions(ringworld-tool PRIVATE -DRINGWORLD_NO_BACKTRACE)
    endif()
endif()


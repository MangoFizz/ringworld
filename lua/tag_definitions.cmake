# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.16)

# Find Lua runtime
find_package(LuaRuntime REQUIRED)

set(TAG_DEFINITIONS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ringhopper/src/ringhopper-definitions/json/tag")
file(GLOB TAG_DEFINITION_FILES ${TAG_DEFINITIONS_PATH}/*.json)

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TAG_DEFINITIONS_HPP_PATH "${SOURCE_PATH}/impl/tag/definitions")
set(TAG_DEFINITION_HPP_FILES)

# Tell CMake how to generate the tag struct headers
set(LUA_ENVIRONMENT_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/lua/env.lua)
set(LUA_COMMNAD ${CMAKE_COMMAND} -E env LUA_INIT="@${LUA_ENVIRONMENT_SCRIPT}" ${LUA_EXECUTABLE})
set(TAG_DEFINITION_HEADERS_GENERATOR_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/lua/tagHeadersGenerator.lua)

# Create output directory for tag structures and get output file paths
file(MAKE_DIRECTORY ${TAG_DEFINITIONS_HPP_PATH})
foreach(TAG_DEFINITION_FILE ${TAG_DEFINITION_FILES})
    get_filename_component(TAG_DEFINITION_NAME ${TAG_DEFINITION_FILE} NAME_WE)
    set(TAG_DEFINITION_HPP_FILES ${TAG_DEFINITION_HPP_FILES} "${TAG_DEFINITIONS_HPP_PATH}/${TAG_DEFINITION_NAME}.h")
endforeach()

add_custom_command(
    OUTPUT ${TAG_DEFINITION_HPP_FILES} 
    COMMAND ${LUA_COMMNAD} ${TAG_DEFINITION_HEADERS_GENERATOR_SCRIPT} ${SOURCE_PATH} ${TAG_DEFINITION_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating tag definitions headers..."
    DEPENDS ${TAG_DEFINITION_HEADERS_GENERATOR_SCRIPT} ${TAG_DEFINITION_FILES}
)

# Add definitions headers target, so we can add them as a dependency to Balltze
add_custom_target(ringworld-tag-definitions-headers ALL DEPENDS ${TAG_DEFINITION_HPP_FILES})

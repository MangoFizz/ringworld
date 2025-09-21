# Find Python and the jsonschema module
find_package(Python3 COMPONENTS Interpreter REQUIRED)
execute_process( 
    COMMAND ${Python3_EXECUTABLE} -m check_jsonschema --version # seems that it doesn't support executable targets
    OUTPUT_QUIET
    ERROR_QUIET
    RESULT_VARIABLE PYTHON_JSONSCHEMA_RESULT
)

set(PYTHON_JSONSCHEMA_INSTALLED true)

if(NOT PYTHON_JSONSCHEMA_RESULT EQUAL 0)
    set(PYTHON_JSONSCHEMA_INSTALLED false)
    message(FATAL_ERROR "The 'check_jsonschema' package is not installed. Skipping schema validation.")
endif()

set(FUNCTIONS_MAPPING_JSON ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/mappings/functions.json)
set(FUNCTIONS_MAPPING_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/mappings/functions.schema.json)
set(FUNCTION_HOOKS_CPP ${CMAKE_CURRENT_BINARY_DIR}/function_hooks.cpp)
set(FUNCTION_WRAPPERS_S ${CMAKE_CURRENT_BINARY_DIR}/function_wrappers.S)
set(GENERATE_HOOKS_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/scripts/generate_functions_hooks.py)

add_custom_command(
    OUTPUT ${FUNCTION_HOOKS_CPP} ${FUNCTION_WRAPPERS_S}
    DEPENDS ${FUNCTIONS_MAPPING_JSON}
    DEPENDS ${GENERATE_HOOKS_SCRIPT}
    COMMAND ${PYTHON_JSONSCHEMA_INSTALLED} && ${Python3_EXECUTABLE} -m check_jsonschema -q --schemafile ${FUNCTIONS_MAPPING_SCHEMA} ${FUNCTIONS_MAPPING_JSON}
    COMMAND ${Python3_EXECUTABLE} ${GENERATE_HOOKS_SCRIPT} ${FUNCTIONS_MAPPING_JSON} ${FUNCTION_HOOKS_CPP} ${FUNCTION_WRAPPERS_S}
)

set(VARIABLES_MAPPING_JSON ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/mappings/variables.json)
set(VARIABLES_MAPPING_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/mappings/variables.schema.json)
set(VARIABLES_CPP ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp)
set(GENERATE_VARIABLES_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/scripts/generate_variables_bindings.py)

add_custom_command(
    OUTPUT ${VARIABLES_CPP}
    DEPENDS ${VARIABLES_MAPPING_JSON}
    DEPENDS ${GENERATE_VARIABLES_SCRIPT}
    COMMAND ${PYTHON_JSONSCHEMA_INSTALLED} && ${Python3_EXECUTABLE} -m check_jsonschema --schemafile ${VARIABLES_MAPPING_SCHEMA} ${VARIABLES_MAPPING_JSON}
    COMMAND ${Python3_EXECUTABLE} ${GENERATE_VARIABLES_SCRIPT} ${VARIABLES_MAPPING_JSON} ${VARIABLES_CPP}
)

set(RINGWORLD_HOOK_SOURCES
    ${FUNCTION_HOOKS_CPP}
    ${FUNCTION_WRAPPERS_S}
    ${VARIABLES_CPP}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/patches/game_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/patches/hud.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/patches/hud.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/patches/shader_transparent_generic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/patches/shader_transparent_generic.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/hook.cpp
)

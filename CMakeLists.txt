cmake_minimum_required(VERSION 3.12)

project("ring-world"
    VERSION 0.1
    LANGUAGES C CXX ASM
)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(NOT WIN32)
    message(FATAL_ERROR "This can only be built for 32-bit Windows. If you are cross compiling, use MinGW.")
endif()

if(NOT (CMAKE_SIZEOF_VOID_P EQUAL 4))
    math(EXPR BITNESS "${CMAKE_SIZEOF_VOID_P} * 8")
    message(FATAL_ERROR "This can only be built for 32-bit Windows. You are trying to build for ${BITNESS}-bit which is not allowed.")
endif()

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/game_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/game_functions.S
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_functions.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_functions.json set_up_game_hooks ${CMAKE_CURRENT_BINARY_DIR}/game_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/game_functions.S
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dedicated_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/dedicated_functions.S
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/dedicated_functions.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/dedicated_functions.json set_up_dedicated_hooks ${CMAKE_CURRENT_BINARY_DIR}/dedicated_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/dedicated_functions.S
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_variables.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_variables.json ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dedicated_variables.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/dedicated_variables.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/dedicated_variables.json ${CMAKE_CURRENT_BINARY_DIR}/dedicated_variables.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(ringworld STATIC
    src/ringworld.cpp
    src/impl/exception/exception.c
    src/impl/math/math.c
    src/impl/input/mouse.c
    src/impl/memory/memory.c
    src/impl/memory/table.c
    src/impl/player/player.c
    src/impl/script/hsc.c
    src/impl/ui/ui_widget.c

    ${CMAKE_CURRENT_BINARY_DIR}/game_function_hooks.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/game_functions.S
    ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/dedicated_function_hooks.cpp
)

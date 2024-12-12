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
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/hook.cpp"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/hook/hook.json"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py"
    COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py" "${CMAKE_CURRENT_SOURCE_DIR}/src/hook/hook.json" "${CMAKE_CURRENT_BINARY_DIR}/hook.cpp"
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(ringworld STATIC
    src/ringworld.cpp
    src/impl/exception/exception.c
    "${CMAKE_CURRENT_BINARY_DIR}/hook.cpp"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/game_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/game_functions.S
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_functions.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_functions.json set_up_game_hooks ${CMAKE_CURRENT_BINARY_DIR}/game_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/game_functions.S "false"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/server_function_hooks.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/server_functions.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_hooks.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/server_functions.json set_up_server_hooks ${CMAKE_CURRENT_BINARY_DIR}/server_function_hooks.cpp ${CMAKE_CURRENT_BINARY_DIR}/server_functions.S "true"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_variables.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_variables.json ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp "set_game_variables" "false"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/server_variables.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/server_variables.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/generate_variables.py ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/server_variables.json ${CMAKE_CURRENT_BINARY_DIR}/server_variables.cpp "set_server_variables" "true"
)

set(RINGWORLD_HOOKS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/hook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/codefinder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/game_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/hud.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/hud.S
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/shader_transparent_generic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/shader_transparent_generic.S
    ${CMAKE_CURRENT_BINARY_DIR}/game_function_hooks.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/game_functions.S
    ${CMAKE_CURRENT_BINARY_DIR}/game_variables.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/server_function_hooks.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/server_variables.cpp
)

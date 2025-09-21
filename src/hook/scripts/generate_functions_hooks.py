import json
import sys

if len(sys.argv) != 4:
    print("Usage: {} <mappings.json> <hooks-out.cpp> <wrappers-out.S>")
    sys.exit(1)

with open(sys.argv[1], "r") as f:
    mappings = json.loads(f.read())

function_definitions = ""
game_function_hooks = ""
server_functions_hooks = ""
function_wrappers = ""

for function in mappings:
    function_info = mappings[function]
    game_function_details = function_info.get("game", {})
    server_function_details = function_info.get("server", {})
    replace_mode = function_info.get("replace")

    if function_info.get("disabled", False):
        continue

    ##########################################################
    ## Build hooks for game and server functions
    ##########################################################

    function_definitions += f"    void *{function}_function_address = nullptr;\n"
    function_definitions += f"    void *{function}_wrapper_function = nullptr;\n"

    game_addr = game_function_details.get("address")
    if game_addr is not None:
        game_function_hooks += f"        {function}_function_address = reinterpret_cast<void *>({game_addr});\n"
        match replace_mode:
            case "forbid":
                game_function_hooks += f"        Hook(\"{function}\", {game_addr}).forbid()\n"
            case "stub":
                game_function_hooks += f"        Hook(\"{function}\", {game_addr}).stub()\n"
            case True:
                function_definitions += f"    extern int {function};\n"
                game_function_hooks += f"        Hook(\"{function}\", {game_addr}, reinterpret_cast<std::uintptr_t>(&{function}))\n"
            case False:
                game_function_hooks += f"        {function}_wrapper_function = Hook(\"{function}\", {game_addr})\n"
            case _:
                print("Unknown replace mode: ", replace_mode)
                sys.exit(1)
    else:
        game_function_hooks += f"        {function}_function_address = nullptr;\n"
        game_function_hooks += f"        {function}_wrapper_function = Hook(\"{function}\", reinterpret_cast<std::uintptr_t>(unavailable_function_called))\n"

    server_addr = server_function_details.get("address")
    if server_addr is not None:
        server_functions_hooks += f"        {function}_function_address = reinterpret_cast<void *>({server_addr});\n"
        match replace_mode:
            case "forbid":
                server_functions_hooks += f"        Hook(\"{function}\", {server_addr}).forbid()\n"
            case "stub":
                server_functions_hooks += f"        Hook(\"{function}\", {server_addr}).stub()\n"
            case True:
                if game_addr is None:
                    function_definitions += f"    extern int {function};\n"
                server_functions_hooks += f"        Hook(\"{function}\", {server_addr}, reinterpret_cast<std::uintptr_t>(&{function}))\n"
            case False | None:
                server_functions_hooks += f"        {function}_wrapper_function = Hook(\"{function}\", {server_addr})\n"
    else:
        server_functions_hooks += f"        {function}_function_address = nullptr;\n"
        server_functions_hooks += f"        {function}_wrapper_function = Hook(\"{function}\", reinterpret_cast<std::uintptr_t>(unavailable_function_called))\n"

    game_args = game_function_details.get("arguments")
    if game_args:
        for arg in game_args:
            if arg.isnumeric():
                game_function_hooks += f"            .push_parameter(Stack, {arg})\n"
            else:
                game_function_hooks += f"            .push_parameter(Register, {arg.upper()})\n"

    server_args = server_function_details.get("arguments")
    if server_args:
        for arg in server_args:
            if arg.isnumeric():
                server_functions_hooks += f"            .push_parameter(Stack, {arg})\n"
            else:
                server_functions_hooks += f"            .push_parameter(Register, {arg.upper()})\n"

    if function_info.get("use_return_value"):
        game_function_hooks     += "            .has_return_value()\n"
        server_functions_hooks  += "            .has_return_value()\n"

    if function_info.get("return_64_bit_value"):
        game_function_hooks     += "            .return_64_bit_value()\n"
        server_functions_hooks  += "            .return_64_bit_value()\n"

    game_function_hooks     += "            .write_hook();\n"
    server_functions_hooks  += "            .write_hook();\n"

    ##########################################################
    ## Build wrappers for functions that are not replaced
    ##########################################################

    if replace_mode == False:
        function_wrappers += f".globl _{function}\n"
        function_wrappers += f"_{function}:\n"
        function_wrappers += f"    jmp dword ptr [_{function}_wrapper_function]\n"


##########################################################
## Write function hooks file
##########################################################

hooks_source_code = """// This file is auto-generated. DO NOT EDIT!

#include "hook/hook.hpp"

extern "C" {
    void unavailable_function_called();
""" + function_definitions + """}

namespace Ringworld {
    void set_up_game_function_mappings() {
""" + game_function_hooks + """    }

    void set_up_server_function_mappings() {
""" + server_functions_hooks + """    }
}
"""

with open(sys.argv[2], "w") as f:
    f.write(hooks_source_code)

##########################################################
## Write function wrappers file
##########################################################

wrappers_source_code = """;# This file is auto-generated. DO NOT EDIT!
.intel_syntax noprefix

.text

""" + function_wrappers

with open(sys.argv[3], "w") as f:
    f.write(wrappers_source_code)

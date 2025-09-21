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
    
    if "disabled" in function_info and function_info["disabled"]:
        continue

    if not function_info["address"]:
        print("Tried to hook function without address in game: ", function)
        sys.exit(1)

    ##########################################################
    ## Build hooks for game and server functions
    ##########################################################

    function_definitions += "    void *{}_function_address = nullptr;\n".format(function)
    function_definitions += "    void *{}_wrapper_function = nullptr;\n".format(function)
    game_function_hooks += "        {hook}_function_address = reinterpret_cast<void *>({address});\n".format(hook=function, address=function_info["address"])

    match function_info["replace"]:
        case "forbid":
            game_function_hooks += "        Hook(\"{hook}\", {address}).forbid()".format(hook=function, address=function_info["address"])
        case "stub":
            game_function_hooks += "        Hook(\"{hook}\", {address}).stub()".format(hook=function, address=function_info["address"])
        case True:
            function_definitions += "    extern int {};\n".format(function)
            game_function_hooks += "        Hook(\"{hook}\", {address}, reinterpret_cast<std::uintptr_t>(&{output}))".format(hook=function, address=function_info["address"], output=function)
        case False:
            game_function_hooks += "        {hook}_wrapper_function = Hook(\"{hook}\", {address})".format(hook=function, address=function_info["address"])
        case _:
            print("Unknown replace mode: ", function_info["replace"])
            sys.exit(1)

    if function_info["server-address"] != None:
        server_functions_hooks += "        {hook}_function_address = reinterpret_cast<void *>({address});\n".format(hook=function, address=function_info["server-address"])
        match function_info["replace"]:
            case "forbid":
                server_functions_hooks += "        Hook(\"{hook}\", {address}).forbid()".format(hook=function, address=function_info["server-address"])
            case "stub":
                server_functions_hooks += "        Hook(\"{hook}\", {address}).stub()".format(hook=function, address=function_info["server-address"])
            case True:
                server_functions_hooks += "        Hook(\"{hook}\", {address}, reinterpret_cast<std::uintptr_t>(&{output}))".format(hook=function, address=function_info["server-address"], output=function)
            case False | None:
                server_functions_hooks += "        {hook}_wrapper_function = Hook(\"{hook}\", {address})".format(hook=function, address=function_info["server-address"])
    else:
        server_functions_hooks += "        {hook}_function_address = nullptr;\n".format(hook=function)
        server_functions_hooks += "        {hook}_wrapper_function = Hook(\"{hook}\", reinterpret_cast<std::uintptr_t>(unavailable_function_called))".format(hook=function)

    if "arguments" in function_info:
        for arg in function_info["arguments"]:
            if arg.isnumeric():
                game_function_hooks     += "\n            .push_parameter(Stack, {index})".format(index=arg)
                server_functions_hooks  += "\n            .push_parameter(Stack, {index})".format(index=arg)
            else:
                game_function_hooks     += "\n            .push_parameter(Register, {index})".format(index=arg.upper())
                server_functions_hooks  += "\n            .push_parameter(Register, {index})".format(index=arg.upper())

    if "use_return_value" in function_info and function_info["use_return_value"]:
        game_function_hooks     += "\n            .has_return_value()"
        server_functions_hooks  += "\n            .has_return_value()"

    if "return_64_bit_value" in function_info and function_info["return_64_bit_value"]:
        game_function_hooks     += "\n            .return_64_bit_value()"
        server_functions_hooks  += "\n            .return_64_bit_value()"

    game_function_hooks     += "\n            .write_hook();\n"
    server_functions_hooks  += "\n            .write_hook();\n"

    ##########################################################
    ## Build wrappers for functions that are not replaced
    ##########################################################

    if function_info["replace"] == False:
        function_wrappers += ".globl _{hook}\n".format(hook=function)
        function_wrappers += "_{hook}:\n".format(hook=function)
        function_wrappers += "    jmp dword ptr [_{hook}_wrapper_function]\n".format(hook=function)


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

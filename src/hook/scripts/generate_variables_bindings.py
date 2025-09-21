import json
import sys

if len(sys.argv) != 3:
    print("Usage: {} <mappings.json> <variables-out.cpp>")
    sys.exit(1)

with open(sys.argv[1], "r") as f:
    mappings = json.loads(f.read())

variables_definitions = ""
game_variables_init = ""
server_variables_init = ""

for variable in mappings:
    variable_info = mappings[variable]

    variables_definitions += "    void *{} = nullptr;\n".format(variable)

    if "address" in variable_info and variable_info["address"] != None:
        game_variables_init += "        {} = reinterpret_cast<void *>({});\n".format(variable, variable_info["address"])

    if "server-address" in variable_info and variable_info["server-address"] != None:
        server_variables_init += "        {} = reinterpret_cast<void *>({});\n".format(variable, variable_info["server-address"])

variables_source_code = """// This file is auto-generated. DO NOT EDIT!

#include <stdint.h>
#include <stddef.h>

extern "C" {
""" + variables_definitions + """
}

namespace Ringworld {
    void set_up_game_variables() {
""" + game_variables_init + """    }

    void set_up_server_variables() {
""" + server_variables_init + """    }
}

"""

with open(sys.argv[2], "w") as f:
    f.write(variables_source_code)

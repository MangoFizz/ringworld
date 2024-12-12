import json
import sys

if len(sys.argv) != 3:
    print("Usage: {} <variables.json> <out.cpp>")
    sys.exit(1)

with open(sys.argv[1], "r") as f:
    variables = json.loads(f.read())

cpp_source_code = """// AUTO-GENERATED! DO NOT EDIT UNLESS YOU LIKE REGRETTING THINGS!

#include <stdint.h>
#include <stddef.h>

extern "C" {
"""

var_decl = ""
for variable in variables:
    var_info = variables[variable]
    var_type = var_info["type"]
    var_name = variable
    var_address = var_info["address"]
    var_decl += "   auto *{} = reinterpret_cast<{} *>({});\n".format(var_name, var_type, var_address)

cpp_source_code += var_decl + """}
"""

with open(sys.argv[2], "w") as f:
    f.write(cpp_source_code)

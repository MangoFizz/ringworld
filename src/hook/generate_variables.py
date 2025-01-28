import json
import sys

if len(sys.argv) != 5:
    print("Usage: {} <variables.json> <out.cpp> <function-name> <extern-variables>")
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
    if sys.argv[4] == "true":
        var_decl += "    extern void *{};\n".format(var_name)
    else:
        var_decl += "    void *{} = NULL;\n".format(var_name)

init_func = """
namespace Demon {
    void """ + sys.argv[3] + """() {
"""

for variable in variables:
    var_name = variable
    init_func += "        {} = reinterpret_cast<void *>({});\n".format(var_name, variables[variable]["address"])

init_func += """    }
}
"""

cpp_source_code += var_decl + """}
"""

cpp_source_code += init_func

with open(sys.argv[2], "w") as f:
    f.write(cpp_source_code)

#include <ringworld/shader/shader.h>

#include "../exception/exception.h"

void *shader_type_assert(Shader *shader, ShaderType type) {
    ASSERT(shader);
    ASSERT(shader->shader_type == type);
    return shader;
}

#include "../exception/exception.h"
#include "shader.h"

void *shader_type_assert(Shader *shader, ShaderType type) {
    ASSERT(shader);
    ASSERT(shader->shader_type == type);
    return shader;
}

BitmapType shader_bitmap_type_for_shader_transparent_generic_first_map(ShaderFirstMapType first_map_type) {
    switch(first_map_type) {
        case SHADER_FIRST_MAP_TYPE_2D_MAP:
            return BITMAP_TYPE_2D_TEXTURES;
        case SHADER_FIRST_MAP_TYPE_FIRST_MAP_IS_REFLECTION_CUBE_MAP:
        case SHADER_FIRST_MAP_TYPE_FIRST_MAP_IS_OBJECT_CENTERED_CUBE_MAP:
        case SHADER_FIRST_MAP_TYPE_FIRST_MAP_IS_VIEWER_CENTERED_CUBE_MAP:
            return BITMAP_TYPE_CUBE_MAPS;
        default:
            CRASHF_DEBUG("Unknown first map type: %d", first_map_type);
            return BITMAP_TYPE_2D_TEXTURES;
    }
}

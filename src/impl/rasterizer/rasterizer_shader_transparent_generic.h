#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_GENERIC_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_GENERIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <d3d9.h>

#include "../memory/table.h"
#include "../tag/definitions/shader_transparent_generic.h"
#include "rasterizer_geometry_group.h"

enum {
    MAX_SHADER_TRANSPARENT_GENERIC_INSTANCES = 255,
    MAX_SHADER_TRANSPARENT_GENERIC_PER_MAP = 255,
    MAX_SHADER_TRANSPARENT_GENERIC_STAGES = 7,
    MAX_SHADER_TRANSPARENT_GENERIC_MAPS = 4
};

typedef struct ShaderTransparentGenericInstance {
    uint16_t id;
    TableResourceHandle handle;
    char hash[32];
    void *compiled_shader;
    IDirect3DPixelShader9 *shader;
    Bool invalid;
} ShaderTransparentGenericInstance;
_Static_assert(sizeof(ShaderTransparentGenericInstance) == 52);

MAKE_TABLE_STRUCT(ShaderTransparentGenericInstances, ShaderTransparentGenericInstance);

typedef struct ShaderTransparentGenericTagCache {
    uint16_t id;
    TableResourceHandle shader_instance;
    ShaderTransparentGeneric *shader_data;
} ShaderTransparentGenericTagCache;
_Static_assert(sizeof(ShaderTransparentGenericTagCache) == 12);

MAKE_TABLE_STRUCT(ShaderTransparentGenericTagsCache, ShaderTransparentGenericTagCache);

/**
 * Draw the shader transparent generic geometry.
 * @param group The transparent geometry group.
 * @param param_2 Lightmap related parameter.
 */
void rasterizer_shader_transparent_generic_draw(TransparentGeometryGroup *group, uint32_t *param_2);

/**
 * Set up the pixel shader constants for rendering the shader transparent generic.
 * @param group The transparent geometry group.
 */
void rasterizer_dx9_transparent_generic_preprocess(TransparentGeometryGroup *group);

/**
 * Create an instance of the shader transparent generic for each tag in the current map.
 */
void rasterizer_shader_transparent_generic_create_instances_for_current_map(void);

/**
 * Create an instance of the shader transparent generic for each tag in the current map except try to reuse 
 * instaces from the previous map that are still valid to save shader compilation time. 
 */
void rasterizer_shader_transparent_generic_update_instances_for_current_map(void);

/**
 * Clear all shader transparent generic instances.
 */
void rasterizer_shader_transparent_generic_clear_instances(void);

#ifdef __cplusplus
}
#endif

#endif

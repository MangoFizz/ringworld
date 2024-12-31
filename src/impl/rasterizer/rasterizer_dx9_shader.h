#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_SHADER_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <d3d9.h>

#include "../memory/memory.h"
#include "rasterizer_dx9_vertex.h"

typedef struct RasterizerDx9PixelShader {
    char name[128];
    IDirect3DPixelShader9 *pixel_shader;
} RasterizerDx9PixelShader; 
_Static_assert(sizeof(RasterizerDx9PixelShader) == 0x84);

typedef struct RasterizerDx9ShaderEffect {
    char name[128]; 
    int32_t pixel_shader_count;
    RasterizerDx9PixelShader *pixel_shaders;
} RasterizerDx9ShaderEffect; 
_Static_assert(sizeof(RasterizerDx9ShaderEffect) == 0x88);

typedef struct RasterizerDx9ShaderEffectEntry {
    RasterizerDx9ShaderEffect *effect;
    char pad[4];
    int32_t vertex_shader_index;
    const char *name;
} RasterizerDx9ShaderEffectEntry;
_Static_assert(sizeof(RasterizerDx9ShaderEffectEntry) == 0x10);

typedef struct VertexShader {
    IDirect3DVertexShader9 *shader;
    const char *filepath;
} VertexShader;
_Static_assert(sizeof(VertexShader) == 0x8);

enum ShaderEffectIndex {
    SHADER_EFFECT_ENVIRONMENT_LIGHTMAP_NORMAL = 0,
    SHADER_EFFECT_ENVIRONMENT_LIGHTMAP_NO_LIGHTMAP,
    SHADER_EFFECT_ENVIRONMENT_LIGHTMAP_NO_ILLUMINATION,
    SHADER_EFFECT_ENVIRONMENT_LIGHTMAP_NO_ILLUMINATION_NO_LIGHTMAP,
    SHADER_EFFECT_ENVIRONMENT_DIFFUSE_LIGHTS,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_BIASED_MULTIPLY_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_BIASED_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_BIASED_MULTIPLY_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_MULTIPLY_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_MULTIPLY_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_BIASED_ADD_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_BIASED_ADD_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_NORMAL_BIASED_ADD_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_BIASED_MULTIPLY_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_BIASED_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_BIASED_MULTIPLY_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_MULTIPLY_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_MULTIPLY_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_BIASED_ADD_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_BIASED_ADD_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_BLENDED_BIASED_ADD_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_BIASED_MULTIPLY_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_BIASED_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_BIASED_MULTIPLY_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_MULTIPLY_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_MULTIPLY_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_BIASED_ADD_BIASED_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_BIASED_ADD_MULTIPLY,
    SHADER_EFFECT_ENVIRONMENT_TEXTURE_SPECULAR_MASK_BIASED_ADD_BIASED_ADD,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_BUMPED,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_FLAT,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_FLAT_SPECULAR,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_RADIOSITY,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_LIGHTMAP_MASK,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_MIRROR_BUMPED,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_MIRROR_FLAT,
    SHADER_EFFECT_ENVIRONMENT_REFLECTION_MIRROR_FLAT_SPECULAR,
    SHADER_EFFECT_ENVIRONMENT_SPECULAR_LIGHT_FLAT,
    SHADER_EFFECT_ENVIRONMENT_SPECULAR_LIGHT_BUMPED,
    SHADER_EFFECT_ENVIRONMENT_SPECULAR_LIGHTMAP_FLAT,
    SHADER_EFFECT_ENVIRONMENT_SPECULAR_LIGHTMAP_BUMPED,
    SHADER_EFFECT_TRANSPARENT_PLASMA,
    SHADER_EFFECT_SHADOW_CONVOLVE,
    SHADER_EFFECT_ENVIRONMENT_SHADOW,
    SHADER_EFFECT_WIDGET_SPRITE,
    SHADER_EFFECT_SCREEN_NORMAL,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_ADD_ADD,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_ADD_DOT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_ADD_MULTIPLY,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_ADD_MULTIPLY2X,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_ADD_SUBTRACT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_DOT_ADD,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_DOT_DOT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_DOT_MULTIPLY,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_DOT_MULTIPLY2X,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_DOT_SUBTRACT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY_ADD,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY_DOT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY_MULTIPLY,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY_MULTIPLY2X,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY_SUBTRACT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY2X_ADD,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY2X_DOT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY2X_MULTIPLY,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY2X_MULTIPLY2X,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_MULTIPLY2X_SUBTRACT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_SUBTRACT_ADD,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_SUBTRACT_DOT,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_SUBTRACT_MULTIPLY,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_SUBTRACT_MULTIPLY2X,
    SHADER_EFFECT_SCREEN_MULTITEXTURE_SUBTRACT_SUBTRACT,
    SHADER_EFFECT_SCREEN_METER,
    SHADER_EFFECT_SUN_GLOW_CONVOLVE,
    SHADER_EFFECT_SUN_GLOW_DRAW,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NONLINEAR_TINT,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NONLINEAR_TINT_ADD,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NONLINEAR_TINT_ALPHA_BLEND,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NONLINEAR_TINT_DOUBLE_MULTIPLY,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NONLINEAR_TINT_MULTIPLY,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NONLINEAR_TINT_MULTIPLY_ADD,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NORMAL_TINT,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NORMAL_TINT_ADD,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NORMAL_TINT_ALPHA_BLEND,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NORMAL_TINT_DOUBLE_MULTIPLY,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NORMAL_TINT_MULTIPLY,
    SHADER_EFFECT_EFFECT_MULTITEXTURE_NORMAL_TINT_MULTIPLY_ADD,
    SHADER_EFFECT_EFFECT_NONLINEAR_TINT,
    SHADER_EFFECT_EFFECT_NONLINEAR_TINT_ADD,
    SHADER_EFFECT_EFFECT_NONLINEAR_TINT_ALPHA_BLEND,
    SHADER_EFFECT_EFFECT_NONLINEAR_TINT_DOUBLE_MULTIPLY,
    SHADER_EFFECT_EFFECT_NONLINEAR_TINT_MULTIPLY,
    SHADER_EFFECT_EFFECT_NONLINEAR_TINT_MULTIPLY_ADD,
    SHADER_EFFECT_EFFECT_NORMAL_TINT,
    SHADER_EFFECT_EFFECT_NORMAL_TINT_ADD,
    SHADER_EFFECT_EFFECT_NORMAL_TINT_ALPHA_BLEND,
    SHADER_EFFECT_EFFECT_NORMAL_TINT_DOUBLE_MULTIPLY,
    SHADER_EFFECT_EFFECT_NORMAL_TINT_MULTIPLY,
    SHADER_EFFECT_EFFECT_NORMAL_TINT_MULTIPLY_ADD,
    SHADER_EFFECT_TRANSPARENT_WATER_OPACITY,
    SHADER_EFFECT_TRANSPARENT_WATER_REFLECTION,
    SHADER_EFFECT_TRANSPARENT_WATER_BUMPMAP_CONVOLUTION,
    SHADER_EFFECT_ACTIVE_CAMOUFLAGE_DRAW,
    SHADER_EFFECT_TRANSPARENT_GLASS_REFLECTION_BUMPED,
    SHADER_EFFECT_TRANSPARENT_GLASS_REFLECTION_FLAT,
    SHADER_EFFECT_TRANSPARENT_GLASS_REFLECTION_MIRROR,
    SHADER_EFFECT_TRANSPARENT_GLASS_DIFFUSE,
    SHADER_EFFECT_TRANSPARENT_CHICAGO,
    SHADER_EFFECT_TRANSPARENT_METER,
    SHADER_EFFECT_ENVIRONMENT_FOG,
    SHADER_EFFECT_SCREEN_EFFECT,
    SHADER_EFFECT_MODEL_ENVIRONMENT,
    SHADER_EFFECT_MODEL_MASK_SELF_ILLUMINATION,
    SHADER_EFFECT_MODEL_MASK_CHANGE_COLOR,
    SHADER_EFFECT_MODEL_MASK_MULTIPURPOSE,
    SHADER_EFFECT_MODEL_MASK_REFLECTION,
    SHADER_EFFECT_MODEL_MASK_NONE,
    SHADER_EFFECT_TRANSPARENT_GENERIC,
    NUM_OF_SHADER_EFFECTS
};

enum VertexShaderIndex {
    VERTEX_SHADER_CONVOLUTION = 0,
    VERTEX_SHADER_DEBUG,
    VERTEX_SHADER_DECAL,
    VERTEX_SHADER_DETAIL_OBJECT_TYPE0,
    VERTEX_SHADER_DETAIL_OBJECT_TYPE1,
    VERTEX_SHADER_EFFECT,
    VERTEX_SHADER_EFFECT_MULTITEXTURE,
    VERTEX_SHADER_EFFECT_MULTITEXTURE_SCREENSPACE,
    VERTEX_SHADER_EFFECT_ZSPRITE,
    VERTEX_SHADER_ENVIRONMENT_DIFFUSE_LIGHT,
    VERTEX_SHADER_ENVIRONMENT_DIFFUSE_LIGHT_FF,
    VERTEX_SHADER_ENVIRONMENT_FOG,
    VERTEX_SHADER_ENVIRONMENT_FOG_SCREEN,
    VERTEX_SHADER_ENVIRONMENT_LIGHTMAP,
    VERTEX_SHADER_ENVIRONMENT_REFLECTION_BUMPED,
    VERTEX_SHADER_ENVIRONMENT_REFLECTION_FLAT,
    VERTEX_SHADER_ENVIRONMENT_REFLECTION_LIGHTMAP_MASK,
    VERTEX_SHADER_ENVIRONMENT_REFLECTION_MIRROR,
    VERTEX_SHADER_ENVIRONMENT_REFLECTION_RADIOSITY,
    VERTEX_SHADER_ENVIRONMENT_SHADOW,
    VERTEX_SHADER_ENVIRONMENT_SPECULAR_LIGHT,
    VERTEX_SHADER_ENVIRONMENT_SPECULAR_SPOT_LIGHT,
    VERTEX_SHADER_ENVIRONMENT_SPECULAR_LIGHTMAP,
    VERTEX_SHADER_ENVIRONMENT_TEXTURE,
    VERTEX_SHADER_LENS_FLARE,
    VERTEX_SHADER_MODEL_FOGGED,
    VERTEX_SHADER_MODEL,
    VERTEX_SHADER_MODEL_FF,
    VERTEX_SHADER_MODEL_FAST,
    VERTEX_SHADER_MODEL_SCENERY,
    VERTEX_SHADER_MODEL_ACTIVE_CAMOUFLAGE,
    VERTEX_SHADER_MODEL_ACTIVE_CAMOUFLAGE_FF,
    VERTEX_SHADER_MODEL_FOG_SCREEN,
    VERTEX_SHADER_MODEL_SHADOW,
    VERTEX_SHADER_MODEL_ZBUFFER,
    VERTEX_SHADER_SCREEN,
    VERTEX_SHADER_SCREEN2,
    VERTEX_SHADER_TRANSPARENT_GENERIC,
    VERTEX_SHADER_TRANSPARENT_GENERIC_LIT_M,
    VERTEX_SHADER_TRANSPARENT_GENERIC_M,
    VERTEX_SHADER_TRANSPARENT_GENERIC_OBJECT_CENTERED,
    VERTEX_SHADER_TRANSPARENT_GENERIC_OBJECT_CENTERED_M,
    VERTEX_SHADER_TRANSPARENT_GENERIC_REFLECTION,
    VERTEX_SHADER_TRANSPARENT_GENERIC_REFLECTION_M,
    VERTEX_SHADER_TRANSPARENT_GENERIC_SCREENSPACE,
    VERTEX_SHADER_TRANSPARENT_GENERIC_SCREENSPACE_M,
    VERTEX_SHADER_TRANSPARENT_GENERIC_VIEWER_CENTERED,
    VERTEX_SHADER_TRANSPARENT_GENERIC_VIEWER_CENTERED_M,
    VERTEX_SHADER_TRANSPARENT_GLASS_DIFFUSE_LIGHT,
    VERTEX_SHADER_TRANSPARENT_GLASS_DIFFUSE_LIGHT_M,
    VERTEX_SHADER_TRANSPARENT_GLASS_REFLECTION_BUMPED,
    VERTEX_SHADER_TRANSPARENT_GLASS_REFLECTION_BUMPED_M,
    VERTEX_SHADER_TRANSPARENT_GLASS_REFLECTION_FLAT,
    VERTEX_SHADER_TRANSPARENT_GLASS_REFLECTION_FLAT_M,
    VERTEX_SHADER_TRANSPARENT_GLASS_REFLECTION_MIRROR,
    VERTEX_SHADER_TRANSPARENT_GLASS_TINT,
    VERTEX_SHADER_TRANSPARENT_GLASS_TINT_M,
    VERTEX_SHADER_TRANSPARENT_METER,
    VERTEX_SHADER_TRANSPARENT_METER_M,
    VERTEX_SHADER_TRANSPARENT_PLASMA_M,
    VERTEX_SHADER_TRANSPARENT_WATER_OPACITY,
    VERTEX_SHADER_TRANSPARENT_WATER_OPACITY_M,
    VERTEX_SHADER_TRANSPARENT_WATER_REFLECTION,
    VERTEX_SHADER_TRANSPARENT_WATER_REFLECTION_M,
    NUM_OF_VERTEX_SHADERS
};

/**
 * Decrypts a shader file.
 * @param data          A pointer to the shader file data.
 * @param data_size     The size of the shader file data.
 * @return              true if successful, false if not.
 */
bool rasterizer_dx9_shader_decrypt_binary_file(void *data, size_t data_size);

/**
 * Reads a shader file from disk and decrypts it.
 * @param filename      The filename of the shader file.
 * @param buffer        A pointer to a buffer that will be allocated and filled with the shader file data.
 * @param bytes_read    A pointer to a size_t that will be filled with the size of the shader file data.
 * @return              true if successful, false if not.
 */
bool rasterizer_dx9_shader_read_binary_file(void **buffer, size_t *bytes_read, const char *filename);

/**
 * Loads the shader effects from a binary file.
 * @return true if successful, false if not.
 */
bool rasterizer_dx9_shader_load_effect_collection_from_binary(void);

/**
 * Disposes of all shader effects.
 */
void rasterizer_dx9_shader_dispose_effects(void);

/**
 * Get the shader effect for the given index.
 * @param index The index of the shader effect.
 * @return The pixel shader.
 */
RasterizerDx9ShaderEffect *rasterizer_dx9_shader_get_effect(uint16_t index);

/**
 * Get the vertex shader for the given index.
 * @param index The index of the vertex shader.
 * @return The vertex shader.
 */
IDirect3DVertexShader9 *rasterizer_dx9_shader_get_vertex_shader(uint16_t vsf_index);

/**
 * Get the vertex shader for the given index.
 * @param vertex_shader_permutation 
 * @param vertex_buffer_type 
 * @return Pointer to the vertex shader.
 */
IDirect3DVertexShader9 *rasterizer_dx9_shader_get_vertex_shader_for_permutation(uint16_t vertex_shader_permutation, VertexBufferType vertex_buffer_type);

#ifdef __cplusplus
}
#endif

#endif

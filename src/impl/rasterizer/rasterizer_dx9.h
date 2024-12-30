#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_H

#ifdef __cplusplus
extern "C" {
#endif

#include <d3d9.h>

#include "../tag/definitions/enum.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/tag.h"

enum VertexShaderIndex {
    VSFUNC_CONVOLUTION = 0,
    VSFUNC_DEBUG,
    VSFUNC_DECAL,
    VSFUNC_DETAIL_OBJECT_TYPE0,
    VSFUNC_DETAIL_OBJECT_TYPE1,
    VSFUNC_EFFECT,
    VSFUNC_EFFECT_MULTITEXTURE,
    VSFUNC_EFFECT_MULTITEXTURE_SCREENSPACE,
    VSFUNC_EFFECT_ZSPRITE,
    VSFUNC_ENVIRONMENT_DIFFUSE_LIGHT,
    VSFUNC_ENVIRONMENT_DIFFUSE_LIGHT_FF,
    VSFUNC_ENVIRONMENT_FOG,
    VSFUNC_ENVIRONMENT_FOG_SCREEN,
    VSFUNC_ENVIRONMENT_LIGHTMAP,
    VSFUNC_ENVIRONMENT_REFLECTION_BUMPED,
    VSFUNC_ENVIRONMENT_REFLECTION_FLAT,
    VSFUNC_ENVIRONMENT_REFLECTION_LIGHTMAP_MASK,
    VSFUNC_ENVIRONMENT_REFLECTION_MIRROR,
    VSFUNC_ENVIRONMENT_REFLECTION_RADIOSITY,
    VSFUNC_ENVIRONMENT_SHADOW,
    VSFUNC_ENVIRONMENT_SPECULAR_LIGHT,
    VSFUNC_ENVIRONMENT_SPECULAR_SPOT_LIGHT,
    VSFUNC_ENVIRONMENT_SPECULAR_LIGHTMAP,
    VSFUNC_ENVIRONMENT_TEXTURE,
    VSFUNC_LENS_FLARE,
    VSFUNC_MODEL_FOGGED,
    VSFUNC_MODEL,
    VSFUNC_MODEL_FF,
    VSFUNC_MODEL_FAST,
    VSFUNC_MODEL_SCENERY,
    VSFUNC_MODEL_ACTIVE_CAMOUFLAGE,
    VSFUNC_MODEL_ACTIVE_CAMOUFLAGE_FF,
    VSFUNC_MODEL_FOG_SCREEN,
    VSFUNC_MODEL_SHADOW,
    VSFUNC_MODEL_ZBUFFER,
    VSFUNC_SCREEN,
    VSFUNC_SCREEN2,
    VSFUNC_TRANSPARENT_GENERIC,
    VSFUNC_TRANSPARENT_GENERIC_LIT_M,
    VSFUNC_TRANSPARENT_GENERIC_M,
    VSFUNC_TRANSPARENT_GENERIC_OBJECT_CENTERED,
    VSFUNC_TRANSPARENT_GENERIC_OBJECT_CENTERED_M,
    VSFUNC_TRANSPARENT_GENERIC_REFLECTION,
    VSFUNC_TRANSPARENT_GENERIC_REFLECTION_M,
    VSFUNC_TRANSPARENT_GENERIC_SCREENSPACE,
    VSFUNC_TRANSPARENT_GENERIC_SCREENSPACE_M,
    VSFUNC_TRANSPARENT_GENERIC_VIEWER_CENTERED,
    VSFUNC_TRANSPARENT_GENERIC_VIEWER_CENTERED_M,
    VSFUNC_TRANSPARENT_GLASS_DIFFUSE_LIGHT,
    VSFUNC_TRANSPARENT_GLASS_DIFFUSE_LIGHT_M,
    VSFUNC_TRANSPARENT_GLASS_REFLECTION_BUMPED,
    VSFUNC_TRANSPARENT_GLASS_REFLECTION_BUMPED_M,
    VSFUNC_TRANSPARENT_GLASS_REFLECTION_FLAT,
    VSFUNC_TRANSPARENT_GLASS_REFLECTION_FLAT_M,
    VSFUNC_TRANSPARENT_GLASS_REFLECTION_MIRROR,
    VSFUNC_TRANSPARENT_GLASS_TINT,
    VSFUNC_TRANSPARENT_GLASS_TINT_M,
    VSFUNC_TRANSPARENT_METER,
    VSFUNC_TRANSPARENT_METER_M,
    VSFUNC_TRANSPARENT_PLASMA_M,
    VSFUNC_TRANSPARENT_WATER_OPACITY,
    VSFUNC_TRANSPARENT_WATER_OPACITY_M,
    VSFUNC_TRANSPARENT_WATER_REFLECTION,
    VSFUNC_TRANSPARENT_WATER_REFLECTION_M,
    VERTEX_SHADER_FUNCTIONS_MAX
};

enum VertexShaderDeclaration {
    VSDECL_ENVIRONMENT,
    VSDECL_ENVIRONMENT_C,
    VSDECL_ENVIRONMENT_LIGHTMAP,
    VSDECL_ENVIRONMENT_LIGHTMAP_C,
    VSDECL_MODEL,
    VSDECL_MODEL_C,
    VSDECL_UNLIT,
    VSDECL_DYNAMIC_UNLIT,
    VSDECL_SCREEN,
    VSDECL_DEBUG,
    VSDECL_DECAL,
    VSDECL_DETAIL_OBJECT,
    VSDECL_ENVIRONMENT_FF,
    VSDECL_ENVIRONMENT_LIGHTMAP_FF,
    VSDECL_MODEL_UNCOMPRESSED_FF,
    VSDECL_MODEL_PROCESSED,
    VSDECL_UNLIT_ZSPRITE,
    VSDECL_WIDGET,
    VERTEX_SHADER_DECLARATIONS_MAX,
};

typedef struct VertexShader {
    IDirect3DVertexShader9 *shader;
    const char *filepath;
} VertexShader;
_Static_assert(sizeof(VertexShader) == 0x8);

typedef struct VertexDeclaration {
    IDirect3DVertexDeclaration9 *declaration;
    uint32_t fvf;
    uint32_t vertex_processing_method;
} VertexDeclaration;
_Static_assert(sizeof(VertexDeclaration) == 0xC);

typedef struct RenderCamera {
    VectorXYZ position;
    VectorIJK forward;
    VectorIJK up;
    uint8_t mirrored;
    float vertical_field_of_view;
    Rectangle2D viewport_bounds;
    Rectangle2D window_bounds;
    float z_near;
    float z_far;
    Plane3D mirror_plane;
  } RenderCamera;
_Static_assert(sizeof(RenderCamera) == 0x54);

typedef struct RenderFrustum {
    Rectangle2DF frustum_bounds;
    Matrix4x3 world_to_view;
    Matrix4x3 view_to_world;
    Plane3D world_planes[6];
    float z_near;
    float z_far;
    VectorXYZ world_vertices[5];
    VectorXYZ world_midpoint;
    Rectangle3DF world_bounds;
    uint8_t projection_valid;
    float projection_matrix[4][4];
    VectorIJ projection_world_to_screen;
  } RenderFrustum;
_Static_assert(sizeof(RenderFrustum) == 0x18C);

typedef struct FogScreen {
    uint16_t flags;
    int16_t layer_count;
    float near_distance;
    float far_distance;
    float near_density;
    float far_density;
    float start_distance_from_fog_plane;
    int unused1[1];
    unsigned int color;
    float rotation_multiplier;
    float strafing_multiplier;
    float zoom_multiplier;
    int unused2[2];
    float map_scale;
    TagReference map;
    float animation_period;
    float animation_unused[1];
    float wind_velocity_lower_bound;
    float wind_velocity_upper_bound;
    float wind_period_lower_bound;
    float wind_period_upper_bound;
    float wind_acceleration_weight;
    float wind_perpendicular_weight;
    int wind_unused[2];
  } FogScreen;
_Static_assert(sizeof(FogScreen) == 0x70);

typedef struct RenderFog {
    uint16_t fog_definition_flags;
    uint16_t runtime_flags;
    ColorRGB atmospheric_color;
    float atmospheric_maximum_density;
    float atmospheric_minimum_distance;
    float atmospheric_maximum_distance;
    int16_t planar_mode;
    Plane3D plane;
    ColorRGB planar_color;
    float planar_maximum_density;
    float planar_maximum_distance;
    float planar_maximum_depth;
    FogScreen *screen;
    float screen_external_intensity;
  } RenderFog;
_Static_assert(sizeof(RenderFog) == 0x50);

typedef struct RenderGlobals {
    int32_t frame_index;
    int32_t scene_index;
    int16_t local_player_index;
    int16_t window_index;
    float time_delta_since_tick_sec;
    float time_delta_since_frame_sec;
    RenderCamera camera;
    RenderFrustum frustum;
    RenderFog fog;
    int32_t leaf_index;
    int32_t cluster_index;
    uint8_t under_water;
    uint8_t visible_sky_model;
    int16_t visible_sky_index;
} RenderGlobals;

typedef struct FrameParameters {
    double elapsed_time;
    float unk1;
    float unk2;
} FrameParameters;
_Static_assert(sizeof(FrameParameters) == 0x10);

/**
 * Get the window render parameters.
 * @return The window render parameters.
 */
RenderGlobals *get_render_globals(void);

/**
 * Get the pointer to the Direct3D 9 device.
* @return pointer to D3D9 device
*/
IDirect3DDevice9 *rasterizer_dx9_device(void);

/**
 * Get the vertex shader for the given index.
 * @param index The index of the vertex shader.
 * @return The vertex shader.
 */
IDirect3DVertexShader9 *rasterizer_dx9_get_vertex_shader(uint16_t vsf_index);

/**
 * Get the vertex shader for the given index.
 * @param vertex_shader_permutation 
 * @param vertex_buffer_type 
 * @return Pointer to the vertex shader.
 */
IDirect3DVertexShader9 *rasterizer_dx9_get_vertex_shader_for_permutation(uint16_t vertex_shader_permutation, uint16_t vertex_buffer_type);

/**
 * Set the vertex shader.
 * @param vertex_shader The vertex shader to set.
 */
void rasterizer_dx9_set_vertex_shader(IDirect3DVertexShader9 *vertex_shader);

/**
 * Get the vertex declaration at the given index
 * @param vertex_buffer_type The index of the vertex declaration.
 * @return The vertex declaration interface.
 */
IDirect3DVertexDeclaration9 *rasterizer_dx9_get_vertex_declaration(uint16_t vertex_buffer_type);

/**
 * Set the vertex declaration.
 * @param vertex_buffer_type The index of the vertex declaration.
 */
void rasterizer_dx9_set_vertex_declaration(uint16_t vertex_buffer_type);

/**
 * Set the pixel shader.
 * @param pixel_shader The pixel shader to set.
 */
void rasterizer_dx9_set_pixel_shader(IDirect3DPixelShader9 *pixel_shader);

/**
 * Set the render state.
 * @param state The render state to set.
 * @param value The value to set the render state to.
 */
void rasterizer_dx9_set_render_state(D3DRENDERSTATETYPE state, DWORD value);

/**
 * Set the framebuffer blend function
 */
void rasterizer_dx9_set_framebuffer_blend_function(FramebufferBlendFunction blend_function);

/**
 * Set the bitmap data texture
 * @param stage The stage to set the texture to.
 * @param bitmap_type The type of the bitmap.
 * @param bitmap_usage The usage of the bitmap.
 * @param bitmap_data_index The index of the bitmap data.
 * @param bitmap_tag The tag handle of the bitmap.
 * @return True if the texture was set successfully, false otherwise.
 */
bool rasterizer_dx9_set_bitmap_data_texture(uint16_t stage, BitmapType bitmap_type, BitmapUsage bitmap_usage, uint16_t bitmap_data_index, TagHandle bitmap_tag);

/**
 * Set the sampler state
 * @param sampler The sampler to set the state of.
 * @param type The type of the sampler state.
 * @param value The value to set the sampler state to.
 */
void rasterizer_dx9_set_sampler_state(uint16_t sampler, D3DSAMPLERSTATETYPE type, DWORD value);

/**
 * Set the texture
 * @param stage The stage to set the texture to.
 * @param texture The texture to set.
 */
void rasterizer_dx9_set_texture(uint16_t stage, IDirect3DTexture9 *texture);

/**
 * Set the vertex shader constant
 * @param start_register The start register to set the constant to.
 * @param data The data to set the constant to.
 * @param count The number of constants to set.
 */
bool rasterizer_dx9_set_vertex_shader_constant_f(uint16_t start_register, const float *data, uint16_t count);

/**
 * Set the pixel shader constant
 * @param start_register The start register to set the constant to.
 * @param data The data to set the constant to.
 * @param count The number of constants to set.
 */
bool rasterizer_dx9_set_pixel_shader_constant_f(uint16_t start_register, const float *data, uint16_t count);

/**
 * Set the texture stage state
 * @param stage The stage to set the state of.
 * @param type The type of the texture stage state.
 * @param value The value to set the texture stage state to.
 */
void rasterizer_dx9_set_texture_stage_state(uint16_t stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);

#ifdef __cplusplus
}
#endif

#endif

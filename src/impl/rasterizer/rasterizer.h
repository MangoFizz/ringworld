#ifndef RINGWORLD__RASTERIZER__RASTERIZER_H
#define RINGWORLD__RASTERIZER__RASTERIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <d3d9.h>

#include "../render/render.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/definitions/globals.h"
#include "../tag/definitions/shader.h"
#include "../tag/tag.h"

typedef enum RasterizerLock {
    RASTERIZER_LOCK_NONE = 0,
    RASTERIZER_LOCK_TEXTURE_CHANGED,
    RASTERIZER_LOCK_VERTEXBUFFER_NEW,
    RASTERIZER_LOCK_DETAIL_OBJECTS,
    RASTERIZER_LOCK_DECAL_UPDATE,
    RASTERIZER_LOCK_DECAL_VERTICES,
    RASTERIZER_LOCK_BINK,
    RASTERIZER_LOCK_UI,
    RASTERIZER_LOCK_CINEMATICS,
    RASTERIZER_LOCK_KOTH,
    RASTERIZER_LOCK_HUD,
    RASTERIZER_LOCK_FLAG,
    RASTERIZER_LOCK_LIGHTNING,
    RASTERIZER_LOCK_DEBUG,
    RASTERIZER_LOCK_TEXT,
    RASTERIZER_LOCK_CONTRAIL,
    RASTERIZER_LOCK_SPRITE,
    RASTERIZER_LOCK_BSP_SWITCH,
    NUMBER_OF_RASTERIZER_LOCKS
} RasterizerLock;

typedef struct RasterizerGlobals {
    Bool initialized;
    int16_t lock_index;
    Rectangle2D screen_bounds;
    Rectangle2D frame_bounds;
    int64_t frame_index;
    uint32_t flip_index;
    uint8_t use_floating_point_zbuffer;
    uint8_t use_rasterizer_frame_rate_throttle;
    uint8_t use_rasterizer_frame_rate_stabilization;
    int16_t refresh_rate;
    float z_near;
    float z_far;
    float z_near_first_person;
    float z_far_first_person;
    IDirect3DBaseTexture9 **default_texture_white;
    IDirect3DBaseTexture9 **default_texture_2d_texture;
    IDirect3DBaseTexture9 **default_texture_3d_texture; // engine actually uses 2d for 3d cases
    IDirect3DBaseTexture9 **default_texture_cubemap;
    int16_t lightmap_mode;
    int16_t maximum_nodes_per_model;
    Bool using_software_vertex_processing;
    int16_t unk2;
    uint32_t fixed_function_ambient; // ambient light value for FF
    Bool use_cheap_active_camo;
    Bool render_targets_disabled;
    Bool alpha_render_targets_disabled;
} RasterizerGlobals; 
_Static_assert(sizeof(RasterizerGlobals) == 0x60);

typedef struct RasterizerWindowRenderParameters {
    int16_t render_target;
    int16_t window_index;
    uint8_t has_mirror;
    uint8_t suppress_clear;
    RenderCamera camera;
    RenderFrustum frustum;
    RenderFog fog;
    RenderScreenFlash screen_flash;
    RenderScreenEffect screen_effect;
} RasterizerWindowRenderParameters;
_Static_assert(sizeof(RasterizerWindowRenderParameters) == 0x258);

typedef struct RasterizerFrameParameters {
    double elapsed_time_sec;
    float delta_time;
} RasterizerFrameParameters;
_Static_assert(sizeof(RasterizerFrameParameters) == 0x10);

/**
 * Get the pointer to the rasterizer globals.
 * @return pointer to rasterizer globals
 */
RasterizerGlobals *rasterizer_get_globals(void);

/**
 * Get the rasterizer window render parameters.
 * @return The rasterizer window render parameters.
 */
RasterizerWindowRenderParameters *rasterizer_get_window_parameters(void);

/**
 * Get the frame parameters.
 * @return The frame parameters.
 */
RasterizerFrameParameters*rasterizer_get_frame_parameters(void);

/**
 * Get the rasterizer data.
 * @return The rasterizer data.
 */
GlobalsRasterizerData *render_get_globals_rasterizer_data(void);

#ifdef __cplusplus
}
#endif

#endif

#ifndef RINGWORLD__RASTERIZER__RASTERIZER_RENDER_H
#define RINGWORLD__RASTERIZER__RASTERIZER_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/enum.h"
#include "../tag/definitions/globals.h"
#include "../tag/tag.h"

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
RenderGlobals *rasterizer_render_get_globals(void);

/**
 * Get the frame parameters.
 * @return The frame parameters.
 */
FrameParameters *rasterizer_render_get_frame_parameters(void);

/**
 * Get whether the fog is enabled.
 */
bool rasterizer_render_get_fog_enabled(void);

/**
 * Get the rasterizer data.
 * @return The rasterizer data.
 */
GlobalsRasterizerData *rasterizer_render_get_globals_rasterizer_data(void);

#ifdef __cplusplus
}
#endif

#endif

#ifndef RINGWORLD__RENDER__RENDER_H
#define RINGWORLD__RENDER__RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../math/math.h"
#include "../tag/definitions/enum.h"
#include "../tag/definitions/globals.h"
#include "../tag/tag.h"

enum {
    RASTERIZER_SCREEN_BASE_WIDTH = 640,
    RASTERIZER_SCREEN_BASE_HEIGHT = 480
};

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
    Bounds2D frustum_bounds;
    Matrix4x3 world_to_view;
    Matrix4x3 view_to_world;
    Plane3D world_planes[6];
    float z_near;
    float z_far;
    VectorXYZ world_vertices[5];
    VectorXYZ world_midpoint;
    Rectangle3DF world_bounds;
    uint8_t projection_valid;
    ProjectionMatrix projection_matrix;
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
    uint32_t time_delta_since_tick;
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

typedef struct RenderScreenFlash {
    int16_t type;
    float intensity;
    ColorARGB color;
} RenderScreenFlash;
_Static_assert(sizeof(RenderScreenFlash) == 0x18);

typedef struct RenderScreenEffect {
    int16_t type;
    float intensity;
} RenderScreenEffect;
_Static_assert(sizeof(RenderScreenEffect) == 0x8);

typedef struct RenderWindow {
    int16_t local_player_index;
    uint8_t console_window;
    RenderCamera render_camera;
    RenderCamera rasterizer_camera;
} RenderWindow;
_Static_assert(sizeof(struct RenderWindow) == 0xAC);

/**
 * Get the window render parameters.
 * @return The window render parameters.
 */
RenderGlobals *render_get_globals(void);

/**
 * Get whether the fog is enabled.
 */
bool render_get_fog_enabled(void);

/**
 * Get the rasterizer data.
 * @return The rasterizer data.
 */
GlobalsRasterizerData *render_get_globals_rasterizer_data(void);

/**
* Get the width of the screen.
* @return The width of the screen.
*/
uint16_t render_get_screen_width(void);

/**
* Get the height of the screen.
* @return The height of the screen.
*/
uint16_t render_get_screen_height(void);

/**
* Check if user interface rendering is enabled.
* @return True if user interface rendering is enabled, false otherwise.
*/
bool render_user_interface_enabled(void);

/**
* Check if widescreen support is enabled.
* @return True if widescreen support is enabled, false otherwise.
*/
bool render_widescreen_support_enabled(void);

/**
 * Project a point from world space to screen space.
 * @param camera The camera.
 * @param frustum The frustum.
 * @param view_point The point in world space.
 * @param screen_point The point in screen space.
 * @return True if the point was projected successfully, false otherwise.
 */
bool render_project_point_to_screen_space(RenderCamera *camera, RenderFrustum *frustum, VectorXYZ *view_point, VectorXY *screen_point);

#ifdef __cplusplus
}
#endif

#endif

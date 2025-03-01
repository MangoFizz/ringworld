#include <stdint.h>

#include <math.h>
#include "../types/types.h"
#include "../exception/exception.h"
#include "../rasterizer/rasterizer_dx9_render_target.h"
#include "render.h"

extern RenderGlobals *render_globals;
extern bool *fog_enable;
extern GlobalsRasterizerData **globals_rasterizer_data;
extern bool *rasterizer_enable_user_interface_render;
bool rasterizer_enable_widescreen_support = true;

RenderGlobals *render_get_globals(void) {
    return render_globals;
}

bool render_get_fog_enabled(void) {
    return *fog_enable;
}

GlobalsRasterizerData *render_get_globals_rasterizer_data(void) {
    ASSERT(globals_rasterizer_data != NULL);
    return *globals_rasterizer_data;
}

uint16_t render_get_screen_width(void) {
    RasterizerDx9RenderTarget *render_target = rasterizer_dx9_render_target_get(RENDER_TARGET_BACK_BUFFER);
    if(rasterizer_enable_widescreen_support && render_target->width > render_target->height) {
        float aspect_ratio = (float)render_target->width / (float)render_target->height;
        float width = math_float_to_long(RASTERIZER_SCREEN_BASE_HEIGHT * aspect_ratio);
        return (uint16_t)width;
    }
    return RASTERIZER_SCREEN_BASE_WIDTH;
}

uint16_t render_get_screen_height(void) {
    RasterizerDx9RenderTarget *render_target = rasterizer_dx9_render_target_get(RENDER_TARGET_BACK_BUFFER);
    if(rasterizer_enable_widescreen_support && render_target->height > render_target->width) {
        float aspect_ratio = (float)render_target->height / (float)render_target->width;
        float height = math_float_to_long(RASTERIZER_SCREEN_BASE_WIDTH * aspect_ratio);
        return (uint16_t)height;
    }
    return RASTERIZER_SCREEN_BASE_HEIGHT;
}

bool render_user_interface_enabled(void) {
    return *rasterizer_enable_user_interface_render;
}

bool render_widescreen_support_enabled(void) {
    return rasterizer_enable_widescreen_support;
}

bool render_project_point_to_screen_space(RenderCamera *camera, RenderFrustum *frustum, VectorXYZ *view_point, VectorXY *screen_point) {
    ASSERT_OR_RETURN(camera, false);
    ASSERT_OR_RETURN(frustum, false);
    ASSERT_OR_RETURN(view_point, false);
    ASSERT_OR_RETURN(screen_point, false);
    if(view_point->z <= 0.0f && view_point->z != 0.0f) {
        ASSERT_OR_RETURN(frustum->projection_valid, false);
        uint16_t screen_width = render_get_screen_width();
        uint16_t screen_height = render_get_screen_height();
        float inverse_depth = -1.0f / view_point->z;
        screen_point->x = (view_point->x * (frustum->projection_matrix).x[0] + view_point->z * (frustum->projection_matrix).z[0]) * inverse_depth;
        screen_point->y = (inverse_depth * -1.0f) * (view_point->y * frustum->projection_matrix.y[1] + view_point->z * frustum->projection_matrix.z[1]);
        if(screen_point->x >= -1.0f && screen_point->x <= 1.0f && screen_point->y >= -1.0f && screen_point->y <= 1.0) {
            screen_point->x = camera->viewport_bounds.left + (screen_point->x * 0.5f + 0.5f) * screen_width;
            screen_point->y = camera->viewport_bounds.top + (screen_point->y * 0.5f + 0.5f) * screen_height;
            return true;
        }
    }
    return false;
}

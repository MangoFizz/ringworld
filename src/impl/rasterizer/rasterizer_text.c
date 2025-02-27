#include <string.h>
#include "../exception/exception.h"
#include "../bitmap/bitmap.h"
#include "../math/math.h"
#include "../render/render.h"
#include "../text/text.h"
#include "rasterizer.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_dx9_vertex_shader.h"
#include "rasterizer_screen.h"
#include "rasterizer_text.h"

extern RasterizerFontCache *rasterizer_font_character_cache;
extern VertexShaderScreenprojConstants *rasterizer_text_vertex_shader_constants;

RasterizerFontCache *rasterizer_text_get_font_cache(void) {
    return rasterizer_font_character_cache;
}

BitmapData *rasterizer_text_get_font_cache_bitmap(void) {
    if(rasterizer_font_character_cache->initialized) {
        return rasterizer_font_character_cache->bitmap;
    }
    return NULL;
}

void rasterizer_text_set_up_vertex_shader_constants(void) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    Rectangle2D *window_bounds = &window_parameters->camera.window_bounds;
    float window_width = window_bounds->right - window_bounds->left;
    float window_height = window_bounds->top - window_bounds->bottom;
    float inv_screen_width = 2.0f / rasterizer_screen_get_width();
    float inv_screen_height = -2.0f / rasterizer_screen_get_height();

    VertexShaderScreenprojConstants *screenproj = rasterizer_text_vertex_shader_constants;
    screenproj->projection.x[0] = inv_screen_width;
    screenproj->projection.x[1] = 0.0f;
    screenproj->projection.x[2] = 0.0f;
    screenproj->projection.x[3] = -1.0f - 1.0f / window_width;
    screenproj->projection.y[0] = 0.0f;
    screenproj->projection.y[1] = inv_screen_height;
    screenproj->projection.y[2] = 0.0f;
    screenproj->projection.y[3] = 1.0f + 1.0f / window_height;
    screenproj->projection.z[0] = 0.0f;
    screenproj->projection.z[1] = 0.0f;
    screenproj->projection.z[2] = 0.0f;
    screenproj->projection.z[3] = 0.5f;
    screenproj->projection.w[0] = 0.0f;
    screenproj->projection.w[1] = 0.0f;
    screenproj->projection.w[2] = 0.0f;
    screenproj->projection.w[3] = 1.0f;
    screenproj->texture_scale.x = 0.0f;
    screenproj->texture_scale.y = 0.0f;
}

bool rasterizer_text_cache_initialize(void) {
    RasterizerFontCache *font_cache = rasterizer_text_get_font_cache();
    ASSERT(font_cache->initialized == false);

    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();
    BitmapData *bitmap = bitmap_new_2d_bitmap_data(screen_width, screen_height, 1, BITMAP_DATA_FORMAT_A4R4G4B4);
    if(bitmap) {
        memset(font_cache, 0, sizeof(RasterizerFontCache));
        bool texture_created = rasterizer_dx9_texture_create(bitmap);
        if(texture_created) {
            font_cache->initialized = true;
            font_cache->bitmap = bitmap;
        }
    }

    return font_cache->initialized;
}

void rasterizer_draw_unicode_string(Rectangle2D *position, Rectangle2D *dest_rect, ColorARGBInt *color, uint32_t flags, const wchar_t *string) {
    RenderGlobals *render_globals = render_get_globals();
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    BitmapData *bitmap = rasterizer_text_get_font_cache_bitmap();

    ASSERT(render_globals != NULL);
    ASSERT(string != NULL);
    ASSERT(bitmap != NULL);

    if(rasterizer_screen_user_interface_render_enabled() == false || window_parameters->render_target != 1) {
        return;
    }

    size_t string_length = wcslen(string);
    Rectangle2D final_position;
    if(position != NULL) {
        final_position = *position;
    }
    else {
        final_position = render_globals->camera.window_bounds;
        math_rectangle_2d_translate(&final_position, render_globals->camera.viewport_bounds.left * -1, render_globals->camera.viewport_bounds.top * -1);
    }

    Rectangle2D final_rect;
    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();
    if(dest_rect != NULL) {
        final_rect.bottom = clamp_i32(dest_rect->bottom, 0, screen_height);
        final_rect.right = clamp_i32(dest_rect->right, 0, screen_width);
        final_rect.top = clamp_i32(dest_rect->top, 0, screen_height);
        final_rect.left = clamp_i32(dest_rect->left, 0, screen_width);
    }
    else {
        // @todo use viewport bounds for calculating this
        final_rect.bottom = screen_height;
        final_rect.right = screen_width;
        final_rect.top = 0;
        final_rect.left = 0;
    }

    RasterizerDynamicScreenGeometryParams screen_geometry_parameters;
    memset(&screen_geometry_parameters, 0, sizeof(screen_geometry_parameters));
    screen_geometry_parameters.map_texture_scale[0].x = 1.0f / bitmap->width;
    screen_geometry_parameters.map_texture_scale[0].y = 1.0f / bitmap->height;
    screen_geometry_parameters.map_scale[0].x = 1.0f;
    screen_geometry_parameters.map_scale[0].y = 1.0f;
    screen_geometry_parameters.point_sampled = false;
    screen_geometry_parameters.framebuffer_blend_function = FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND;
    screen_geometry_parameters.map[0] = bitmap;
    screen_geometry_parameters.meter_parameters = NULL;

    rasterizer_text_begin(&screen_geometry_parameters);
    text_draw_unicode_string(rasterizer_text_draw_character_with_shadow, &final_position, color, &final_rect, flags, string);
    rasterizer_text_end();
}

void rasterizer_draw_string(Rectangle2D *position, Rectangle2D *dest_rect, ColorARGBInt *color, uint32_t flags, const char *string) {
    RenderGlobals *render_globals = render_get_globals();
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    BitmapData *bitmap = rasterizer_text_get_font_cache_bitmap();

    ASSERT(render_globals != NULL);
    ASSERT(string != NULL);
    ASSERT(bitmap != NULL);

    if(rasterizer_screen_user_interface_render_enabled() == false || window_parameters->render_target != 1) {
        return;
    }

    size_t string_length = strlen(string);
    Rectangle2D final_position;
    if(position != NULL) {
        final_position = *position;
    }
    else {
        final_position = render_globals->camera.window_bounds;
        math_rectangle_2d_translate(&final_position, render_globals->camera.viewport_bounds.left * -1, render_globals->camera.viewport_bounds.top * -1);
    }

    Rectangle2D final_rect;
    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();
    if(dest_rect != NULL) {
        final_rect.bottom = clamp_i32(dest_rect->bottom, 0, screen_height);
        final_rect.right = clamp_i32(dest_rect->right, 0, screen_width);
        final_rect.top = clamp_i32(dest_rect->top, 0, screen_height);
        final_rect.left = clamp_i32(dest_rect->left, 0, screen_width);
    }
    else {
        final_rect.bottom = render_globals->camera.viewport_bounds.right - render_globals->camera.viewport_bounds.left;
        final_rect.right = render_globals->camera.viewport_bounds.bottom - render_globals->camera.viewport_bounds.top;
        final_rect.top = 0;
        final_rect.left = 0;
    }

    RasterizerDynamicScreenGeometryParams screen_geometry_parameters;
    memset(&screen_geometry_parameters, 0, sizeof(screen_geometry_parameters));
    screen_geometry_parameters.map_texture_scale[0].x = 1.0f / bitmap->width;
    screen_geometry_parameters.map_texture_scale[0].y = 1.0f / bitmap->height;
    screen_geometry_parameters.map_scale[0].x = 1.0f;
    screen_geometry_parameters.map_scale[0].y = 1.0f;
    screen_geometry_parameters.point_sampled = false;
    screen_geometry_parameters.framebuffer_blend_function = FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND;
    screen_geometry_parameters.map[0] = bitmap;
    screen_geometry_parameters.meter_parameters = NULL;

    rasterizer_text_begin(&screen_geometry_parameters);
    text_draw_string(rasterizer_text_draw_character_with_shadow, &final_position, color, &final_rect, flags, string);
    rasterizer_text_end();
}

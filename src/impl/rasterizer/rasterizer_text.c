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
extern uint16_t *string_draw_count;

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
    BitmapData *bitmap = bitmap_new_2d_bitmap_data(screen_width, screen_height, 1, BITMAP_DATA_FORMAT_A8R8G8B8);
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

    *string_draw_count = *string_draw_count + 1;

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

    *string_draw_count = *string_draw_count + 1;

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

void rasterizer_text_dispose_font_character(FontCacheCharacter *character_cache) {
    ASSERT(character_cache != NULL);
    if(character_cache->character != NULL) {
        character_cache->character->hardware_character_index = -1;
        if(character_cache->character->draw_generation == *string_draw_count) {
            debug_printf("Trying to dispose of a character that is still in use: %d\n", character_cache->character->hardware_character_index);
        }
        character_cache->character = NULL;
    }
}

void rasterizer_text_cache_font_character(Font *font, FontCharacter *character) {
    RasterizerFontCache *font_cache = rasterizer_text_get_font_cache();
    
    ASSERT(font_cache);
    ASSERT(font_cache->initialized);
    ASSERT(font);
    ASSERT(font->pixels.pointer);
    ASSERT(character);
    ASSERT(character->bitmap_width <= FONT_CHARACTER_CACHE_BITMAP_WIDTH);
    ASSERT(character->bitmap_height <= FONT_CHARACTER_CACHE_BITMAP_HEIGHT);

    if(character->hardware_character_index != UINT16_MAX) {
        return;
    }

    character->draw_generation = *string_draw_count;

    if(character->bitmap_width + font_cache->position.x + 2 > FONT_CHARACTER_CACHE_BITMAP_WIDTH) {
        font_cache->position.x = 0;
        font_cache->position.y = font_cache->position.y + font_cache->maximum_character_height;
        font_cache->maximum_character_height = 0;
    }

    if(character->bitmap_height + font_cache->position.y + 2 > FONT_CHARACTER_CACHE_BITMAP_HEIGHT) {
        font_cache->position.y = 0;
        font_cache->position.x = 0;
        font_cache->maximum_character_height = 0;
        FontCacheCharacter *character_cache = &font_cache->characters[font_cache->read_index];
        while(font_cache->read_index != font_cache->write_index && character_cache->position.y > 0) {
            rasterizer_text_dispose_font_character(character_cache);
            font_cache->read_index = (font_cache->read_index + 1) % MAX_FONT_CACHE_CHARACTERS;
            character_cache = &font_cache->characters[font_cache->read_index];
        }
    }

    if(character->bitmap_height + 2 >= font_cache->maximum_character_height) {
        uint16_t character_baseline_y = font_cache->position.y + font_cache->maximum_character_height;
        uint16_t character_bottom_y = font_cache->position.y + character->bitmap_height + 2;
        FontCacheCharacter *character_cache = &font_cache->characters[font_cache->read_index];
        while(font_cache->read_index != font_cache->write_index && character_cache->position.y >= character_baseline_y && character_cache->position.y < character_bottom_y) {
            rasterizer_text_dispose_font_character(character_cache);
            font_cache->read_index = (font_cache->read_index + 1) % MAX_FONT_CACHE_CHARACTERS;
            character_cache = &font_cache->characters[font_cache->read_index];
        }
        font_cache->maximum_character_height = character->bitmap_height + 2;
    }

    uint16_t character_left_x = font_cache->position.x + character->bitmap_width + 2;
    if(character_left_x == font_cache->read_index) {
        rasterizer_text_dispose_font_character(&font_cache->characters[font_cache->read_index]);
        font_cache->read_index = (font_cache->read_index + 1) % MAX_FONT_CACHE_CHARACTERS;
    }
    
    FontCacheCharacter *character_cache = &font_cache->characters[font_cache->write_index];
    character_cache->character = character;
    character_cache->position.x = font_cache->position.x;
    character_cache->position.y = font_cache->position.y;
    character->hardware_character_index = font_cache->write_index;

    uint8_t *character_pixels = font->pixels.pointer + character->pixels_offset;
    size_t pixels_count = 0;
    for(size_t offset_y = 0; offset_y < character->bitmap_height + 2; offset_y++) {
        ColorARGBInt *pixel = bitmap_address_for_pixel(font_cache->position.x, font_cache->position.y + offset_y, 0, font_cache->bitmap);
        for(size_t offset_x = 0; offset_x < character->bitmap_width + 2; offset_x++) {
            if(offset_y <= 0 || offset_y > character->bitmap_height || offset_x <= 0 || offset_x > character->bitmap_width) {
                *pixel = 0x00000000;
            }
            else {
                *pixel = *character_pixels << 24 | 0xFFFFFF;
                character_pixels++;
                pixels_count++;
            }
            pixel++;
        }
    }
    
    character_cache->position.x = character_cache->position.x + 1;
    character_cache->position.y = character_cache->position.y + 1;

    ASSERT(pixels_count == character->bitmap_width * character->bitmap_height);

    rasterizer_dx9_texture_update_bitmap_2d(font_cache->bitmap);
    font_cache->position.x = font_cache->position.x + character->bitmap_width + 2;
    font_cache->write_index = (font_cache->write_index + 1) % MAX_FONT_CACHE_CHARACTERS;
}

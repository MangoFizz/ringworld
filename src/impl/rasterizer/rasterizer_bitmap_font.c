#include <string.h>
#include "../exception/exception.h"
#include "../bitmap/bitmap.h"
#include "../font/bitmap_font.h"
#include "../math/math.h"
#include "../render/render.h"
#include "../text/text.h"
#include "rasterizer.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_dx9_render_target.h"
#include "rasterizer_dx9_vertex_shader.h"
#include "rasterizer_dx9_vertex_shader_constants.h"
#include "rasterizer_bitmap_font.h"

extern BitmapFontCache *rasterizer_font_character_cache;
extern VertexShaderScreenprojConstants *rasterizer_bitmap_font_vertex_shader_constants;
extern uint16_t *string_draw_count;
extern bool *rasterizer_enable_user_interface_render;
extern bool *rasterizer_window_is_being_rendered;

BitmapFontCache *rasterizer_bitmap_font_get_cache(void) {
    return rasterizer_font_character_cache;
}

BitmapData *rasterizer_bitmap_font_get_cache_bitmap(void) {
    if(rasterizer_font_character_cache->initialized) {
        return rasterizer_font_character_cache->bitmap;
    }
    return NULL;
}

void rasterizer_bitmap_font_set_up_vertex_shader_constants(void) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    Rectangle2D *window_bounds = &window_parameters->camera.window_bounds;
    float window_width = window_bounds->right - window_bounds->left;
    float window_height = window_bounds->top - window_bounds->bottom;
    float inv_screen_width = 2.0f / render_get_screen_width();
    float inv_screen_height = -2.0f / render_get_screen_height();

    VertexShaderScreenprojConstants *screenproj = rasterizer_bitmap_font_vertex_shader_constants;
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

    // Probably not needed, but just in case
    screenproj->pad1 = 0.0f;
    screenproj->pad2 = 0.0f;
}

bool rasterizer_bitmap_font_cache_initialize(void) {
    BitmapFontCache *font_cache = rasterizer_bitmap_font_get_cache();
    ASSERT(font_cache->initialized == false);

    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();
    BitmapData *bitmap = bitmap_new_2d_bitmap_data(screen_width, screen_height, 1, BITMAP_DATA_FORMAT_A8R8G8B8);
    if(bitmap) {
        memset(font_cache, 0, sizeof(BitmapFontCache));
        bool texture_created = rasterizer_dx9_texture_create(bitmap);
        if(texture_created) {
            font_cache->initialized = true;
            font_cache->bitmap = bitmap;
        }
    }

    return font_cache->initialized;
}

void rasterizer_bitmap_font_cache_flush(void) {
    BitmapFontCache *font_cache = rasterizer_bitmap_font_get_cache();
    if(font_cache->initialized) {
        for(size_t i = 0; i < MAX_FONT_CACHE_CHARACTERS; i++) {
            BitmapFontCacheCharacter *character = &font_cache->characters[i];
            if(character->character != NULL) {
                character->character->hardware_character_index = -1;
            }
            character->character = NULL;
        }
    }
}

void rasterizer_bitmap_font_begin_text_draw(RasterizerDynamicScreenGeometryParams *parameters) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    IDirect3DDevice9 *device = rasterizer_dx9_device();
  
    if(rasterizer_enable_user_interface_render && window_parameters->render_target == RENDER_TARGET_RENDER_PRIMARY) {
        rasterizer_bitmap_font_set_up_vertex_shader_constants();
        rasterizer_dx9_set_framebuffer_blend_function(parameters->framebuffer_blend_function);
        BitmapData *map = parameters->map[0];
        if(map != NULL) {
            bitmap_load(true, true, map);
            IDirect3DDevice9_SetTexture(device, 0, map->hardware_format);
        }
        
        rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_CCW);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
        rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, TRUE);
        rasterizer_dx9_set_render_state(D3DRS_ALPHAREF, 0);
        rasterizer_dx9_set_render_state(D3DRS_ZENABLE, FALSE);
        rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

        if(*rasterizer_window_is_being_rendered) {
            rasterizer_dx9_set_render_state(D3DRS_FILLMODE, D3DFILL_SOLID);
        }

        rasterizer_dx9_set_vertex_declaration(VERTEX_DECLARATION_DYNAMIC_SCREEN);
        uint32_t vertex_processing_method = rasterizer_dx9_vertex_get_processing_method(VERTEX_DECLARATION_DYNAMIC_SCREEN);
        rasterizer_dx9_set_software_vertex_processing(vertex_processing_method & D3DUSAGE_SOFTWAREPROCESSING);

        IDirect3DVertexShader9 *vs = rasterizer_dx9_shader_get_vertex_shader(VSH_SCREEN);
        rasterizer_dx9_set_vertex_shader(vs);
        rasterizer_dx9_set_pixel_shader(NULL);

        VertexShaderScreenprojConstants *vs_constants = rasterizer_bitmap_font_vertex_shader_constants;
        vs_constants->texture_scale.x = parameters->map_texture_scale[0].x;
        vs_constants->texture_scale.y = parameters->map_texture_scale[0].y;
        rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_SCREENPROJ_OFFSET, vs_constants, VSH_CONSTANTS_SCREENPROJ_COUNT);

        for(size_t i = 0; i < 3; i++) {
            map = parameters->map[i];
            if(map == NULL) {
                rasterizer_dx9_set_texture(i, NULL);
                break;
            }
            bitmap_load(true, true, map);
            rasterizer_dx9_set_texture(i, map->hardware_format);
            rasterizer_dx9_set_sampler_state(i, D3DSAMP_ADDRESSU, parameters->map_wrapped[i] ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
            rasterizer_dx9_set_sampler_state(i, D3DSAMP_ADDRESSV, parameters->map_wrapped[i] ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
            rasterizer_dx9_set_sampler_state(i, D3DSAMP_MAGFILTER, parameters->point_sampled ? D3DTEXF_POINT : D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(i, D3DSAMP_MINFILTER, parameters->point_sampled ? D3DTEXF_POINT : D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(i, D3DSAMP_MIPFILTER, parameters->point_sampled ? D3DTEXF_POINT : D3DTEXF_LINEAR);
        }

        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        rasterizer_dx9_set_texture_stage_state(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        rasterizer_dx9_set_texture_stage_state(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
}

void rasterizer_bitmap_font_end_text_draw(void) {
    if(*rasterizer_window_is_being_rendered) {
        rasterizer_dx9_set_render_state(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }
    rasterizer_dx9_set_software_vertex_processing(rasterizer_dx9_device_supports_software_vertex_processing());
}

void rasterizer_bitmap_font_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    BitmapData *bitmap = rasterizer_bitmap_font_get_cache_bitmap();

    ASSERT(string != NULL);
    ASSERT(bitmap != NULL);

    if(render_user_interface_enabled() == false || window_parameters->render_target != 1) {
        return;
    }

    *string_draw_count = *string_draw_count + 1;

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

    rasterizer_bitmap_font_begin_text_draw(&screen_geometry_parameters);
    bitmap_font_draw_unicode_string(rasterizer_bitmap_font_draw_character_with_shadow, bounds, offset_out, text_rect, flags, string);
    rasterizer_bitmap_font_end_text_draw();
}

void rasterizer_bitmap_font_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    BitmapData *bitmap = rasterizer_bitmap_font_get_cache_bitmap();

    ASSERT(string != NULL);
    ASSERT(bitmap != NULL);

    if(render_user_interface_enabled() == false || window_parameters->render_target != 1) {
        return;
    }

    *string_draw_count = *string_draw_count + 1;

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

    rasterizer_bitmap_font_begin_text_draw(&screen_geometry_parameters);
    bitmap_font_draw_string(rasterizer_bitmap_font_draw_character_with_shadow, bounds, offset_out, text_rect, flags, string);
    rasterizer_bitmap_font_end_text_draw();
}

void rasterizer_bitmap_font_dispose_font_character(BitmapFontCacheCharacter *character_cache) {
    ASSERT(character_cache != NULL);
    if(character_cache->character != NULL) {
        character_cache->character->hardware_character_index = -1;
        if(character_cache->character->draw_generation == *string_draw_count) {
            debug_printf("Trying to dispose of a character that is still in use: %d\n", character_cache->character->hardware_character_index);
        }
        character_cache->character = NULL;
    }
}

void rasterizer_bitmap_font_cache_font_character(Font *font, FontCharacter *character) {
    BitmapFontCache *font_cache = rasterizer_bitmap_font_get_cache();
    
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

    uint16_t bitmap_width = character->bitmap_width + 2;
    uint16_t bitmap_height = character->bitmap_height + 2;

    if(bitmap_width + font_cache->position.x > FONT_CHARACTER_CACHE_BITMAP_WIDTH) {
        font_cache->position.x = 0;
        font_cache->position.y = font_cache->position.y + font_cache->maximum_character_height;
        font_cache->maximum_character_height = 0;
    }

    if(bitmap_height + font_cache->position.y > FONT_CHARACTER_CACHE_BITMAP_HEIGHT) {
        font_cache->position.y = 0;
        font_cache->position.x = 0;
        font_cache->maximum_character_height = 0;
        BitmapFontCacheCharacter *character_cache = &font_cache->characters[font_cache->read_index];
        while(font_cache->read_index != font_cache->write_index && character_cache->position.y > 0) {
            rasterizer_bitmap_font_dispose_font_character(character_cache);
            font_cache->read_index = (font_cache->read_index + 1) % MAX_FONT_CACHE_CHARACTERS;
            character_cache = &font_cache->characters[font_cache->read_index];
        }
    }

    if(font_cache->maximum_character_height <= bitmap_height) {
        uint16_t character_baseline_y = font_cache->position.y + font_cache->maximum_character_height;
        uint16_t character_bottom_y = font_cache->position.y + bitmap_height;
        BitmapFontCacheCharacter *character_cache = &font_cache->characters[font_cache->read_index];
        while(font_cache->read_index != font_cache->write_index && character_cache->position.y >= character_baseline_y && character_cache->position.y < character_bottom_y) {
            rasterizer_bitmap_font_dispose_font_character(character_cache);
            font_cache->read_index = (font_cache->read_index + 1) % MAX_FONT_CACHE_CHARACTERS;
            character_cache = &font_cache->characters[font_cache->read_index];
        }
        font_cache->maximum_character_height = bitmap_height;
    }

    if(font_cache->read_index == font_cache->write_index + 1) {
        rasterizer_bitmap_font_dispose_font_character(&font_cache->characters[font_cache->read_index]);
        font_cache->read_index = (font_cache->read_index + 1) % MAX_FONT_CACHE_CHARACTERS;
    }
    
    BitmapFontCacheCharacter *character_cache = &font_cache->characters[font_cache->write_index];
    character_cache->character = character;
    character_cache->position.x = font_cache->position.x;
    character_cache->position.y = font_cache->position.y;
    character->hardware_character_index = font_cache->write_index;

    uint8_t *character_pixels = font->pixels.pointer + character->pixels_offset;
    size_t pixels_count = 0;
    for(size_t offset_y = 0; offset_y < bitmap_height; offset_y++) {
        ColorARGBInt *pixel = bitmap_address_for_pixel(font_cache->position.x, font_cache->position.y + offset_y, 0, font_cache->bitmap);
        for(size_t offset_x = 0; offset_x < bitmap_width; offset_x++) {
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
    font_cache->position.x = font_cache->position.x + bitmap_width;
    font_cache->write_index = (font_cache->write_index + 1) % MAX_FONT_CACHE_CHARACTERS;
}

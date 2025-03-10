#include <stdio.h>
#include "../bitmap/bitmap.h"
#include "../game/game_globals.h"
#include "../math/color.h"
#include "../tag/definitions/bitmap.h"
#include "../player/player.h"
#include "../render/render.h"
#include "rasterizer.h"
#include "rasterizer_dx9_render_target.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9_vertex_shader_constants.h"
#include "rasterizer_dx9_vertex_shader.h"
#include "rasterizer_screen_geometry.h"
#include "rasterizer_dx9.h"

extern bool *motion_sensor_blip_being_rendered;
extern bool *motion_sensor_blip_render_enabled;

void rasterizer_hud_motion_sensor_blip_begin(void) {
    RasterizerGlobals *rasterizer_globals = rasterizer_get_globals();
    Globals *game_globals = game_globals_get();
    if(game_globals == NULL || game_globals->interface_bitmaps.count == 0) {
        return;
    }

    GlobalsInterfaceBitmaps *interface_bitmaps = &game_globals->interface_bitmaps.elements[0];
    BitmapData *motion_sensor_blip_bitmap = bitmap_get_data(interface_bitmaps->motion_sensor_blip_bitmap.tag_handle, 0);
    BitmapData *goo_map1 = bitmap_get_data(interface_bitmaps->interface_goo_map_1.tag_handle, 0);

    if(motion_sensor_blip_bitmap == NULL || goo_map1 == NULL) {
        return;
    }

    *motion_sensor_blip_being_rendered = false;
    if(!rasterizer_globals->render_targets_disabled && *motion_sensor_blip_render_enabled) {
        void *bitmap_loaded = bitmap_load(false, true, motion_sensor_blip_bitmap);
        if(bitmap_loaded == NULL) {
            return;
        }

        bitmap_loaded = bitmap_load(false, true, goo_map1);
        if(bitmap_loaded == NULL) {
            return;
        }

        *motion_sensor_blip_being_rendered = true;

        rasterizer_dx9_render_target_set(0, true, RENDER_TARGET_MOTION_SENSOR);
        rasterizer_dx9_texture_set_bitmap_data_directly(0, motion_sensor_blip_bitmap);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_CCW);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
        rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_ONE);
        rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_ONE);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
        rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_FALSE);
        rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

        uint32_t vertex_processing_method = rasterizer_dx9_vertex_get_processing_method(VERTEX_DECLARATION_UNLIT);
        IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader(VSH_SCREEN);
        rasterizer_dx9_vertex_get_declaration(VERTEX_DECLARATION_DYNAMIC_SCREEN);
        rasterizer_dx9_set_software_vertex_processing(vertex_processing_method & D3DUSAGE_SOFTWAREPROCESSING != 0);
        rasterizer_dx9_set_vertex_shader(vertex_shader);
        rasterizer_dx9_set_pixel_shader(NULL);

        VertexShaderScreenprojConstants screenproj;
        screenproj.projection.x[0] = 1.0f;
        screenproj.projection.y[0] = 0.0f;
        screenproj.projection.z[0] = 0.0f;
        screenproj.projection.w[0] = 0.0f;
        screenproj.projection.x[1] = 0.0f;
        screenproj.projection.y[1] = 1.0f;
        screenproj.projection.z[1] = 0.0f;
        screenproj.projection.w[1] = 0.0f;
        screenproj.projection.x[2] = 0.0f;
        screenproj.projection.y[2] = 0.0f;
        screenproj.projection.z[2] = 1.0f;
        screenproj.projection.w[2] = 0.0f;
        screenproj.projection.x[3] = 0.0f;
        screenproj.projection.y[3] = 0.0f;
        screenproj.projection.z[3] = 0.0f;
        screenproj.projection.w[3] = 1.0f;
        screenproj.texture_scale.x = 1.0f;
        screenproj.texture_scale.y = 1.0f;

        rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_SCREENPROJ_OFFSET, &screenproj, VSH_CONSTANTS_SCREENPROJ_COUNT);
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

void rasterizer_hud_motion_sensor_blip_draw(float opacity, float blip_size, VectorXY *position, ColorRGB *color_tint) {
    if(*motion_sensor_blip_render_enabled && *motion_sensor_blip_being_rendered) {
        float inv_screen_width = 2.0f / render_get_screen_width();
        float half_blip_size = blip_size * (inv_screen_width * 20.0f);
        float blip_center_x = position->x * (inv_screen_width * -10.0f);
        float blip_center_y = position->y * (inv_screen_width * -10.0f);

        ColorRGB final_color;
        final_color.r = color_tint->r * opacity;
        final_color.g = color_tint->g * opacity;
        final_color.b = color_tint->b * opacity;
        uint32_t color_opaque = color_encode_r8g8b8(&final_color);
        
        RasterizerDynamicVertex vertices[4];
        vertices[0].position.x = blip_center_x - half_blip_size;
        vertices[0].position.y = blip_center_y + half_blip_size;
        vertices[0].position.z = 0.0f;
        vertices[0].texture_pos.x = 0.0f;
        vertices[0].texture_pos.y = 0.0f;
        vertices[0].color = color_opaque;
        vertices[1].position.x = blip_center_x + half_blip_size;
        vertices[1].position.y = blip_center_y + half_blip_size;
        vertices[1].position.z = 0.0f;
        vertices[1].texture_pos.x = 1.0f;
        vertices[1].texture_pos.y = 0.0f;
        vertices[1].color = color_opaque;
        vertices[2].position.x = blip_center_x + half_blip_size;
        vertices[2].position.y = blip_center_y - half_blip_size;
        vertices[2].position.z = 0.0f;
        vertices[2].texture_pos.x = 1.0f;
        vertices[2].texture_pos.y = 1.0f;
        vertices[2].color = color_opaque;
        vertices[3].position.x = blip_center_x - half_blip_size;
        vertices[3].position.y = blip_center_y - half_blip_size;
        vertices[3].position.z = 0.0f;
        vertices[3].texture_pos.x = 0.0f;
        vertices[3].texture_pos.y = 1.0f;
        vertices[3].color = color_opaque;

        rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(RasterizerDynamicVertex));
    }
}

void rasterizer_hud_motion_sensor_blip_end(float sweep_progress, VectorXY *offset) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    RasterizerGlobals *rasterizer_globals = rasterizer_get_globals();
    Globals *game_globals = game_globals_get();
    PlayersGlobals *players_globals = player_get_globals();

    if(game_globals == NULL || game_globals->interface_bitmaps.count == 0) {
        return;
    }

    if(!*motion_sensor_blip_render_enabled) {
        return;
    }

    GlobalsInterfaceBitmaps *interface_bitmaps = &game_globals->interface_bitmaps.elements[0];
    BitmapData *motion_sensor_sweep_bitmap = bitmap_get_data(interface_bitmaps->motion_sensor_sweep_bitmap.tag_handle, 0);
    BitmapData *motion_sensor_sweep_bitmap_mask = bitmap_get_data(interface_bitmaps->motion_sensor_sweep_bitmap_mask.tag_handle, 0);

    if(motion_sensor_sweep_bitmap == NULL || motion_sensor_sweep_bitmap_mask == NULL) {
        return;
    }

    if(*motion_sensor_blip_being_rendered) {
        void *bitmap_loaded = bitmap_load(false, true, motion_sensor_sweep_bitmap);
        if(bitmap_loaded == NULL) {
            rasterizer_dx9_render_target_set(0, 0, window_parameters->render_target);
            return;
        }

        bitmap_loaded = bitmap_load(false, true, motion_sensor_sweep_bitmap_mask);
        if(bitmap_loaded == NULL) {
            rasterizer_dx9_render_target_set(0, 0, window_parameters->render_target);
            return;
        }

        VertexShaderScreenprojConstants screenproj;
        RasterizerDynamicVertex vertices[4];
        ColorARGBInt color;

        Rectangle2D *window_bounds = &window_parameters->camera.window_bounds;
        float window_width_inverse = 1.0f / (window_bounds->right - window_bounds->left);
        float window_height_inverse = 1.0f / (window_bounds->top - window_bounds->bottom);
        float screen_width_inverse = 1.0f / render_get_screen_width();
        float screen_height_inverse = 1.0f / render_get_screen_height();

        rasterizer_dx9_texture_set_bitmap_data_directly(0, motion_sensor_sweep_bitmap);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
        rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_CCW);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
        rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_ONE);
        rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
        rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_FALSE);
        rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

        screenproj.projection.x[0] = 1.0f;
        screenproj.projection.y[0] = 0.0f;
        screenproj.projection.z[0] = 0.0f;
        screenproj.projection.w[0] = 0.0f;
        screenproj.projection.x[1] = 0.0f;
        screenproj.projection.y[1] = 1.0f;
        screenproj.projection.z[1] = 0.0f;
        screenproj.projection.w[1] = 0.0f;
        screenproj.projection.x[2] = 0.0f;
        screenproj.projection.y[2] = 0.0f;
        screenproj.projection.z[2] = 1.0f;
        screenproj.projection.w[2] = 0.0f;
        screenproj.projection.x[3] = 0.0f;
        screenproj.projection.y[3] = 0.0f;
        screenproj.projection.z[3] = 0.0f;
        screenproj.projection.w[3] = 1.0f;
        screenproj.texture_scale.x = 1.0f;
        screenproj.texture_scale.y = 1.0f;

        rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_SCREENPROJ_OFFSET, &screenproj, VSH_CONSTANTS_SCREENPROJ_COUNT);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        rasterizer_dx9_set_texture_stage_state(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        rasterizer_dx9_set_texture_stage_state(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        if(sweep_progress <= 2.75) {
            color = 0xFF74B9FF;
            vertices[0].position.x = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
            vertices[0].position.y = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
            vertices[0].position.z = 0.0f;
            vertices[0].texture_pos.x = sweep_progress * 0.5f + 0.5f;
            vertices[0].texture_pos.y = sweep_progress * -0.5f + 0.5f;
            vertices[0].color = color;
            vertices[1].position.x = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
            vertices[1].position.y = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
            vertices[1].position.z = 0.0f;
            vertices[1].texture_pos.x = sweep_progress * -0.5f + 0.5f;
            vertices[1].texture_pos.y = sweep_progress * -0.5f + 0.5f;
            vertices[1].color = color;
            vertices[2].position.x = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
            vertices[2].position.y = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
            vertices[2].position.z = 0.0f;
            vertices[2].texture_pos.x = sweep_progress * -0.5f + 0.5f;
            vertices[2].texture_pos.y = sweep_progress * 0.5f + 0.5f;
            vertices[2].color = color;
            vertices[3].position.x = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
            vertices[3].position.y = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
            vertices[3].position.z = 0.0f;
            vertices[3].texture_pos.x = sweep_progress * 0.5f + 0.5f;
            vertices[3].texture_pos.y = sweep_progress * 0.5f + 0.5f;
            vertices[3].color = color;
            rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(RasterizerDynamicVertex));
        }

        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        rasterizer_dx9_texture_set_bitmap_data_directly(0, motion_sensor_sweep_bitmap_mask);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
        rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
        rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        rasterizer_dx9_set_texture_stage_state(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        rasterizer_dx9_set_texture_stage_state(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        color = 0xFF66CC66;
        vertices[0].position.x = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
        vertices[0].position.y = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
        vertices[0].position.z = 0.0f;
        vertices[0].texture_pos.x = 1.0f;
        vertices[0].texture_pos.y = 0.0f;
        vertices[0].color = color;
        vertices[1].position.x = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
        vertices[1].position.y = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
        vertices[1].position.z = 0.0f;
        vertices[1].texture_pos.x = 0.0f;
        vertices[1].texture_pos.y = 0.0f;
        vertices[1].color = color;
        vertices[2].position.x = screen_width_inverse + (screen_width_inverse * 2.0f + 1.0f);
        vertices[2].position.y = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
        vertices[2].position.z = 0.0f;
        vertices[2].texture_pos.x = 0.0f;
        vertices[2].texture_pos.y = 1.0f;
        vertices[2].color = color;
        vertices[3].position.x = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
        vertices[3].position.y = screen_width_inverse - (screen_width_inverse * 2.0f + 1.0f);
        vertices[3].position.z = 0.0f;
        vertices[3].texture_pos.x = 1.0f;
        vertices[3].texture_pos.y = 1.0f;
        vertices[3].color = color;
        rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(RasterizerDynamicVertex));

        rasterizer_dx9_render_target_set(0, false, window_parameters->render_target);
        IDirect3DTexture9 *texture = rasterizer_dx9_render_target_get_texture(RENDER_TARGET_MOTION_SENSOR);
        rasterizer_dx9_set_texture(0, texture);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_CCW);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
        rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_ONE);
        rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_ONE);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
        rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_FALSE);
        rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

        screenproj.projection.x[0] = screen_width_inverse * 2.0f;
        screenproj.projection.y[0] = 0.0f;
        screenproj.projection.z[0] = 0.0f;
        screenproj.projection.w[0] = 0.0f;
        screenproj.projection.x[1] = 0.0f;
        screenproj.projection.y[1] = (screen_height_inverse * 2.0f) * -1.0f;
        screenproj.projection.z[1] = 0.0f;
        screenproj.projection.w[1] = 0.0f;
        screenproj.projection.x[2] = 0.0f;
        screenproj.projection.y[2] = 0.0f;
        screenproj.projection.z[2] = 0.0f;
        screenproj.projection.w[2] = 0.0f;
        screenproj.projection.x[3] = (1.0f + window_width_inverse) * -1.0f;
        screenproj.projection.y[3] = 1.0f + window_height_inverse;
        screenproj.projection.z[3] = 0.5f;
        screenproj.projection.w[3] = 1.0f;
        screenproj.texture_scale.x = 1.0f;
        screenproj.texture_scale.y = 1.0f;
        rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_SCREENPROJ_OFFSET, &screenproj, VSH_CONSTANTS_SCREENPROJ_COUNT);

        float sweep_size = 42.0f;
        if(players_globals->local_player_count >= 2) {
            sweep_size = 32.0f;
        }

        vertices[0].position.x = offset->x - sweep_size;
        vertices[0].position.y = offset->y - sweep_size;
        vertices[0].position.z = 0.0;
        vertices[0].texture_pos.x = 0.0;
        vertices[0].texture_pos.y = 0.0;
        vertices[0].color = 0xFFFFFFFF;

        vertices[1].position.x = sweep_size + offset->x;
        vertices[1].position.y = vertices[0].position.y;
        vertices[1].position.z = 0.0;
        vertices[1].texture_pos.x = 1.0;
        vertices[1].texture_pos.y = 0.0;
        vertices[1].color = 0xFFFFFFFF;
        
        vertices[2].position.x = vertices[1].position.x;
        vertices[2].position.y = sweep_size + offset->y;
        vertices[2].position.z = 0.0;
        vertices[2].texture_pos.x = 1.0;
        vertices[2].texture_pos.y = 1.0;
        vertices[2].color = 0xFFFFFFFF;

        vertices[3].position.x = vertices[0].position.x;
        vertices[3].position.y = vertices[2].position.y;
        vertices[3].position.z = 0.0;
        vertices[3].texture_pos.x = 0.0;
        vertices[3].texture_pos.y = 1.0;
        vertices[3].color = 0xFFFFFFFF;

        rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(RasterizerDynamicVertex));
        rasterizer_dx9_set_software_vertex_processing(rasterizer_globals->using_software_vertex_processing);
    }
}

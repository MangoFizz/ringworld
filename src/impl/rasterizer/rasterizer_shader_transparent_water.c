#include <stdio.h>
#include <math.h>

#include "../exception/exception.h"
#include "../shader/shader.h"
#include "../math/math.h"
#include "../render/render.h"
#include "rasterizer.h"
#include "rasterizer_dx9_render_target.h"
#include "rasterizer_dx9_shader_effect.h"
#include "rasterizer_dx9_vertex_shader.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_shader_transparent_water.h"

extern bool *shader_transparent_water_unk1;
extern bool *shader_transparent_water_should_update_bumpmap;
bool shader_transparent_water_enabled = true;

void rasterizer_shader_transparent_water_render_bumpmap(ShaderTransparentWater *shader) {
    ASSERT(shader != NULL);

    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);

    if(shader_transparent_water_enabled) {
        RasterizerDx9ShaderEffect *water_bumpmap_effect = rasterizer_dx9_shader_effect_get(SHADER_EFFECT_TRANSPARENT_WATER_BUMPMAP_CONVOLUTION);
        if(water_bumpmap_effect != NULL && water_bumpmap_effect->pixel_shaders != NULL) {
            RasterizerDynamicVertex vertices[4];

            vertices[0].position.x = -1.007813f;
            vertices[0].position.y = 1.007813f;
            vertices[0].position.z = 0.0f;
            vertices[0].texture_pos.x = 0.0f;
            vertices[0].texture_pos.y = 0.0f;
            vertices[0].color = D3DCOLOR_ARGB(255, 255, 255, 255);
            
            vertices[1].position.x = 0.9921875f;
            vertices[1].position.y = 1.007813f;
            vertices[1].position.z = 0.0f;
            vertices[1].texture_pos.x = 1.0f;
            vertices[1].texture_pos.y = 0.0f;
            vertices[1].color = D3DCOLOR_ARGB(255, 255, 255, 255);
            
            vertices[2].position.x = 0.9921875f;
            vertices[2].position.y = -0.9921875f;
            vertices[2].position.z = 0.0f;
            vertices[2].texture_pos.x = 1.0f;
            vertices[2].texture_pos.y = 1.0f;
            vertices[2].color = D3DCOLOR_ARGB(255, 255, 255, 255);
            
            vertices[3].position.x = -1.007813f;
            vertices[3].position.y = -0.9921875f;
            vertices[3].position.z = 0.0f;
            vertices[3].texture_pos.x = 0.0f;
            vertices[3].texture_pos.y = 1.0f;
            vertices[3].color = D3DCOLOR_ARGB(255, 255, 255, 255);

            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_CCW);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
            rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_FALSE);
            rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

            rasterizer_dx9_set_vertex_declaration(VERTEX_DECLARATION_DYNAMIC_SCREEN);

            uint32_t vertex_processing_method = rasterizer_dx9_vertex_get_processing_method(VERTEX_DECLARATION_DYNAMIC_SCREEN);
            bool software_processing = vertex_processing_method & D3DUSAGE_SOFTWAREPROCESSING != 0;
            IDirect3DDevice9_SetSoftwareVertexProcessing(device, software_processing);

            IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader(VSH_CONVOLUTION);
            rasterizer_dx9_set_vertex_shader(vertex_shader);

            rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

            rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

            rasterizer_dx9_set_sampler_state(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

            ShaderTransparentWaterRipple ripples[4];
            for(size_t i = 0; i < 4; i++) {
                if(i < shader->ripples.count) {
                    ShaderTransparentWaterRipple *ripple = TAG_BLOCK_GET_ELEMENT(shader->ripples, i);
                    ripples[i] = *ripple;
                }
                else {
                    memset(&ripples[i], 0, sizeof(ShaderTransparentWaterRipple));
                    ripples[i].map_repeats = 1;
                }
            }

            if(nan_f32(ripples[0].contribution_factor) || ripples[0].contribution_factor == 0.0f) {
                if(nan_f32(ripples[1].contribution_factor) || ripples[1].contribution_factor == 0.0f) {
                    ripples[1].contribution_factor = 1.0f;
                }
            }
            if(nan_f32(ripples[2].contribution_factor) || ripples[2].contribution_factor == 0.0f) {
                if(nan_f32(ripples[3].contribution_factor) || ripples[3].contribution_factor == 0.0f) {
                    ripples[3].contribution_factor = 1.0f;
                }
            }

            FrameParameters *frame_parameters = render_get_frame_parameters();
            float vs_constants[4 * 8] = {0};
            for(size_t i = 0; i < 4; i++) {
                float cos = cosf(ripples[i].animation_angle);
                float sin = sinf(ripples[i].animation_angle);
                vs_constants[i * 8 + 0] = ripples[i].map_repeats;
                vs_constants[i * 8 + 1] = 0.0f;
                vs_constants[i * 8 + 2] = 0.0f;
                vs_constants[i * 8 + 3] = cos * ripples[i].animation_velocity * frame_parameters->elapsed_time_sec + ripples[i].map_offset.i;
                vs_constants[i * 8 + 4] = 0.0f;
                vs_constants[i * 8 + 5] = ripples[i].map_repeats;
                vs_constants[i * 8 + 6] = 0.0f;
                vs_constants[i * 8 + 7] = sin * ripples[i].animation_velocity * frame_parameters->elapsed_time_sec + ripples[i].map_offset.j;
            }

            rasterizer_dx9_set_vertex_shader_constant_f(13, vs_constants, 8);

            float ps_constants[4 * 4];
            ps_constants[0] = 0.0f;
            ps_constants[1] = 0.0f;
            ps_constants[2] = 0.0f;
            ps_constants[3] = ripples[0].contribution_factor / (ripples[0].contribution_factor + ripples[1].contribution_factor);
            ps_constants[4] = 0.0f;
            ps_constants[5] = 0.0f;
            ps_constants[6] = 0.0f;
            ps_constants[8] = 0.0f;
            ps_constants[7] = ripples[2].contribution_factor / (ripples[2].contribution_factor + ripples[3].contribution_factor);
            ps_constants[9] = 0.0f;
            ps_constants[10] = 0.0f;
            ps_constants[11] = (ripples[0].contribution_factor + ripples[1].contribution_factor) / (ripples[0].contribution_factor + 
                                ripples[1].contribution_factor + ripples[2].contribution_factor + ripples[3].contribution_factor);
            ps_constants[12] = 0.0f;
            ps_constants[13] = 0.0f;
            ps_constants[14] = 0.0f;
            ps_constants[15] = 0.0f;

            rasterizer_dx9_render_target_set(0x00000000, 0, 8);
            rasterizer_dx9_set_stencil_mode(0);

            size_t mipmap_levels = min_i32(shader->ripple_mipmap_levels, 4);

            for(size_t i = 0; i < mipmap_levels; i++) {
                ps_constants[12] = 0.5f;
                ps_constants[13] = 0.5f;
                ps_constants[14] = 1.0f;
                
                if(shader->ripple_mipmap_levels < 2) {
                    ps_constants[15] = 0.0f;
                }
                else {
                    float alpha = (float)i / (float)(shader->ripple_mipmap_levels - 1);
                    ps_constants[15] = alpha * shader->ripple_mipmap_fade_factor;
                }

                for(size_t j = 0; j < 4; j++) {
                    TagHandle ripple_map_handle = NULL_HANDLE;
                    if(j < shader->ripples.count) {
                        ripple_map_handle = shader->ripple_maps.tag_handle;
                    }
                    rasterizer_dx9_texture_set_bitmap_data_texture_no_assert(j, ripples[j].map_index, ripple_map_handle);
                }

                rasterizer_dx9_set_pixel_shader(water_bumpmap_effect->pixel_shaders[0].pixel_shader);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                rasterizer_dx9_set_pixel_shader_constant_f(0, ps_constants, 4);
                HRESULT res = IDirect3DDevice9_DrawPrimitiveUP(device, D3DPT_TRIANGLEFAN, 2, vertices, sizeof(RasterizerDynamicVertex));
                if(FAILED(res)) {
                    CRASHF_DEBUG("failed to draw shader transparent water bumpmap");
                }
            }

            IDirect3DDevice9_SetSoftwareVertexProcessing(device, rasterizer_dx9_device_supports_software_vertex_processing());
        }

        rasterizer_dx9_render_target_set(0x00000000, 0, rasterizer_dx9_render_target_current_index());
        rasterizer_dx9_set_stencil_mode(2);
    }
}

void rasterizer_shader_transparent_water_draw(TransparentGeometryGroup *group) {
    ASSERT(group != NULL);

    if(*shader_transparent_water_unk1 == false && shader_transparent_water_enabled) {
        ShaderTransparentWater *shader = shader_type_assert(group->shader, SHADER_TYPE_SHADER_TRANSPARENT_WATER);
        GlobalsRasterizerData *globals_rasterizer_data = render_get_globals_rasterizer_data();
        
        int vertex_buffer_type = rasterizer_geometry_group_get_vertex_buffer_type(group);
        bool use_m_vertex_shader = false;
        if(vertex_buffer_type != VERTEX_BUFFER_TYPE_ENVIRONMENT_UNCOMPRESSED && vertex_buffer_type != VERTEX_BUFFER_TYPE_ENVIRONMENT_LIGHTMAP_UNCOMPRESSED && vertex_buffer_type == VERTEX_BUFFER_TYPE_MODEL_UNCOMPRESSED) {
            use_m_vertex_shader = true;
        }

        if(shader->water_flags.draw_before_fog && group->geometry_flags.no_queue == false && group->geometry_flags.sky == false) {
            IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader(use_m_vertex_shader ? VSH_TRANSPARENT_WATER_OPACITY_M : VSH_TRANSPARENT_WATER_OPACITY);

            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_TRUE);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
            rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);
            rasterizer_dx9_set_vertex_declaration(vertex_buffer_type);
            rasterizer_dx9_set_vertex_shader(vertex_shader);

            rasterizer_dx9_set_pixel_shader(NULL);
            rasterizer_dx9_set_render_state(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
            rasterizer_dx9_set_texture_stage_state(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            rasterizer_dx9_set_texture_stage_state(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

            rasterizer_transparent_geometry_group_draw_vertices(false, group);
            return;
        }

        bool z_write_enable = false;
        if(group->geometry_flags.sky == false && shader->water_flags.draw_before_fog == false) {
            z_write_enable = true;
        }

        if(*shader_transparent_water_should_update_bumpmap) {
            rasterizer_shader_transparent_water_render_bumpmap(shader);
            *shader_transparent_water_should_update_bumpmap = false;
        }

        RasterizerDx9ShaderEffect *water_opacity_effect = rasterizer_dx9_shader_effect_get(SHADER_EFFECT_TRANSPARENT_WATER_OPACITY);
        if(water_opacity_effect != NULL) {
            float view_perpendicular_brightness = shader->view_perpendicular_brightness;
            float view_parallel_brightness = shader->view_parallel_brightness;
            float ps_constants[8] = {0};
            ps_constants[0] = view_perpendicular_brightness;
            ps_constants[1] = ps_constants[0];
            ps_constants[2] = ps_constants[0];
            ps_constants[3] = ps_constants[0];
            ps_constants[4] = view_parallel_brightness;
            ps_constants[5] = ps_constants[4];
            ps_constants[6] = ps_constants[4];
            ps_constants[7] = ps_constants[4];

            rasterizer_dx9_set_vertex_declaration(vertex_buffer_type);
            IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader(use_m_vertex_shader ? VSH_TRANSPARENT_WATER_OPACITY_M : VSH_TRANSPARENT_WATER_OPACITY);
            rasterizer_dx9_set_vertex_shader(vertex_shader);

            if(shader->water_flags.base_map_alpha_modulates_reflection) {
                rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
                rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
                rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, FALSE);
                rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
                rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_TRUE);
                rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
                rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, z_write_enable);
                rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

                rasterizer_dx9_texture_set_bitmap_data_texture(0, BITMAP_TYPE_2D_TEXTURES, BITMAP_USAGE_DEFAULT, group->shader_permutation_index, shader->base_map.tag_handle);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                
                rasterizer_dx9_texture_set_bitmap_data_texture_no_assert(1, 0, globals_rasterizer_data->vector_normalization.tag_handle);
                rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

                rasterizer_dx9_set_pixel_shader(water_opacity_effect->pixel_shaders[0].pixel_shader);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                rasterizer_dx9_set_pixel_shader_constant_f(0, ps_constants, 2);
                rasterizer_transparent_geometry_group_draw_vertices(false, group);
            }

            if(shader->water_flags.base_map_color_modulates_background) {
                rasterizer_dx9_texture_set_bitmap_data_texture(0, BITMAP_TYPE_2D_TEXTURES, BITMAP_USAGE_DEFAULT, group->shader_permutation_index, shader->base_map.tag_handle);
                
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

                rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
                rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
                rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
                rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_ZERO);
                rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
                rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
                rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
                rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_TRUE);
                rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
                rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, z_write_enable);
                rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, shader->water_flags.atmospheric_fog != 0);

                rasterizer_dx9_set_pixel_shader(water_opacity_effect->pixel_shaders[1].pixel_shader);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                rasterizer_dx9_set_pixel_shader_constant_f(0, ps_constants, 2);
                rasterizer_transparent_geometry_group_draw_vertices(false, group);
            }
        }

        water_opacity_effect = rasterizer_dx9_shader_effect_get(SHADER_EFFECT_TRANSPARENT_WATER_REFLECTION);
        if(water_opacity_effect != NULL) {
            FrameParameters *frame_params = render_get_frame_parameters();
            float ripple_scale = shader->ripple_scale;
            float vs_constants[12] = {0};
            vs_constants[0] = ripple_scale;
            vs_constants[1] = ripple_scale;
            vs_constants[2] = cos(shader->ripple_animation_angle) * shader->ripple_animation_velocity * frame_params->elapsed_time_sec;
            vs_constants[3] = sin(shader->ripple_animation_angle) * shader->ripple_animation_velocity * frame_params->elapsed_time_sec;
            vs_constants[4] = 0.0f;
            vs_constants[5] = 0.0f;
            vs_constants[6] = 0.0f;
            vs_constants[7] = 0.0f;
            vs_constants[8] = 0.0f;
            vs_constants[9] = 0.0f;
            vs_constants[10] = 0.0f;
            vs_constants[11] = 0.0f;

            IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader(use_m_vertex_shader ? VSH_TRANSPARENT_WATER_REFLECTION_M : VSH_TRANSPARENT_WATER_REFLECTION);
            rasterizer_dx9_set_vertex_declaration(vertex_buffer_type);
            rasterizer_dx9_set_vertex_shader(vertex_shader);
            rasterizer_dx9_set_vertex_shader_constant_f(10, vs_constants, 3);

            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
            rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, group->geometry_flags.sky == false);
            rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, shader->water_flags.base_map_alpha_modulates_reflection ? D3DBLEND_DESTALPHA : D3DBLEND_ONE);
            rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_ONE);
            rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DZB_TRUE);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
            rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, z_write_enable);
            rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, shader->water_flags.atmospheric_fog);

            D3DCAPS9 *caps = rasterizer_dx9_device_caps();
            if(caps->PixelShaderVersion < D3DPS_VERSION(1, 1)) {
                rasterizer_dx9_texture_set_bitmap_data_texture_no_assert(0, 0, globals_rasterizer_data->test_0.tag_handle);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                rasterizer_transparent_geometry_group_draw_vertices(false, group);
                return;
            }

            IDirect3DTexture9 *render_target_texture = rasterizer_dx9_render_target_get_texture(8);
            rasterizer_dx9_set_texture(0, render_target_texture);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

            rasterizer_dx9_texture_set_bitmap_data_texture(3, BITMAP_TYPE_CUBE_MAPS, BITMAP_USAGE_ALPHA_BLEND, group->shader_permutation_index, shader->reflection_map.tag_handle);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            rasterizer_dx9_set_sampler_state(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

            float ps_constants[4] = {0};
            RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
            Plane3D *plane = &group->plane;
            double norm = math_vector_normalize((VectorIJK *)plane);
            if(norm > 0.0) {
                double dot_product = math_vector_dot_product((VectorIJK *)plane, (VectorIJK *)&window_parameters->camera.forward);
                if(dot_product >= 0.0) {
                    if((dot_product * -1.0) <= 1.0) {
                        dot_product = dot_product * -1.0;
                    }
                    else {
                        dot_product = 1.0;
                    }
                }
                else {
                    dot_product = 0.0f;
                }
                ps_constants[0] = shader->view_perpendicular_tint_color.r * dot_product + (1.0f - dot_product) * shader->view_parallel_tint_color.r;
                ps_constants[1] = shader->view_perpendicular_tint_color.g * dot_product + (1.0f - dot_product) * shader->view_parallel_tint_color.g;
                ps_constants[2] = shader->view_perpendicular_tint_color.b * dot_product + (1.0f - dot_product) * shader->view_parallel_tint_color.b;
            }
            else {
                ps_constants[0] = 1.0f;
                ps_constants[1] = 1.0f;
                ps_constants[2] = 1.0f;
            }
            ps_constants[3] = 0.0f;

            RasterizerDx9ShaderEffect *water_reflection_effect = rasterizer_dx9_shader_effect_get(SHADER_EFFECT_TRANSPARENT_WATER_REFLECTION);
            rasterizer_dx9_set_pixel_shader(water_reflection_effect->pixel_shaders[0].pixel_shader);
            rasterizer_dx9_set_pixel_shader_constant_f(0, ps_constants, 1);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            rasterizer_transparent_geometry_group_draw_vertices(false, group);
        }
    }
}

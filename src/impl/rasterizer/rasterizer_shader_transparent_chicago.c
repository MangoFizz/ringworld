#include <math.h>
#include <stdio.h>

#include "../debug/assertion.h"
#include "../exception/exception.h"
#include "../shader/shader.h"
#include "../tag/definitions/shader_transparent_chicago.h"
#include "../terminal/terminal.h"
#include "../interface/numeric_countdown.h"
#include "../math/math.h"
#include "../render/render.h"
#include "rasterizer.h"
#include "rasterizer_geometry_group.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_shader_effect.h"
#include "rasterizer_dx9_vertex_shader.h"
#include "rasterizer_dx9_vertex_shader_constants.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_shader_transparent_chicago.h"

extern float *shader_transparent_generic_vertex_constants;
extern bool *af_is_enabled;

void rasterizer_shader_transparent_chicago_draw(TransparentGeometryGroup *group, uint32_t *param_2) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    ShaderTransparentChicago *shader_data = shader_type_assert(group->shader, SHADER_TYPE_PC_TRANSPARENT_CHICAGO);

    for(int i = 0; i < shader_data->properties.extra_layers.count; i++) {
        TransparentGeometryGroup extra_layer_group = {};
        memcpy(&extra_layer_group, group, sizeof(TransparentGeometryGroup));
        ShaderTransparentExtraLayer *layer = TAG_BLOCK_GET_ELEMENT(shader_data->properties.extra_layers, i);
        extra_layer_group.shader = tag_get_data(TAG_GROUP_SHADER, layer->shader.tag_handle);
        extra_layer_group.sorted_index = -1;
        rasterizer_transparent_geometry_group_draw(&extra_layer_group, param_2);
    }

    uint16_t vertex_permutation = shader_get_vertex_shader_permutation(group->shader);
    uint16_t vertex_buffer_type = rasterizer_geometry_group_get_vertex_buffer_type(group);
    IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader_for_permutation(vertex_permutation, vertex_buffer_type);
    rasterizer_dx9_set_vertex_shader(vertex_shader);
    rasterizer_dx9_set_vertex_declaration(vertex_buffer_type);
    rasterizer_dx9_set_pixel_shader(NULL);

    rasterizer_dx9_set_render_state(D3DRS_CULLMODE, shader_data->properties.flags.two_sided ? D3DCULL_NONE : D3DCULL_CCW);
    rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, D3DBLENDOP_ADD);
    rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, shader_data->properties.flags.alpha_tested);
    rasterizer_dx9_set_render_state(D3DRS_ALPHAREF, 0x0000007F);
    rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);

    rasterizer_dx9_set_framebuffer_blend_function(shader_data->properties.framebuffer_blend_function);

    uint16_t bitmap_data_index = group->shader_permutation_index;
    if(shader_data->properties.flags.numeric && group->animation != NULL && shader_data->maps.count > 0) {
        ShaderTransparentChicagoMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, 0);
        Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, map->parameters.map.tag_handle);
        uint16_t bitmap_count = bitmap->bitmap_data.count;
        
        if(shader_data->extra_flags.numeric_countdown_timer == false) {
            int numeric_count = shader_data->properties.numeric_counter_limit;
            short index = (bitmap_count != 8) ? 0 : 3;
            float new_val = floor(numeric_count * group->animation->values[index] + 0.5f);
            int res = math_float_to_long(new_val);
            if(res < 0) {
                numeric_count = 0;
            }
            else if(res < numeric_count) {
                numeric_count = res;
            }

            for(int i = 0; i < group->shader_permutation_index; i++) {
                numeric_count = numeric_count / bitmap_count;
            }
            bitmap_data_index = numeric_count % bitmap_count;
        }
        else {
            bitmap_data_index = numeric_countdown_timer_get(group->shader_permutation_index);
        }
    }

    if(shader_data->maps.count > 0) {
        float animation_vsh_constants[4 * 8] = {0};

        for(size_t map_index = 0; map_index < 4; map_index++) {
            if(map_index < shader_data->maps.count) {
                bool is_first_map = map_index == 0;
                ShaderTransparentChicagoMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, map_index);
                ShaderFirstMapType first_map_type = shader_data->properties.first_map_type;
                BitmapType bitmap_type;
                if(is_first_map) {
                    bitmap_type = shader_bitmap_type_for_shader_transparent_generic_first_map(first_map_type);
                }
                else {
                    bitmap_type = BITMAP_TYPE_2D_TEXTURES;
                }

                ASSERT(first_map_type >= 0 && first_map_type < SHADER_FIRST_MAP_TYPE_MAX_VALUE);
                ASSERT(shader_data->base.radiosity.flags.transparent_lit == false || first_map_type == SHADER_FIRST_MAP_TYPE_2D_MAP);

                rasterizer_dx9_texture_set_bitmap_data_texture(map_index, bitmap_type, BITMAP_USAGE_ALPHA_BLEND, bitmap_data_index, map->parameters.map.tag_handle);

                D3DTEXTUREADDRESS fisrt_map_texture_mode;
                switch(bitmap_type) {
                    case BITMAP_TYPE_2D_TEXTURES:
                        fisrt_map_texture_mode = D3DTADDRESS_WRAP;
                        break;
                    case BITMAP_TYPE_CUBE_MAPS:
                        fisrt_map_texture_mode = D3DTADDRESS_CLAMP;
                        break;
                    default:
                        exception_throw_runtime_error("Invalid bitmap type: %d", bitmap_type);
                        break;
                }

                #define SELECT_TEXTURE_MODE_IF_FIRST_MAP(val) \
                    (is_first_map ? fisrt_map_texture_mode : val)

                #define SELECT_TEXTURE_MODE_IF_CLAMPED(is_clamped, clamp_val, default_val) \
                    (bitmap_type == BITMAP_TYPE_2D_TEXTURES && is_clamped ? clamp_val : SELECT_TEXTURE_MODE_IF_FIRST_MAP(default_val))

                D3DTEXTUREADDRESS u_texture_mode = SELECT_TEXTURE_MODE_IF_CLAMPED(map->flags.u_clamped, D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP);
                D3DTEXTUREADDRESS v_texture_mode = SELECT_TEXTURE_MODE_IF_CLAMPED(map->flags.v_clamped, D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP);
                D3DTEXTUREADDRESS w_texture_mode = SELECT_TEXTURE_MODE_IF_FIRST_MAP(D3DTADDRESS_WRAP);

                #undef SELECT_TEXTURE_MODE_IF_CLAMPED
                #undef SELECT_TEXTURE_MODE_IF_FIRST_MAP

                uint32_t filter_type = D3DTEXF_LINEAR;
#ifdef RINGWORLD_ENABLE_ENHANCEMENTS
                D3DCAPS9 *d3d9_device_caps = rasterizer_dx9_device_caps();
                if(*af_is_enabled && (d3d9_device_caps->RasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0 && 1 < d3d9_device_caps->MaxAnisotropy) {
                    rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MAXANISOTROPY, d3d9_device_caps->MaxAnisotropy < 16 ? d3d9_device_caps->MaxAnisotropy : 16);
                    filter_type = D3DTEXF_ANISOTROPIC;
                }
#endif
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSU, u_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSV, v_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSW, w_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MAGFILTER, filter_type);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MINFILTER, map->flags.unfiltered ? D3DTEXF_POINT : filter_type);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MIPFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);

                uint32_t maps_count = shader_data->maps.count;
                if(is_first_map && shader_data->properties.first_map_type != SHADER_FIRST_MAP_TYPE_2D_MAP) {
                    if(shader_data->properties.flags.first_map_is_in_screenspace == false) {
                        animation_vsh_constants[map_index * 8 + 0] = 1.0;
                        animation_vsh_constants[map_index * 8 + 1] = 0.0;
                        animation_vsh_constants[map_index * 8 + 2] = 0.0;
                        animation_vsh_constants[map_index * 8 + 3] = 0.0;
                        animation_vsh_constants[map_index * 8 + 4] = 0.0;
                        animation_vsh_constants[map_index * 8 + 5] = 1.0;
                        animation_vsh_constants[map_index * 8 + 6] = 0.0;
                        animation_vsh_constants[map_index * 8 + 7] = 0.0;
                    } 
                    else {
                        animation_vsh_constants[map_index * 8 + 0] = window_parameters->frustum.view_to_world.forward.i;
                        animation_vsh_constants[map_index * 8 + 1] = window_parameters->frustum.view_to_world.forward.j;
                        animation_vsh_constants[map_index * 8 + 2] = window_parameters->frustum.view_to_world.forward.k;
                        animation_vsh_constants[map_index * 8 + 3] = 0.0;
                        animation_vsh_constants[map_index * 8 + 4] = window_parameters->frustum.view_to_world.left.i;
                        animation_vsh_constants[map_index * 8 + 5] = window_parameters->frustum.view_to_world.left.j;
                        animation_vsh_constants[map_index * 8 + 6] = window_parameters->frustum.view_to_world.left.k;
                        animation_vsh_constants[map_index * 8 + 7] = 0.0;
                    }
                }
                else {
                    float map_v_scale = map->parameters.map_v_scale;
                    float map_u_scale = map->parameters.map_u_scale;

                    if(is_first_map && shader_data->properties.flags.scale_first_map_with_distance) {
                        map_v_scale = map_v_scale * group->z_sort * -1;
                        map_u_scale = map_u_scale * group->z_sort * -1;
                    }

                    if(!is_first_map || shader_data->properties.flags.first_map_is_in_screenspace == false) {
                        map_u_scale = map_u_scale * group->model_base_map_scale.x;
                        map_v_scale = map_v_scale * group->model_base_map_scale.y;
                    }

                    ShaderTransparentMapAnimation *texture_animation = &map->animation;
                    float map_u_offset = map->parameters.map_u_offset;
                    float map_v_offset = map->parameters.map_v_offset;
                    float map_rotation = map->parameters.map_rotation;
                    RasterizerFrameParameters *frame_parameters = rasterizer_get_frame_parameters();
                    shader_texture_animation_evaluate(map_u_scale, map_v_scale, map_u_offset, map_v_offset, map_rotation,
                                                        frame_parameters->elapsed_time_sec, texture_animation, group->animation, 
                                                        &animation_vsh_constants[map_index * 8 + 0],
                                                        &animation_vsh_constants[map_index * 8 + 4]);
                }
            }
            else {
                animation_vsh_constants[map_index * 8 + 0] = 1.0;
                animation_vsh_constants[map_index * 8 + 1] = 0.0;
                animation_vsh_constants[map_index * 8 + 2] = 0.0;
                animation_vsh_constants[map_index * 8 + 3] = 0.0;
                animation_vsh_constants[map_index * 8 + 4] = 0.0;
                animation_vsh_constants[map_index * 8 + 5] = 1.0;
                animation_vsh_constants[map_index * 8 + 6] = 0.0;
                animation_vsh_constants[map_index * 8 + 7] = 0.0;
                rasterizer_dx9_set_texture(map_index, NULL);
            }

        }

        rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_TEXANIM_OFFSET, animation_vsh_constants, VSH_CONSTANTS_TEXANIM_COUNT);
    }
        
    rasterizer_shader_transparent_chicago_preprocess(shader_data);

    if(group->geometry_flags.sky && shader_data->properties.framebuffer_blend_function == FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND) {
        uint16_t maps_count = shader_data->maps.count;
        rasterizer_dx9_set_texture(maps_count, NULL);
        rasterizer_dx9_set_texture_stage_state(maps_count, D3DTSS_COLOROP, D3DTOP_DISABLE);
        rasterizer_dx9_set_texture_stage_state(maps_count, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        rasterizer_transparent_geometry_group_draw_vertices(false, group);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DTOP_DISABLE);
        return;
    }

    float vertex_constants[3 * 4];
    vertex_constants[0] = 0.0f;
    vertex_constants[1] = 0.0f;
    vertex_constants[2] = 0.0f;
    vertex_constants[3] = 0.0f;
    vertex_constants[4] = 0.0f;
    vertex_constants[5] = 0.0f;
    vertex_constants[6] = 0.0f;
    vertex_constants[7] = 0.0f;
    vertex_constants[8] = 0.0f;
    vertex_constants[9] = 0.0f;
    vertex_constants[10] = 1.0f;
    vertex_constants[11] = 0.0f;

    if(group->effect.type == RENDER_MODEL_EFFECT_TYPE_ACTIVE_CAMOUFLAGE && shader_data->extra_flags.dont_fade_active_camouflage == false) {
        vertex_constants[10] = clamp_f32(1.0f - group->effect.intensity, 0.0f, 1.0f);
    }

    if(shader_data->properties.framebuffer_fade_source > 0 && group->animation != NULL && group->animation->values != NULL) {
        vertex_constants[10] *= group->animation->values[shader_data->properties.framebuffer_fade_source - 1];
        ASSERT(!nan_f32(vertex_constants[10])); // check for NaN, just in case
    }

    rasterizer_dx9_set_vertex_shader_constant_f(10, vertex_constants, 3);

    int tss_option_argument;
    switch(shader_data->properties.framebuffer_fade_mode) {
        case FRAMEBUFFER_FADE_MODE_NONE:
            tss_option_argument = D3DTA_ALPHAREPLICATE;
            break;
        case FRAMEBUFFER_FADE_MODE_FADE_WHEN_PERPENDICULAR:
            tss_option_argument = D3DTA_ALPHAREPLICATE | D3DTA_SPECULAR;
            break;
        case FRAMEBUFFER_FADE_MODE_FADE_WHEN_PARALLEL:
            tss_option_argument = D3DTA_SPECULAR;
            break;
        default:
            exception_throw_runtime_error("Unknown framebuffer fade mode: %d", shader_data->properties.framebuffer_fade_mode);
            break;
    }

    D3DCAPS9 *device_caps = rasterizer_dx9_device_caps();
    uint32_t max_simultaneous_textures = device_caps->MaxSimultaneousTextures;

    uint16_t texture_stage;
    switch(shader_data->properties.framebuffer_blend_function) {
        case FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND: {
            if(max_simultaneous_textures == 2 && shader_data->maps.count > 1) {
                texture_stage = shader_data->maps.count - 1;
                if(texture_stage < 2) {
                    texture_stage = 1;
                }
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTA_TFACTOR);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG2, tss_option_argument);
                break;
            }
            texture_stage = shader_data->maps.count;
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG1, D3DTA_CURRENT);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAARG2, tss_option_argument);
            break;
        }

        case FRAMEBUFFER_BLEND_FUNCTION_MULTIPLY:
        case FRAMEBUFFER_BLEND_FUNCTION_COMPONENT_MIN: {
            if(max_simultaneous_textures < 3) {
                texture_stage = shader_data->maps.count - 1;
                if(texture_stage < 2) {
                    texture_stage = 1;
                }
            }
            else {
                texture_stage = shader_data->maps.count;
            }

            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG1, tss_option_argument | D3DTOP_BLENDCURRENTALPHA);
#ifdef RINGWORLD_ENABLE_ENHANCEMENTS
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG2, tss_option_argument);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG0, D3DTA_CURRENT);
#else
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG0, tss_option_argument);
#endif
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
            break;
        }

        case FRAMEBUFFER_BLEND_FUNCTION_DOUBLE_MULTIPLY: {
            if(max_simultaneous_textures < 3) {
                texture_stage = shader_data->maps.count - 1;
                if(texture_stage < 2) {
                    texture_stage = 1;
                }
            }
            else {
                texture_stage = shader_data->maps.count;
            }

            rasterizer_dx9_set_render_state(D3DRS_TEXTUREFACTOR, 0x7F7F7F7F);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTOP_LERP);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG1, tss_option_argument);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG0, D3DTA_TFACTOR);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
            break;
        }

        case FRAMEBUFFER_BLEND_FUNCTION_ADD:
        case FRAMEBUFFER_BLEND_FUNCTION_SUBTRACT:
        case FRAMEBUFFER_BLEND_FUNCTION_COMPONENT_MAX: {
            if(max_simultaneous_textures != 2 || shader_data->maps.count < 2) {
                texture_stage = shader_data->maps.count;
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG1, D3DTA_CURRENT);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG2, tss_option_argument);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                break;
            }

            uint16_t stage = shader_data->maps.count - 1;
            if(stage < 2) {
                stage = 1;
            }
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG2, tss_option_argument);
            break;
        }

        case FRAMEBUFFER_BLEND_FUNCTION_ALPHA_MULTIPLY_ADD: {
            if(max_simultaneous_textures == 2 && shader_data->maps.count > 1) {
                texture_stage = shader_data->maps.count - 1;
                if(texture_stage < 2) {
                    texture_stage = 1;
                }
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLORARG2, tss_option_argument);
                rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            }
            else {
                texture_stage = shader_data->maps.count;
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG1, D3DTA_CURRENT);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLORARG2, tss_option_argument);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
            }
            rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAARG2, tss_option_argument);
            break;
        }

        default: {
            rasterizer_dx9_set_texture(texture_stage, NULL);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTA_CURRENT);
            rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTA_CURRENT);
            rasterizer_transparent_geometry_group_draw_vertices(group, 0);
            rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            return;
        }
    }

    texture_stage = texture_stage + 1;
    rasterizer_dx9_set_texture(texture_stage, NULL);
    rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_COLOROP, D3DTA_CURRENT);
    rasterizer_dx9_set_texture_stage_state(texture_stage, D3DTSS_ALPHAOP, D3DTA_CURRENT);
    rasterizer_transparent_geometry_group_draw_vertices(false, group);
    rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
}

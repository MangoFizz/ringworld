#include <math.h>

#include "../console/console.h"
#include "../exception/exception.h"
#include "../shader/shader.h"
#include "../tag/definitions/shader_transparent_generic.h"
#include "../math/math.h"
#include "rasterizer_geometry_group.h"
#include "rasterizer_dx9.h"

extern float *shader_transparent_generic_vertex_contants;
extern bool *fog_enable;
extern FrameParameters *frame_parameters;

static bool assert_transparent_shader_map(ShaderTransparentGenericMap *map, uint16_t map_index) {
    ASSERT(map);
    if(!HANDLE_IS_NULL(map->map.tag_handle)) {
        console_printf_debug_err("Transparent shader map %d has a no bitmap", map_index);
        return false;
    }
    if(!(map->mipmap_bias == map->mipmap_bias) || map->mipmap_bias != 0.0f) {
        console_printf_debug_err("Transparent shader map %d has a non-zero mipmap bias", map_index);
        return false;
    }
    return true;
}

#define STG_ASSERT(a, b) if(!(a)) { console_printf_debug_err(b); return false; }

static bool assert_transparent_shader_stage(ShaderTransparentGenericStage *stage, uint16_t stage_index) {
    ASSERT(stage);
    if((stage->input_d != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_ZERO && stage->output_ab != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD && stage->input_d == stage->output_ab) ||
        (stage->input_d != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_ZERO && stage->output_bc != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD && stage->input_d == stage->output_bc) ||
        (stage->output_ab != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD && stage->output_bc != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD && stage->output_ab == stage->output_bc) ||
        (stage->input_d_alpha != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_ZERO && stage->input_d_mapping_alpha != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_MAPPING_COLOR_CLAMP_X && stage->input_d_alpha == stage->input_d_mapping_alpha) ||
        (stage->input_d_alpha != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_ZERO && stage->output_ab_alpha != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD && stage->input_d_alpha == stage->output_ab_alpha) ||
        (stage->input_d_mapping_alpha != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_MAPPING_COLOR_CLAMP_X && stage->output_ab_alpha != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD && stage->input_d_mapping_alpha == stage->output_ab_alpha)) {
        
        console_printf_debug_err("Transparent shader stage %d has invalid input/output", stage_index);
        return false;
    }

    if(stage->input_d_mapping != SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_MAPPING_COLOR_CLAMP_X ||
        (stage->output_ab_function != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_FUNCTION_MULTIPLY && stage->output_bc != SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_DISCARD)) {
        
        console_printf_debug_err("Transparent shader stage %d has invalid output function", stage_index);
        return false;
    }

    if(stage->input_d_alpha == SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_NEGATIVE_ONE ||
        stage->input_d_mapping_alpha == SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_MAPPING_COLOR_1_2 ||
        stage->output_ab_alpha == SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_VERTEX_ALPHA_0_FOG) {
        
        console_printf_debug_err("Transparent shader stage %d has invalid alpha", stage_index);
        return false;
    }

    return true;
}

bool rasterizer_dx9_transparent_generic_process(TransparentGeometryGroup *group) {
    ASSERT(group);
    ASSERT(group->shader);

    ShaderTransparentGeneric *shader_data = shader_type_assert(group->shader, SHADER_TYPE_SHADER_TRANSPARENT_GENERIC);

    float pixel_shader_constants[4][17] = {};

    ASSERT(shader_data->maps.count < 0 || shader_data->stages.count < 0);

    bool flag1 = true;
    for(size_t map_index = 0; map_index < shader_data->maps.count; map_index++) {
        ShaderTransparentGenericMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, map_index);
        if(flag1) {
            flag1 = assert_transparent_shader_map(map, map_index);
        }
    }

    if(*fog_enable) {
        uint16_t stage = 1;
        if(shader_data->stages.count > 0) {
            stage = shader_data->stages.count;
        }

        if(group->geometry_flags.sky == false || shader_data->framebuffer_blend_function != FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND) {
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

            if(group->effect.type == RENDER_MODEL_EFFECT_TYPE_ACTIVE_CAMOUFLAGE) {
                vertex_constants[10] = max_f32(0.0f, min_f32(1.0f, group->effect.intensity));
            }
        
            if(shader_data->framebuffer_fade_source > 0 && group->animation != NULL && group->animation->values != NULL) {
                vertex_constants[10] *= group->animation->values[shader_data->framebuffer_fade_source - 1];
            }
            
            rasterizer_dx9_set_vertex_shader_constant_f(10, vertex_constants, 3);
        }
        else {
            RenderGlobals *render_globals = get_render_globals();
            Plane3D *plane = &render_globals->fog.plane;
            VectorXYZ *camera = &render_globals->camera.position;
            float product = plane->i * camera->x + plane->j * camera->y + plane->k * camera->z - plane->w;
            float res = max_f32(0.0f, min_f32(1.0f, (product * -1.0f) / render_globals->fog.planar_maximum_depth));
            pixel_shader_constants[stage][0] = render_globals->fog.planar_color.r;
            pixel_shader_constants[stage][1] = render_globals->fog.planar_color.g;
            pixel_shader_constants[stage][2] = render_globals->fog.planar_color.b;
            pixel_shader_constants[stage][3] = render_globals->fog.planar_maximum_density * res;
        }
    }

    flag1 = true;
    if(shader_data->stages.count > 0) {
        for(int i = 0; i < shader_data->stages.count; i++) {
            ShaderTransparentGenericStage *stage = TAG_BLOCK_GET_ELEMENT(shader_data->stages, i);
            if(flag1) {
                flag1 = assert_transparent_shader_stage(stage, i);
            }

            ASSERT(stage->color0_animation_period == stage->color0_animation_period && stage->color0_animation_period != 0.0f);

            double progress = group->animation->values[0];
            if(group->animation != NULL || stage->flags.a_out_controls_color0_animation == false) {
                progress = math_periodic_function_evaluate(frame_parameters->elapsed_time / stage->color0_animation_period, stage->color0_animation_function);
            }

            ColorARGB constant_color0;
            constant_color0.a = stage->color0_animation_upper_bound.a - stage->color0_animation_lower_bound.a;
            constant_color0.r = stage->color0_animation_upper_bound.r - stage->color0_animation_lower_bound.r;
            constant_color0.g = stage->color0_animation_upper_bound.g - stage->color0_animation_lower_bound.g;
            constant_color0.b = stage->color0_animation_upper_bound.b - stage->color0_animation_lower_bound.b;

            constant_color0.a = stage->color0_animation_lower_bound.a + constant_color0.a * progress;
            constant_color0.r = stage->color0_animation_lower_bound.r + constant_color0.r * progress;
            constant_color0.g = stage->color0_animation_lower_bound.g + constant_color0.g * progress;
            constant_color0.b = stage->color0_animation_lower_bound.b + constant_color0.b * progress;

            ASSERT(constant_color0.r >= 0.0f && constant_color0.r <= 1.0f);
            ASSERT(constant_color0.g >= 0.0f && constant_color0.g <= 1.0f);
            ASSERT(constant_color0.b >= 0.0f && constant_color0.b <= 1.0f);

            if(stage->color0_source > 0 && stage->color0_source < FUNCTION_NAME_NULLABLE_MAX) {
                if(group->animation != NULL && group->animation->colors != NULL) {
                    ColorRGB *external_color = group->animation->colors + stage->color0_source - 1;
                    ASSERT(external_color->r >= 0.0f && external_color->r <= 1.0f);
                    ASSERT(external_color->g >= 0.0f && external_color->g <= 1.0f);
                    ASSERT(external_color->b >= 0.0f && external_color->b <= 1.0f);

                    constant_color0.r *= external_color->r;
                    constant_color0.g *= external_color->g;
                    constant_color0.b *= external_color->b;
                }
            }

            pixel_shader_constants[i][0] = constant_color0.r;
            pixel_shader_constants[i][1] = constant_color0.g;
            pixel_shader_constants[i][2] = constant_color0.b;
            pixel_shader_constants[i][3] = constant_color0.a;
            pixel_shader_constants[i][32] = stage->color1.r;
            pixel_shader_constants[i][33] = stage->color1.g;
            pixel_shader_constants[i][34] = stage->color1.b;
            pixel_shader_constants[i][35] = stage->color1.a;
        }
    }

    return rasterizer_dx9_set_pixel_shader_constant_f(0, pixel_shader_constants[0], 17) == D3D_OK;
}

void rasterizer_transparent_generic_draw(TransparentGeometryGroup *group, uint32_t *param_2) {
    ShaderTransparentGeneric *shader_data = shader_type_assert(group->shader, SHADER_TYPE_SHADER_TRANSPARENT_GENERIC);

    for(int i = 0; i < shader_data->extra_layers.count; i++) {
        TransparentGeometryGroup extra_layer_group = {};
        memcpy(&extra_layer_group, group, sizeof(TransparentGeometryGroup));
        ShaderTransparentExtraLayer *layers = shader_data->extra_layers.elements;
        extra_layer_group.shader = get_tag_data(layers[i].shader.tag_handle);
        extra_layer_group.sorted_index = -1;
        rasterizer_transparent_geometry_group_draw(&extra_layer_group, param_2);
    }

    bool flag1 = true;
    uint16_t vertex_permutation = shader_get_vertex_shader_permutation(group->shader);
    uint16_t vertex_buffer_type = rasterizer_get_vertex_buffer_type(group);
    uint16_t shader_permutation_index = group->shader_permutation_index;
    IDirect3DPixelShader9 *pixel_shader = rasterizer_dx9_get_pixel_shader(PS_TRANSPARENT_GENERIC);
    ASSERT(pixel_shader);
    IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_get_vertex_shader_for_permutation(vertex_permutation, vertex_buffer_type);
    rasterizer_dx9_set_vertex_shader(vertex_shader);
    rasterizer_dx9_set_vertex_declaration(vertex_buffer_type);
    rasterizer_dx9_set_pixel_shader(NULL);

    bool is_two_sided = shader_data->shader_transparent_generic_flags.two_sided;
    rasterizer_dx9_set_render_state(D3DRS_CULLMODE, is_two_sided ? D3DCULL_NONE : D3DCULL_CCW);
    rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, D3DBLENDOP_ADD);
    rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, shader_data->shader_transparent_generic_flags.alpha_tested);
    rasterizer_dx9_set_render_state(D3DRS_ALPHAREF, 0x0000007F);
    
    rasterizer_dx9_set_framebuffer_blend_function(shader_data->framebuffer_blend_function);

    short bitmap_data_index = group->shader_permutation_index;
    if(shader_data->shader_transparent_generic_flags.numeric && group->animation != NULL && shader_data->maps.count > 0) {
        ShaderTransparentGenericMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, 0);
        Bitmap *bitmap = tag_get_data(map->map.tag_handle, TAG_GROUP_BITMAP);
        uint16_t bitmap_count = bitmap->bitmap_data.count;
        
        int numeric_count = shader_data->numeric_counter_limit;
        short index = (bitmap_count != 8) ? 0 : 3;
        float new_val = floor(numeric_count * group->animation->values[index] + 0.5f);
        int res = round(new_val);
        if(res < 0) {
            index = 0;
        }
        else if(res > numeric_count) {
            numeric_count = res;
        }

        for(int i = 0; i < group->shader_permutation_index; i++) {
            numeric_count = numeric_count / bitmap_count;
        }
        bitmap_data_index = numeric_count % bitmap_count;
    }

    if(shader_data->maps.count > 0) {
        short unk_translation_table_1[] = {0, 2, 2, 2};
        short unk_translation_table_2[] = {1, 3, 3, 3};

        float vertex_shader_constants[8 * 4];

        int map_index = 0;
        for(int i = 0; i < 4; i++) {
            map_index = i;
            if(shader_data->maps.count > map_index) {
                ShaderTransparentGenericMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, map_index);
                ShaderFirstMapType first_map_type;
                if(map_index == 0) {
                    first_map_type = unk_translation_table_1[shader_data->first_map_type];
                }
                else {
                    first_map_type = 0;
                }
                
                ASSERT(shader_data->base.shader_flags.transparent_lit == false || shader_data->first_map_type == SHADER_FIRST_MAP_TYPE_2D_MAP);
                ASSERT(first_map_type < 0 || first_map_type >= SHADER_FIRST_MAP_TYPE_MAX);

                rasterizer_dx9_set_bitmap_data_texture(map_index, first_map_type, 0, bitmap_data_index, map->map.tag_handle);

                D3DTEXTUREADDRESS u_texture_mode;
                D3DTEXTUREADDRESS v_texture_mode;
                D3DTEXTUREADDRESS w_texture_mode;
                D3DTEXTUREADDRESS alternative_mode = unk_translation_table_2[shader_data->first_map_type];
                if(first_map_type == SHADER_FIRST_MAP_TYPE_2D_MAP && map->flags.u_clamped) {
                    u_texture_mode = D3DTADDRESS_CLAMP;
                    if(map->flags.v_clamped) {
                        v_texture_mode = D3DTADDRESS_CLAMP;
                    }
                    else {
                        if(map_index == 0) {
                            v_texture_mode = alternative_mode;
                        }
                        else {
                            v_texture_mode = D3DTADDRESS_WRAP;
                        }
                    }
                } 
                else {
                    if(map_index == 0) {
                        u_texture_mode = alternative_mode;
                    }
                    else {
                        u_texture_mode = D3DTADDRESS_WRAP;
                    }

                    if(first_map_type == SHADER_FIRST_MAP_TYPE_2D_MAP) {
                        if(map->flags.v_clamped) {
                            v_texture_mode = D3DTADDRESS_CLAMP;
                        }
                        else {
                            if(map_index == 0) {
                                v_texture_mode = alternative_mode;
                            }
                            else {
                                v_texture_mode = D3DTADDRESS_WRAP;
                            }
                        }
                    }
                    else {
                        if(map_index == 0) {
                            v_texture_mode = alternative_mode;
                        }
                        else {
                            v_texture_mode = D3DTADDRESS_WRAP;
                        }
                    }
                }
                if(map_index == 0) {
                    w_texture_mode = alternative_mode;
                }
                else {
                    w_texture_mode = D3DTADDRESS_WRAP;
                }

                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSU, u_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSV, v_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSW, w_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MINFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MIPFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);

                uint32_t maps_count = shader_data->maps.count;
                if(map_index == 0 || maps_count <= map_index || map_index == 1 || shader_data->first_map_type != SHADER_FIRST_MAP_TYPE_2D_MAP) {
                    if(shader_data->shader_transparent_generic_flags.first_map_is_in_screenspace == false) {
                        vertex_shader_constants[map_index * 8 + 0] = 1.0;
                        vertex_shader_constants[map_index * 8 + 1] = 0.0;
                        vertex_shader_constants[map_index * 8 + 2] = 0.0;
                        vertex_shader_constants[map_index * 8 + 3] = 0.0;
                        vertex_shader_constants[map_index * 8 + 4] = 0.0;
                        vertex_shader_constants[map_index * 8 + 5] = 1.0;
                        vertex_shader_constants[map_index * 8 + 6] = 0.0;
                        vertex_shader_constants[map_index * 8 + 7] = 0.0;
                    } 
                    else {
                        vertex_shader_constants[map_index * 8 + 0] = shader_transparent_generic_vertex_contants[0];
                        vertex_shader_constants[map_index * 8 + 1] = shader_transparent_generic_vertex_contants[1];
                        vertex_shader_constants[map_index * 8 + 2] = shader_transparent_generic_vertex_contants[2];
                        vertex_shader_constants[map_index * 8 + 3] = 0.0;
                        vertex_shader_constants[map_index * 8 + 4] = shader_transparent_generic_vertex_contants[3];
                        vertex_shader_constants[map_index * 8 + 5] = shader_transparent_generic_vertex_contants[4];
                        vertex_shader_constants[map_index * 8 + 6] = shader_transparent_generic_vertex_contants[5];
                        vertex_shader_constants[map_index * 8 + 7] = 0.0;
                    }
                }
                else {
                    float map_v_scale = map->map_v_scale;
                    float map_u_scale = map->map_u_scale;

                    if(map_index == 0 && shader_data->shader_transparent_generic_flags.scale_first_map_with_distance) {
                        map_v_scale = map_v_scale * group->z_sort * -1;
                        map_u_scale = map_u_scale * group->z_sort * -1;
                    }

                    if(map_index > 0 || shader_data->shader_transparent_generic_flags.first_map_is_in_screenspace == false) {
                        map_v_scale = map_v_scale * group->model_base_map_scale.x;
                        map_u_scale = map_u_scale * group->model_base_map_scale.y;
                    }

                    void *texture_animation = &map->u_animation_source;
                    float map_u_offset = map->map_u_offset;
                    float map_v_offset = map->map_v_offset;
                    float map_rotation = map->map_rotation;
                    shader_texture_animation_evaluate(map_u_scale, map_v_scale, map_u_offset, map_v_offset, map_rotation,
                                                      frame_parameters->elapsed_time, texture_animation, group->animation, 
                                                      vertex_shader_constants[map_index * 8 + 0],
                                                      vertex_shader_constants[map_index * 8 + 4]);
                }

                if(shader_data->maps.count <= map_index) {
                    rasterizer_dx9_set_texture(map_index, NULL);
                }
            }

            flag1 = false;
            rasterizer_dx9_set_vertex_shader_constant_f(13, vertex_shader_constants, 8);
        }

        rasterizer_dx9_transparent_generic_process(group);
        rasterizer_dx9_set_pixel_shader(pixel_shader);
        rasterizer_transparent_geometry_group_draw_vertices(false, group);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    }
}

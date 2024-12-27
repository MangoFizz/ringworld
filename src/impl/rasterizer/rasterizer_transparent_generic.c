#include <ringworld/shader/shader.h>
#include <ringworld/tag/definitions/shader_transparent_generic.h>
#include <ringworld/rasterizer/rasterizer_geometry_group.h>
#include <ringworld/rasterizer/rasterizer_dx9.h>

#include "../exception/exception.h"

extern float *shader_transparent_generic_vertex_contants;

void rasterizer_transparent_generic_draw(TransparentGeometryGroup *group, uint32_t *param_2) {
    ShaderTransparentGeneric *shader_data = shader_type_assert(group->shader, SHADER_TYPE_SHADER_TRANSPARENT_GENERIC);

    for(int i = 0; i < shader_data->extra_layers.count; i++) {
        TransparentGeometryGroup extra_layer_group = {};
        memcpy(&extra_layer_group, group, sizeof(TransparentGeometryGroup));
        ShaderTransparentExtraLayer *layers = shader_data->extra_layers.elements;
        extra_layer_group.shader = get_tag_data(layers[i].shader.tag_handle);
        extra_layer_group.sorted_index = -1;
        render_transparent_geometry_group(&extra_layer_group, param_2);
    }

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
                float vertex_shader_constants[8 * 4];
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

                    
                }
            }
        }
    }
}

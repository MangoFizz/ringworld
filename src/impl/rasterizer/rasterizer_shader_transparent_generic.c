#include <math.h>
#include <stdio.h>

#include "../console/console.h"
#include "../shader/shader.h"
#include "../tag/definitions/shader_transparent_generic.h"
#include "../interface/numeric_countdown.h"
#include "../math/math.h"
#include "../math/wave_functions.h"
#include "../exception/exception.h"
#include "../crypto/md5.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_shader.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_dx9_shader_compiler.h"
#include "rasterizer_render.h"
#include "rasterizer_geometry_group.h"
#include "rasterizer_shader_transparent_generic.h"

enum {
    NUM_OF_SHADER_COMPILE_DEFINES = 10
};

typedef struct ShaderStageParams {
    int16_t input_a;
    int16_t input_a_mapping;
    int16_t input_b;
    int16_t input_b_mapping;
    int16_t input_c;
    int16_t input_c_mapping;
    int16_t input_d;
    int16_t input_d_mapping;

    int16_t color_mux;
    int16_t output_ab;
    int16_t output_ab_function;
    int16_t output_cd;
    int16_t output_cd_function;
    int16_t output_ab_cd_mux_sum;
    int16_t output_mapping_color;

    int16_t input_a_alpha;
    int16_t input_a_mapping_alpha;
    int16_t input_b_alpha;
    int16_t input_b_mapping_alpha;
    int16_t input_c_alpha;
    int16_t input_c_mapping_alpha;
    int16_t input_d_alpha;
    int16_t input_d_mapping_alpha;

    int16_t alpha_mux;
    int16_t output_ab_alpha;
    int16_t output_cd_alpha;
    int16_t output_ab_cd_mux_sum_alpha;
    int16_t output_mapping_alpha;
    
    int16_t is_fog_stage;
} ShaderStageParams;
_Static_assert(sizeof(ShaderStageParams) == sizeof(int16_t) * 29);

char *shader_transparent_generic_source = NULL; 
ShaderTransparentGenericInstances *shader_transparent_generic_instances = NULL;
ShaderTransparentGenericTagsCache *shader_transparent_generic_tags_cache = NULL;

static const char *get_shader_source() {
    if(shader_transparent_generic_source != NULL) {
        return shader_transparent_generic_source;
    }

    void *buffer = NULL;
    int bytes_read = 0;

    HANDLE file = CreateFileA("shaders/shader_transparent_generic.hlsl", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_SUPPORTS_EXTENDED_ATTRIBUTES, NULL);
    if(file == INVALID_HANDLE_VALUE) {
        const char *error = "Failed to read shader transparent generic source; make sure the file \"shaders/shader_transparent_generic.hlsl\" exists.";
        MessageBoxA(NULL, error, "Ringworld", MB_OK | MB_ICONERROR);
        CRASHF_DEBUG("%s", error);
    }
    
    size_t file_size = GetFileSize(file, NULL);
    ASSERT(file_size != INVALID_FILE_SIZE);

    HGLOBAL buffer_handle = GlobalAlloc(GMEM_FIXED, file_size);
    if(buffer_handle != NULL) {
        bool success = ReadFile(file, buffer_handle, file_size, (LPDWORD)&bytes_read, NULL);
        if(!success) {
            GlobalFree(buffer_handle);
        }
        else {
            shader_transparent_generic_source = buffer_handle;
            shader_transparent_generic_source[bytes_read] = '\0';
        }
        CloseHandle(file);
    }

    return shader_transparent_generic_source;
}

static D3D_SHADER_MACRO generate_stage_define(size_t stage_index, ShaderStageParams params) {
    char buffer[128];
    sprintf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
        stage_index, 

        params.input_a, params.input_a_mapping, 
        params.input_b, params.input_b_mapping, 
        params.input_c, params.input_c_mapping, 
        params.input_d, params.input_d_mapping, 
        
        params.color_mux, 
        params.output_ab, params.output_ab_function, 
        params.output_cd, params.output_cd_function, 
        params.output_ab_cd_mux_sum, 
        params.output_mapping_color, 
        
        params.input_a_alpha, params.input_a_mapping_alpha, 
        params.input_b_alpha, params.input_b_mapping_alpha, 
        params.input_c_alpha, params.input_c_mapping_alpha, 
        params.input_d_alpha, params.input_d_mapping_alpha, 
        
        params.alpha_mux, 
        params.output_ab_alpha, 
        params.output_cd_alpha, 
        params.output_ab_cd_mux_sum_alpha, 
        params.output_mapping_alpha,

        params.is_fog_stage
    );

    char *macro = GlobalAlloc(GMEM_FIXED, strlen(buffer) + 1);
    strcpy(macro, buffer);
    
    sprintf(buffer, "S%d_CONFIGURATION", stage_index);
    char *name = GlobalAlloc(GMEM_FIXED, strlen(buffer) + 1);
    strcpy(name, buffer);

    D3D_SHADER_MACRO result = {0};
    result.Name = name;
    result.Definition = macro;

    return result;
}

static char *generate_defines_hash(D3D_SHADER_MACRO *defines) {
    char buffer[1024] = {0};
    for(size_t i = 0; i < NUM_OF_SHADER_COMPILE_DEFINES; i++) {
        if(defines[i].Name != NULL) {
            strcat(buffer, defines[i].Name);
        }
        if(defines[i].Definition != NULL) {
            strcat(buffer, defines[i].Definition);
        }
    }
    char *hash = GlobalAlloc(GMEM_FIXED, 32);
    generate_md5_hash(buffer, sizeof(buffer), hash);
    return hash;
}

static D3D_SHADER_MACRO *generate_defines(ShaderTransparentGeneric *tag) {
    D3D_SHADER_MACRO *defines = GlobalAlloc(GPTR, sizeof(D3D_SHADER_MACRO) * 10);
    size_t defines_count = 0;
    ShaderStageParams params = {0};

    // if there are no stages, set up the default stage
    if(tag->stages.count == 0) {
        params.input_a = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_MAP_COLOR_0;
        params.input_b = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_ONE;
        params.output_ab = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
        params.input_a_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_MAP_ALPHA_0;
        params.input_b_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_ONE;
        params.output_ab_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
        params.is_fog_stage = false;
        defines[defines_count] = generate_stage_define(defines_count, params);
        defines_count++;
    }
    else {
        for(size_t current_stage = 0; current_stage < tag->stages.count && current_stage < 7; current_stage++) {
            ShaderTransparentGenericStage *stage = TAG_BLOCK_GET_ELEMENT(tag->stages, current_stage);
            
            params.input_a = stage->input_a;
            params.input_a_mapping = stage->input_a_mapping;
            params.input_b = stage->input_b;
            params.input_b_mapping = stage->input_b_mapping;
            params.input_c = stage->input_c;
            params.input_c_mapping = stage->input_c_mapping;
            params.input_d = stage->input_d;
            params.input_d_mapping = stage->input_d_mapping;

            params.color_mux = stage->flags.color_mux;
            params.output_ab = stage->output_ab;
            params.output_ab_function = stage->output_ab_function;
            params.output_cd = stage->output_bc;
            params.output_cd_function = stage->output_cd_function;
            params.output_ab_cd_mux_sum = stage->output_ab_cd_mux_sum;
            params.output_mapping_color = stage->output_mapping_color;

            params.input_a_alpha = stage->input_a_alpha;
            params.input_a_mapping_alpha = stage->input_a_mapping_alpha;
            params.input_b_alpha = stage->input_b_alpha;
            params.input_b_mapping_alpha = stage->input_b_mapping_alpha;
            params.input_c_alpha = stage->input_c_alpha;
            params.input_c_mapping_alpha = stage->input_c_mapping_alpha;
            params.input_d_alpha = stage->input_d_alpha;

            params.alpha_mux = stage->flags.alpha_mux;
            params.output_ab_alpha = stage->output_ab_alpha;
            params.output_cd_alpha = stage->output_cd_alpha;
            params.output_ab_cd_mux_sum_alpha = stage->output_ab_cd_mux_sum_alpha;
            params.output_mapping_alpha = stage->output_mapping_alpha;

            params.is_fog_stage = false;

            defines[defines_count] = generate_stage_define(current_stage, params);
            defines_count++;

            // Wipe the params for the next iteration
            memset(&params, 0, sizeof(ShaderStageParams));
        }
    }

    FramebufferFadeMode framebuffer_mode = tag->framebuffer_fade_mode;
    ShaderTransparentGenericStageInputColor input_color = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_ZERO;
    ShaderTransparentGenericStageInputAlpha input_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_ZERO;
    switch(framebuffer_mode) {
        case FRAMEBUFFER_FADE_MODE_NONE:
            input_color = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_VERTEX_ALPHA_0_FADE_NONE;
            input_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_VERTEX_ALPHA_0_FADE_NONE;
            break;
        case FRAMEBUFFER_FADE_MODE_FADE_WHEN_PERPENDICULAR:
            input_color = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_VERTEX_ALPHA_1_FADE_PERPENDICULAR;
            input_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_VERTEX_ALPHA_1_FADE_PERPENDICULAR;
            break;
        case FRAMEBUFFER_FADE_MODE_FADE_WHEN_PARALLEL:
            input_color = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_VERTEX_COLOR_1_FADE_PERPENDICULAR;
            input_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_VERTEX_BLUE_1_FADE_PARALLEL;
            break;
        default:
            CRASHF_DEBUG("invalid framebuffer fade mode: %d", framebuffer_mode);
            break;
    }
    FramebufferBlendFunction framebuffer_blend_function = tag->framebuffer_blend_function;
    switch(framebuffer_blend_function) {
        case FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND:
            params.input_a_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_SCRATCH_ALPHA_0;
            params.input_b_alpha = input_alpha;
            params.output_ab_cd_mux_sum_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
            break;

        case FRAMEBUFFER_BLEND_FUNCTION_MULTIPLY:
        case FRAMEBUFFER_BLEND_FUNCTION_COMPONENT_MIN:
            params.input_a = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_SCRATCH_COLOR_0;
            params.input_b = input_color;
            params.input_c = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_ONE;
            params.input_d = input_color;
            params.input_d_mapping = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_MAPPING_COLOR_1_CLAMP_X;
            params.output_ab_cd_mux_sum = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
            break;

        case FRAMEBUFFER_BLEND_FUNCTION_DOUBLE_MULTIPLY:
            params.input_a = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_SCRATCH_COLOR_0;
            params.input_b = input_color;
            params.input_c = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_ONE_HALF;
            params.input_d = input_color;
            params.input_d_mapping = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_MAPPING_COLOR_1_CLAMP_X;
            params.output_ab_cd_mux_sum = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
            break;

        case FRAMEBUFFER_BLEND_FUNCTION_ADD:
        case FRAMEBUFFER_BLEND_FUNCTION_SUBTRACT:
        case FRAMEBUFFER_BLEND_FUNCTION_COMPONENT_MAX:
            params.input_a = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_SCRATCH_COLOR_0;
            params.input_b = input_color;
            params.output_ab_cd_mux_sum = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
            break;

        case FRAMEBUFFER_BLEND_FUNCTION_ALPHA_MULTIPLY_ADD:
            params.input_a_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_ALPHA_SCRATCH_ALPHA_0;
            params.input_b_alpha = input_alpha;
            params.output_ab_cd_mux_sum_alpha = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
            params.input_a = SHADER_TRANSPARENT_GENERIC_STAGE_INPUT_COLOR_SCRATCH_COLOR_0;
            params.input_b = input_color;
            params.output_ab_cd_mux_sum = SHADER_TRANSPARENT_GENERIC_STAGE_OUTPUT_ALPHA_SCRATCH_ALPHA_0_FINAL_ALPHA;
            break;

        default:
            CRASHF_DEBUG("invalid framebuffer blend function: %d", framebuffer_blend_function);
            break;
    }
    params.is_fog_stage = true;
    defines[defines_count] = generate_stage_define(defines_count, params);
    defines_count++;

    if(tag->first_map_type != SHADER_FIRST_MAP_TYPE_2D_MAP) {
        const char *first_map_is_cube = "FIRST_MAP_IS_CUBE";
        char *first_map_type_macro = GlobalAlloc(GMEM_FIXED, strlen(first_map_is_cube) + 1);
        strcpy(first_map_type_macro, first_map_is_cube);
        defines[defines_count].Name = first_map_type_macro;
        defines[defines_count].Definition = NULL;
        defines_count++;
    }

    defines[defines_count].Name = NULL;
    defines[defines_count].Definition = NULL;

    return defines;
}

static void free_defines(D3D_SHADER_MACRO *defines) {
    // al manejo de memoria no hay que tenerle miedo, hay que tenerle respeto
    for(size_t i = 0; i < NUM_OF_SHADER_COMPILE_DEFINES; i++) {
        if(defines[i].Name != NULL) {
            GlobalFree((void *)defines[i].Name);
        }
        if(defines[i].Definition != NULL) {
            GlobalFree((void *)defines[i].Definition);
        }
    }
    GlobalFree(defines);
}

ID3DBlob *rasterizer_shader_transparent_generic_compile_shader(D3D_SHADER_MACRO *defines) {
    ID3DBlob *compiled_shader = NULL;
    if(!rasterizer_dx9_shader_compiler_compile_shader_from_blob(get_shader_source(), "main", "ps_3_0", defines, &compiled_shader)) {
        CRASHF_DEBUG("failed to compile shader transparent generic\n");
    }
    return compiled_shader;
}

ShaderTransparentGenericInstance *rasterizer_shader_transparent_generic_get_or_create_instance(ShaderTransparentGeneric *tag) {
    if(shader_transparent_generic_instances == NULL) {
        shader_transparent_generic_instances = table_new("transparent generic instances", MAX_SHADER_TRANSPARENT_GENERIC_INSTANCES, sizeof(ShaderTransparentGenericInstance));
    }
    if(shader_transparent_generic_tags_cache == NULL) {
        shader_transparent_generic_tags_cache = table_new("transparent generic tags cache", MAX_SHADER_TRANSPARENT_GENERIC_PER_MAP, sizeof(ShaderTransparentGenericTagCache));
    }
    
    // Look for the instance in the cache
    TableIterator it;
    table_init_iterator(&it, shader_transparent_generic_tags_cache);
    ShaderTransparentGenericTagCache *cache_entry;
    while(cache_entry = table_iterate(&it), cache_entry != NULL) {
        if(cache_entry->shader_data == tag) {
            ShaderTransparentGenericInstance *instance = table_get_element(shader_transparent_generic_instances, cache_entry->shader_instance);
            if(instance == NULL) {
                CRASHF_DEBUG("shader transparent generic instance not found in the instance table");
            }
            return instance;
        }
    }
    
    if(shader_transparent_generic_instances->count == MAX_SHADER_TRANSPARENT_GENERIC_INSTANCES) {
        CRASHF_DEBUG("maximum number of shader transparent generic instances reached");
    }

    D3D_SHADER_MACRO *defines = generate_defines(tag);
    char *hash = generate_defines_hash(defines);
    ShaderTransparentGenericInstance *instance = NULL;

    table_init_iterator(&it, shader_transparent_generic_instances);
    while(instance = table_iterate(&it), instance != NULL) {
        if(strncmp(instance->hash, hash, 32) == 0) {
            break;
        }
    }

    if(instance == NULL) {
        size_t instance_index = shader_transparent_generic_instances->next_free_element_index;
        instance = table_add_element(shader_transparent_generic_instances);
        table_init_element(shader_transparent_generic_instances, instance);
        instance->compiled_shader = rasterizer_shader_transparent_generic_compile_shader(defines);
        instance->handle = MAKE_HANDLE(instance->id, instance_index);
        memcpy(instance->hash, hash, 32);
    }

    cache_entry = table_add_element(shader_transparent_generic_tags_cache);
    table_init_element(shader_transparent_generic_tags_cache, cache_entry);
    cache_entry->shader_data = tag;
    cache_entry->shader_instance = instance->handle;

    free_defines(defines);
    GlobalFree(hash);

    return instance;
}

void rasterizer_shader_transparent_generic_create_instances_for_current_map(void) {
    TagDataHeader *tag_data_header = tag_get_data_header();
    if(tag_data_header == NULL) {
        CRASHF_DEBUG("tag data header is NULL");
    }
    
    for(size_t i = 0; i < tag_data_header->tag_count; i++) {
        TagEntry *tag_entry = tag_data_header->tags + i;
        if(tag_entry->primary_group == TAG_GROUP_SHADER_TRANSPARENT_GENERIC) {
            rasterizer_shader_transparent_generic_get_or_create_instance(tag_entry->data);
        }
    }
}

static bool release_instances(const TableIterator *iterator, void *element, void *) {
    ShaderTransparentGenericInstance *instance = (ShaderTransparentGenericInstance *)element;
    if(instance->shader != NULL) {
        IDirect3DPixelShader9_Release(instance->shader);
        instance->shader = NULL;
    }
    if(instance->compiled_shader != NULL) {
        ID3D10Blob_Release((ID3DBlob *)instance->compiled_shader);
        instance->compiled_shader = NULL;
    }
    return true;
}

void rasterizer_shader_transparent_generic_clear_instances(void) {
    if(shader_transparent_generic_tags_cache != NULL) {
        table_clear(shader_transparent_generic_tags_cache);
    }
    if(shader_transparent_generic_instances != NULL) {
        table_iterate_simple(shader_transparent_generic_instances, release_instances, NULL);
        table_clear(shader_transparent_generic_instances);
    }
}

IDirect3DPixelShader9 *rasterizer_shader_transparent_generic_get_pixel_shader(ShaderTransparentGeneric *tag) {
    ASSERT(tag != NULL);

    ShaderTransparentGenericInstance *instance = rasterizer_shader_transparent_generic_get_or_create_instance(tag);
    if(instance->shader == NULL) {
        ID3DBlob *compiled_shader = instance->compiled_shader;
        IDirect3DPixelShader9 *shader;
        if(FAILED(IDirect3DDevice9_CreatePixelShader(rasterizer_dx9_device(), (DWORD *)ID3D10Blob_GetBufferPointer(compiled_shader), &shader))) {
            CRASHF_DEBUG("failed to create pixel shader for shader transparent generic\n");
        }
        ID3D10Blob_Release(compiled_shader);
        instance->compiled_shader = NULL;
        instance->shader = shader;
    }

    return instance->shader;
}

void rasterizer_shader_transparent_generic_draw(TransparentGeometryGroup *group, uint32_t *param_2) {
    RenderGlobals *render_globals = rasterizer_render_get_globals();
    ShaderTransparentGeneric *shader_data = shader_type_assert(group->shader, SHADER_TYPE_SHADER_TRANSPARENT_GENERIC);

    uint16_t vertex_permutation = shader_get_vertex_shader_permutation(group->shader);
    uint16_t vertex_buffer_type = rasterizer_geometry_group_get_vertex_buffer_type(group);
    IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader_for_permutation(vertex_permutation, vertex_buffer_type);
    rasterizer_dx9_set_vertex_shader(vertex_shader);
    rasterizer_dx9_set_vertex_declaration(vertex_buffer_type);
    rasterizer_dx9_set_pixel_shader(NULL);

    for(int i = 0; i < shader_data->extra_layers.count; i++) {
        TransparentGeometryGroup extra_layer_group = {};
        memcpy(&extra_layer_group, group, sizeof(TransparentGeometryGroup));
        ShaderTransparentExtraLayer *layers = shader_data->extra_layers.elements;
        extra_layer_group.shader = tag_get_data(TAG_GROUP_SHADER, layers[i].shader.tag_handle);
        extra_layer_group.sorted_index = -1;
        rasterizer_transparent_geometry_group_draw(&extra_layer_group, param_2);
    }

    rasterizer_dx9_set_render_state(D3DRS_CULLMODE, shader_data->shader_transparent_generic_flags.two_sided ? D3DCULL_NONE : D3DCULL_CCW);
    rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, D3DBLENDOP_ADD);
    rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, shader_data->shader_transparent_generic_flags.alpha_tested);
    rasterizer_dx9_set_render_state(D3DRS_ALPHAREF, 0x0000007F);

    rasterizer_dx9_set_framebuffer_blend_function(shader_data->framebuffer_blend_function);

    uint16_t bitmap_data_index = group->shader_permutation_index;
    if(shader_data->shader_transparent_generic_flags.numeric && group->animation != NULL && shader_data->maps.count > 0) {
        ShaderTransparentGenericMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, 0);
        Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, map->map.tag_handle);
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
        float animation_vsh_constants[4 * 8] = {0};

        for(size_t map_index = 0; map_index < 4; map_index++) {
            if(map_index < shader_data->maps.count) {
                bool is_first_map = map_index == 0;
                ShaderTransparentGenericMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, map_index);
                ShaderFirstMapType first_map_type = shader_data->first_map_type;
                BitmapType bitmap_type;
                if(is_first_map) {
                    bitmap_type = shader_bitmap_type_for_shader_transparent_generic_first_map(first_map_type);
                }
                else {
                    bitmap_type = BITMAP_TYPE_2D_TEXTURES;
                }
                D3DTEXTUREADDRESS first_map_texture_mode;
                switch(bitmap_type) {
                    case BITMAP_TYPE_2D_TEXTURES:
                        first_map_texture_mode = D3DTADDRESS_WRAP;
                        break;
                    case BITMAP_TYPE_CUBE_MAPS:
                        first_map_texture_mode = D3DTADDRESS_CLAMP;
                        break;
                    default:
                        CRASHF_DEBUG("Invalid bitmap type: %d", bitmap_type);
                        break;
                }

                #define SELECT_TEXTURE_MODE_IF_FIRST_MAP(val) \
                    (is_first_map ? first_map_texture_mode : val)

                #define SELECT_TEXTURE_MODE_IF_CLAMPED(is_clamped, clamp_val, default_val) \
                    (bitmap_type == BITMAP_TYPE_2D_TEXTURES && is_clamped ? clamp_val : SELECT_TEXTURE_MODE_IF_FIRST_MAP(default_val))

                D3DTEXTUREADDRESS u_texture_mode = SELECT_TEXTURE_MODE_IF_CLAMPED(map->flags.u_clamped, D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP);
                D3DTEXTUREADDRESS v_texture_mode = SELECT_TEXTURE_MODE_IF_CLAMPED(map->flags.v_clamped, D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP);
                D3DTEXTUREADDRESS w_texture_mode = SELECT_TEXTURE_MODE_IF_FIRST_MAP(D3DTADDRESS_WRAP);

                #undef SELECT_TEXTURE_MODE_IF_CLAMPED
                #undef SELECT_TEXTURE_MODE_IF_FIRST_MAP

                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSU, u_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSV, v_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_ADDRESSW, w_texture_mode);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MINFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(map_index, D3DSAMP_MIPFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);

                rasterizer_dx9_texture_set_bitmap_data_texture(map_index, bitmap_type, BITMAP_USAGE_ALPHA_BLEND, bitmap_data_index, map->map.tag_handle);

                uint32_t maps_count = shader_data->maps.count;
                if(is_first_map && shader_data->first_map_type != SHADER_FIRST_MAP_TYPE_2D_MAP) {
                    if(shader_data->shader_transparent_generic_flags.first_map_is_in_screenspace == false) {
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
                        animation_vsh_constants[map_index * 8 + 0] = render_globals->frustum.world_to_view.forward.i;
                        animation_vsh_constants[map_index * 8 + 1] = render_globals->frustum.world_to_view.forward.j;
                        animation_vsh_constants[map_index * 8 + 2] = render_globals->frustum.world_to_view.forward.k;
                        animation_vsh_constants[map_index * 8 + 3] = 0.0;
                        animation_vsh_constants[map_index * 8 + 4] = render_globals->frustum.world_to_view.left.i;
                        animation_vsh_constants[map_index * 8 + 5] = render_globals->frustum.world_to_view.left.j;
                        animation_vsh_constants[map_index * 8 + 6] = render_globals->frustum.world_to_view.left.k;
                        animation_vsh_constants[map_index * 8 + 7] = 0.0;
                    }
                }
                else {
                    float map_v_scale = map->map_v_scale;
                    float map_u_scale = map->map_u_scale;

                    if(is_first_map && shader_data->shader_transparent_generic_flags.scale_first_map_with_distance) {
                        map_v_scale = map_v_scale * group->z_sort * -1;
                        map_u_scale = map_u_scale * group->z_sort * -1;
                    }

                    if(!is_first_map || shader_data->shader_transparent_generic_flags.first_map_is_in_screenspace == false) {
                        map_u_scale = map_u_scale * group->model_base_map_scale.x;
                        map_v_scale = map_v_scale * group->model_base_map_scale.y;
                    }

                    void *texture_animation = &map->u_animation_source;
                    float map_u_offset = map->map_u_offset;
                    float map_v_offset = map->map_v_offset;
                    float map_rotation = map->map_rotation;
                    FrameParameters *frame_parameters = rasterizer_render_get_frame_parameters();
                    shader_texture_animation_evaluate(map_u_scale, map_v_scale, map_u_offset, map_v_offset, map_rotation,
                                                        frame_parameters->elapsed_time, texture_animation, group->animation, 
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

        rasterizer_dx9_set_vertex_shader_constant_f(13, animation_vsh_constants, 8);
        
        rasterizer_dx9_transparent_generic_preprocess(group); 
        IDirect3DPixelShader9 *shader = rasterizer_shader_transparent_generic_get_pixel_shader(shader_data);
        rasterizer_dx9_set_pixel_shader(shader);
        rasterizer_transparent_geometry_group_draw_vertices(false, group);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    }
}

void rasterizer_dx9_transparent_generic_preprocess(TransparentGeometryGroup *group) {
    ASSERT(group != NULL);
    ASSERT(group->shader != NULL);

    ShaderTransparentGeneric *shader_data = shader_type_assert(group->shader, SHADER_TYPE_SHADER_TRANSPARENT_GENERIC);

    if(shader_data->stages.count == 0 && shader_data->maps.count == 0) {
        return;
    }

    float ps_constants_buffer[8*4 + 8*4 + 4] = {0};

    float *stage_color0 = &ps_constants_buffer[8 * 0];
    float *stage_color1 = &ps_constants_buffer[8 * 4];
    float *fog_config   = &ps_constants_buffer[8 * 8];

    ASSERT(shader_data->maps.count > 0 || shader_data->stages.count > 0);

    for(size_t map_index = 0; map_index < shader_data->maps.count; map_index++) {
        ShaderTransparentGenericMap *map = TAG_BLOCK_GET_ELEMENT(shader_data->maps, map_index);
        if(HANDLE_IS_NULL(map->map.tag_handle)) {
            CRASHF_DEBUG("transparent generic shader map %d has a no bitmap", map_index);
        }
        if(map->mipmap_bias != map->mipmap_bias || map->mipmap_bias != 0.0f) {
            CRASHF_DEBUG("transparent generic shader map %d has a non-zero mipmap bias", map_index);
        }
    }

    bool is_fog_enabled = rasterizer_render_get_fog_enabled();
    if(is_fog_enabled) {
        uint16_t fog_stage = 1;
        if(shader_data->stages.count > 0) {
            fog_stage = shader_data->stages.count;
        }

        fog_config[0] = 1.0f;

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
                vertex_constants[10] = clamp_f32(1.0f - group->effect.intensity, 0.0f, 1.0f);
            }
        
            if(shader_data->framebuffer_fade_source > 0 && group->animation != NULL && group->animation->values != NULL) {
                vertex_constants[10] *= group->animation->values[shader_data->framebuffer_fade_source - 1];
                ASSERT(!nan_f32(vertex_constants[10])); 
            }
            
            rasterizer_dx9_set_vertex_shader_constant_f(10, vertex_constants, 3);
        }
        else {
            RenderGlobals *render_globals = rasterizer_render_get_globals();
            Plane3D *plane = &render_globals->fog.plane;
            VectorXYZ *camera = &render_globals->camera.position;
            float product = plane->i * camera->x + plane->j * camera->y + plane->k * camera->z - plane->w;
            float res = clamp_f32((product * -1.0f) / render_globals->fog.planar_maximum_depth, 0.0f, 1.0f);
            stage_color0[fog_stage * 4 + 0] = render_globals->fog.planar_color.r;
            stage_color0[fog_stage * 4 + 1] = render_globals->fog.planar_color.g;
            stage_color0[fog_stage * 4 + 2] = render_globals->fog.planar_color.b;
            stage_color0[fog_stage * 4 + 3] = render_globals->fog.planar_maximum_density * res;

            fog_config[1] = 1.0f;
        }
    }

    if(shader_data->stages.count > 0) {
        for(int i = 0; i < shader_data->stages.count; i++) {
            ShaderTransparentGenericStage *stage = TAG_BLOCK_GET_ELEMENT(shader_data->stages, i);

            float progress = 0.0f;
            if(group->animation != NULL && group->animation->values != NULL && stage->flags.a_out_controls_color0_animation) {
                progress = group->animation->values[0];
            }
            else {
                FrameParameters *frame_parameters = rasterizer_render_get_frame_parameters();
                progress = wave_function_calculate_value_og(frame_parameters->elapsed_time / stage->color0_animation_period, stage->color0_animation_function);
            } 
            ASSERT(!nan_f32(progress));

            ColorARGB constant_color0;
            constant_color0.a = stage->color0_animation_upper_bound.a - stage->color0_animation_lower_bound.a;
            constant_color0.r = stage->color0_animation_upper_bound.r - stage->color0_animation_lower_bound.r;
            constant_color0.g = stage->color0_animation_upper_bound.g - stage->color0_animation_lower_bound.g;
            constant_color0.b = stage->color0_animation_upper_bound.b - stage->color0_animation_lower_bound.b;

            constant_color0.a = stage->color0_animation_lower_bound.a + constant_color0.a * progress;
            constant_color0.r = stage->color0_animation_lower_bound.r + constant_color0.r * progress;
            constant_color0.g = stage->color0_animation_lower_bound.g + constant_color0.g * progress;
            constant_color0.b = stage->color0_animation_lower_bound.b + constant_color0.b * progress;

            if(stage->color0_source > 0 && stage->color0_source < FUNCTION_NAME_NULLABLE_MAX) {
                if(group->animation != NULL && group->animation->colors != NULL) {
                    ColorRGB *external_color = &group->animation->colors[stage->color0_source - 1];
                    constant_color0.r *= external_color->r;
                    constant_color0.g *= external_color->g;
                    constant_color0.b *= external_color->b;
                }
            }

            stage_color0[i * 4 + 0] = constant_color0.r;
            stage_color0[i * 4 + 1] = constant_color0.g;
            stage_color0[i * 4 + 2] = constant_color0.b;
            stage_color0[i * 4 + 3] = constant_color0.a;
            stage_color1[i * 4 + 0] = stage->color1.r;
            stage_color1[i * 4 + 1] = stage->color1.g;
            stage_color1[i * 4 + 2] = stage->color1.b;
            stage_color1[i * 4 + 3] = stage->color1.a;
        }
    }

    rasterizer_dx9_set_pixel_shader_constant_f(0, stage_color0, 8);
    rasterizer_dx9_set_pixel_shader_constant_f(8, stage_color1, 8);
    rasterizer_dx9_set_pixel_shader_constant_f(16, fog_config, 1);
}

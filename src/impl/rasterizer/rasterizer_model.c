#include <math.h>
#include "../debug/assertion.h"
#include "../game/game_state.h"
#include "../math/color.h"
#include "../math/math.h"
#include "../math/wave_functions.h"
#include "../rng/rng.h"
#include "../shader/shader.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_pixel_shader_constants.h"
#include "rasterizer_dx9_vertex_shader_constants.h"
#include "rasterizer_dx9_vertex_shader.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer.h"
#include "rasterizer_model.h"

extern float *rasterizer_model_depth_bias;
extern float *rasterizer_model_slope_scale_depth_bias;
extern RasterizerModelRenderParameters **rasterizer_model_render_parameters;
extern bool *rasterizer_model_zbuffer_disabled;
extern bool *rasterizer_model_fog_enabled;
extern PixelShaderModelConstants *rasterizer_model_pixel_shader_constants;
extern bool *rasterizer_model_should_use_planar_fog;
extern float *rasterizer_model_illumination_intensity;
extern float *rasterizer_model_effect_opacity;
extern ColorARGB **rasterizer_model_reflection_tint_color;
extern void rasterizer_model_draw_triangles(); // Properly define later

RasterizerModelRenderParameters *rasterizer_model_get_render_parameters(void) {
    return *rasterizer_model_render_parameters;
}

bool rasterizer_model_is_zbuffer_disabled(void) {
    return *rasterizer_model_zbuffer_disabled;
}

bool rasterizer_model_is_fog_enabled(void) {
    return *rasterizer_model_fog_enabled;
}

PixelShaderModelConstants *rasterizer_model_get_pixel_shader_constants(void) {
    return rasterizer_model_pixel_shader_constants;
}

bool rasterizer_model_use_planar_fog_enabled(void) {
    return *rasterizer_model_should_use_planar_fog;
}

float rasterizer_model_get_illumination_intensity(void) {
    return *rasterizer_model_illumination_intensity;
}

// This is calculated from (1.0f - TransparentGeometryGroup.effect.intesity)
float rasterizer_model_get_effect_opacity(void) {
    return *rasterizer_model_effect_opacity;
}

ColorARGB *rasterizer_model_get_reflection_tint_color(void) {
    return *rasterizer_model_reflection_tint_color;
}

void rasterizer_model_enable_depth_bias(void) {
    rasterizer_dx9_set_depth_bias(*rasterizer_model_depth_bias, *rasterizer_model_slope_scale_depth_bias);
}

void rasterizer_model_disable_depth_bias(void) {
    rasterizer_dx9_set_depth_bias(0.0f, 0.0f);
}

void rasterizer_model_initialize(void) {
    *rasterizer_model_reflection_tint_color = game_state_allocate_heap(sizeof(ColorARGB));
}

void rasterizer_model_draw(ShaderModel *shader, int16_t texture_index, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7) {
    D3DCAPS9 *d3d_caps = rasterizer_dx9_device_caps();
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    RasterizerModelRenderParameters *render_params = rasterizer_model_get_render_parameters();
    RasterizerFrameParameters *frame_parameters = rasterizer_get_frame_parameters();
    bool is_zbuffer_disabled = rasterizer_model_is_zbuffer_disabled();
    bool is_fog_enabled = rasterizer_model_is_fog_enabled();
    
    IDirect3DPixelShader9 *pixel_shader = rasterizer_model_get_pixel_shader(shader);
    ASSERT_OR_RETURN(pixel_shader != NULL);
    
    bool alpha_blended_decal = shader->properties.flags.alpha_blended_decal != 0;
    bool disable_two_sided_culling = true;

    VectorXYZ *center_point = &render_params->center_point;
    VectorXYZ *camera_position = &window_parameters->camera.position;
    VectorXYZ camera_to_center = {
        center_point->x - camera_position->x,
        center_point->y - camera_position->y,
        center_point->z - camera_position->z
    };
    float camera_center_dot = math_vector_dot_product(&camera_to_center, &window_parameters->camera.forward);

    float reflection_strength = 1.0f;
    float cutoff = shader->reflection.cutoff_distance;
    if(!nan_f32(cutoff) && cutoff != 0.0f) {
        reflection_strength = clamp_f32((camera_center_dot - cutoff) / (shader->reflection.falloff_distance - cutoff), 0.0f, 1.0f);
    }

    if(!render_params->flags.no_zbuffer) {
        if(!is_zbuffer_disabled) {
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
            rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, alpha_blended_decal ? FALSE : TRUE);
        }
        if(alpha_blended_decal) {
            rasterizer_model_enable_depth_bias();
        } 
        else {
            rasterizer_model_disable_depth_bias();
        }
    } 
    else {
        rasterizer_dx9_set_render_state(D3DRS_ZENABLE, FALSE);
    }

    if(shader->properties.flags.two_sided != 0) {
        if(shader->properties.flags.disable_two_sided_culling == 0) {
            VectorIJK to_camera;
            to_camera.i = render_params->center_point.x - window_parameters->camera.position.x;
            to_camera.j = render_params->center_point.y - window_parameters->camera.position.y;
            to_camera.k = render_params->center_point.z - window_parameters->camera.position.z;
            if(math_vector_length(&to_camera) <= 8.0f) {
                disable_two_sided_culling = true;
            } 
            else {
                disable_two_sided_culling = false;
            }
        } 
        else {
            disable_two_sided_culling = true;
        }
    }
    rasterizer_dx9_set_render_state(D3DRS_CULLMODE, disable_two_sided_culling ? D3DCULL_CCW : D3DCULL_NONE);

    if(!is_zbuffer_disabled) {
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    }

    rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, (alpha_blended_decal || is_zbuffer_disabled) ? TRUE : FALSE);
    rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    bool alpha_test_enable = !is_zbuffer_disabled && !alpha_blended_decal && !shader->properties.flags.not_alpha_tested;
    rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, alpha_test_enable ? TRUE : FALSE);
    rasterizer_dx9_set_render_state(D3DRS_ALPHAREF, 0x7F);

    bool fog_enable;
    if(d3d_caps->PixelShaderVersion >= D3DPS_VERSION(1, 4)) {
        fog_enable = shader->properties.flags.true_atmospheric_fog;
    }
    else {
        fog_enable = rasterizer_model_is_fog_enabled();
    }
    rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, fog_enable ? TRUE : FALSE);

    rasterizer_dx9_set_pixel_shader(pixel_shader);
    rasterizer_dx9_set_texture_stage_state(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    rasterizer_dx9_set_texture_stage_state(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    rasterizer_dx9_texture_set_bitmap_data_texture(0, 0, 1, texture_index, shader->maps.base_map.tag_handle);
    rasterizer_dx9_texture_set_bitmap_data_texture(1, 0, 2, texture_index, shader->maps.detail_map.tag_handle);
    rasterizer_dx9_texture_set_bitmap_data_texture(2, 0, 1, texture_index, shader->maps.multipurpose_map.tag_handle);
    rasterizer_dx9_texture_set_bitmap_data_texture(3, 2, 0, texture_index, shader->reflection.cube_map.tag_handle);

    ColorRGB self_illumination_color = {0};
    float illumination_color_progress = 1.0f;
    float animation_phase = 0.0f;
    PixelShaderModelConstants ps_constants = {0};

    if(d3d_caps->PixelShaderVersion > D3DPS_VERSION(1, 3) || shader->maps.detail_mask == SHADER_MODEL_DETAIL_MASK_NONE) {
        illumination_color_progress = 1.0f;
        if(shader->self_illumination.flags.no_random_phase == 0) {
            uint32_t rng_seed = render_params->count;
            animation_phase = rng_random_seed_float(&rng_seed);
        }

        self_illumination_color.r = shader->self_illumination.animation_color[1].r - shader->self_illumination.animation_color[0].r;
        self_illumination_color.g = shader->self_illumination.animation_color[1].g - shader->self_illumination.animation_color[0].g;
        self_illumination_color.b = shader->self_illumination.animation_color[1].b - shader->self_illumination.animation_color[0].b;

        animation_phase = wave_function_calculate_value(
            frame_parameters->elapsed_time_sec / shader->self_illumination.animation_period + animation_phase,
            shader->self_illumination.animation_function
        );

        FunctionNameNullable color_source = shader->self_illumination.color_source;
        self_illumination_color.r = self_illumination_color.r * animation_phase + shader->self_illumination.animation_color[0].r;
        self_illumination_color.g = self_illumination_color.g * animation_phase + shader->self_illumination.animation_color[0].g;
        self_illumination_color.b = self_illumination_color.b * animation_phase + shader->self_illumination.animation_color[0].b;

        if(color_source > 0 && color_source < FUNCTION_NAME_NULLABLE_MAX_VALUE) {
            ColorRGB *external_color = &render_params->animation.colors[color_source - 1];
            self_illumination_color.r *= external_color->r;
            self_illumination_color.g *= external_color->g;
            self_illumination_color.b *= external_color->b;

            if(d3d_caps->PixelShaderVersion < D3DPS_VERSION(1, 4)) {
                if(shader->maps.detail_mask == SHADER_MODEL_DETAIL_MASK_NONE && color_source == FUNCTION_NAME_NULLABLE_B) {
                    float rnd = fabsf(math_vector_distance(&window_parameters->camera.position, &render_params->center_point));
                    if(!nan_f32(rnd) && rnd < 6.0f) {
                        illumination_color_progress = 1.0f - rnd / 6.0f;
                    }
                }
            }
        }

        ps_constants.self_illumination_color.r = self_illumination_color.r * illumination_color_progress;
        ps_constants.self_illumination_color.g = self_illumination_color.g * illumination_color_progress;
        ps_constants.self_illumination_color.b = self_illumination_color.b * illumination_color_progress;
        ps_constants.self_illumination_color.a = 1.0f;
    }

    ColorRGB animation_color = {0};
    FunctionNameNullable change_color_source = shader->change_color.change_color_source;
    if(change_color_source > 0 && change_color_source < FUNCTION_NAME_NULLABLE_MAX_VALUE) {
        animation_color = render_params->animation.colors[change_color_source - 1];
    } 
    else {
        animation_color = COLOR_RGB_WHITE;
    }

    bool should_use_fast_shader = false;
    VertexShaderIndex vs_index;
    if(shader->properties.flags.true_atmospheric_fog == 0) {
        if(!rasterizer_model_use_planar_fog_enabled()) {
            if(render_params->lighting.point_light_count < 1) {
                bool should_use_simple_shader = false;
                bool has_multiple_nodes = (render_params->nodes_count > 1);
                bool has_multipurpose_map = (shader->maps.multipurpose_map.tag_handle.value != 0xffffffffU);
                bool has_detail_mask = (shader->maps.detail_mask != SHADER_MODEL_DETAIL_MASK_NONE);
                bool illum_color_is_nan_or_zero =
                    (nan_f32(self_illumination_color.r) || self_illumination_color.r == 0.0f) ||
                    (nan_f32(self_illumination_color.g) || self_illumination_color.g == 0.0f) ||
                    (nan_f32(self_illumination_color.b) || self_illumination_color.b == 0.0f);

                bool anim_color_is_nan_or_one =
                    (nan_f32(animation_color.r) || animation_color.r == 1.0f) ||
                    (nan_f32(animation_color.g) || animation_color.g == 1.0f) ||
                    (nan_f32(animation_color.b) || animation_color.b == 1.0f);

                if(has_multiple_nodes || (has_multipurpose_map && (has_detail_mask || illum_color_is_nan_or_zero || anim_color_is_nan_or_one))) {
                    should_use_simple_shader = true;
                }
                if(should_use_simple_shader || (!HANDLE_IS_NULL(shader->reflection.cube_map.tag_handle) && reflection_strength > 0.0f)) {
                    should_use_fast_shader = true;
                } 
                else {
                    vs_index = VSH_MODEL_SCENERY;
                }
            } 
            else {
                vs_index = VSH_MODEL;
            }
        } 
        else {
            vs_index = VSH_MODEL_FOGGED;
        }
    } 
    else {
        should_use_fast_shader = true;
    }

    if(should_use_fast_shader) {
        vs_index = VSH_MODEL_FAST;
    }

    ColorARGB fog_blend_base_color = {0};
    ColorRGB fog_blend_extinction_color = {0};
    ColorRGB fog_blend_additive_color = {0};
    if(is_fog_enabled && shader->properties.flags.true_atmospheric_fog != 0 && d3d_caps->PixelShaderVersion < D3DPS_VERSION(1, 4)) {
        rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, TRUE);
        fog_blend_additive_color.r = 0.0f;
        fog_blend_additive_color.g = 0.0f;
        fog_blend_additive_color.b = 0.0f;
        fog_blend_base_color.a = 1.0f;
        fog_blend_base_color.r = 0.0f;
        fog_blend_base_color.g = 0.0f;
        fog_blend_base_color.b = 0.0f;
        uint32_t fog_color = color_encode_r8g8b8(&window_parameters->fog.atmospheric_color);
        rasterizer_dx9_set_render_state(D3DRS_FOGCOLOR, fog_color);
    }
    else if(is_fog_enabled && !render_params->flags.no_fog) {
        Plane3D *fog_plane = &window_parameters->fog.plane;
        VectorXYZ *camera_position = &window_parameters->camera.position;
        float fog_atmospheric_maximum_distance = window_parameters->fog.atmospheric_maximum_distance;
        float fog_atmospheric_minimum_distance = window_parameters->fog.atmospheric_minimum_distance;

        float fog_plane_dist = (math_vector_dot_product(fog_plane, camera_position) - fog_plane->w) / fog_atmospheric_maximum_distance;
        fog_plane_dist = clamp_f32(fog_plane_dist, 0.0f, 1.0f);

        float center_dot = (camera_center_dot - fog_atmospheric_minimum_distance) / (fog_atmospheric_maximum_distance - fog_atmospheric_minimum_distance);
        center_dot = clamp_f32(center_dot, 0.0f, 1.0f);

        float fog_density = center_dot * window_parameters->fog.atmospheric_maximum_density;
        if(window_parameters->fog.fog_definition_flags.atmosphere_fog) {
            fog_plane_dist = 1.0f;
        }

        ColorRGB *fog_planar_color = &window_parameters->fog.planar_color;
        ColorRGB *fog_atmospheric_color = &window_parameters->fog.atmospheric_color;
        fog_blend_base_color.a = 1.0f - fog_density;
        float inv = 1.0f - fog_plane_dist;
        fog_blend_base_color.r = fog_planar_color->r - (fog_plane_dist * fog_planar_color->r + fog_atmospheric_color->r * inv) * fog_density;
        fog_blend_base_color.g = fog_planar_color->g - (fog_plane_dist * fog_planar_color->g + fog_atmospheric_color->g * inv) * fog_density;
        fog_blend_base_color.b = fog_planar_color->b - (fog_plane_dist * fog_planar_color->b + fog_atmospheric_color->b * inv) * fog_density;

        fog_blend_extinction_color.r = clamp_f32(fog_blend_base_color.r * -1.0f, 0.0f, 1.0f);
        fog_blend_extinction_color.g = clamp_f32(fog_blend_base_color.g * -1.0f, 0.0f, 1.0f);
        fog_blend_extinction_color.b = clamp_f32(fog_blend_base_color.b * -1.0f, 0.0f, 1.0f);

        fog_blend_base_color.r = clamp_f32(fog_blend_base_color.r, 0.0f, 1.0f);
        fog_blend_base_color.g = clamp_f32(fog_blend_base_color.g, 0.0f, 1.0f);
        fog_blend_base_color.b = clamp_f32(fog_blend_base_color.b, 0.0f, 1.0f);

        fog_blend_additive_color.r = window_parameters->fog.atmospheric_color.r * fog_density;
        fog_blend_additive_color.g = window_parameters->fog.atmospheric_color.g * fog_density;
        fog_blend_additive_color.b = window_parameters->fog.atmospheric_color.b * fog_density;

        if(d3d_caps->PixelShaderVersion < D3DPS_VERSION(1, 4)) {
            if(shader->maps.detail_mask != SHADER_MODEL_DETAIL_MASK_NONE && vs_index == VSH_MODEL_FOGGED) {
                float illumination_intensity = rasterizer_model_get_illumination_intensity();
                ColorRGB fog_color;
                fog_color.r = clamp_f32(fog_blend_additive_color.r - illumination_intensity * fog_blend_extinction_color.r, 0.0f, 1.0f);
                fog_color.g = clamp_f32(fog_blend_additive_color.g - illumination_intensity * fog_blend_extinction_color.g, 0.0f, 1.0f);
                fog_color.b = clamp_f32(fog_blend_additive_color.b - illumination_intensity * fog_blend_extinction_color.b, 0.0f, 1.0f);

                fog_color.r = clamp_f32(fog_color.r + fog_blend_base_color.r, 0.0f, 1.0f);
                fog_color.g = clamp_f32(fog_color.g + fog_blend_base_color.g, 0.0f, 1.0f);
                fog_color.b = clamp_f32(fog_color.b + fog_blend_base_color.b, 0.0f, 1.0f);

                rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, TRUE);
                rasterizer_dx9_set_render_state(D3DRS_FOGCOLOR, color_encode_r8g8b8(&fog_color));
            } 
            else {
                rasterizer_dx9_set_render_state(D3DRS_FOGENABLE, FALSE);
            }
        }
    }
    else {
        fog_blend_additive_color.r = 0.0f;
        fog_blend_additive_color.g = 0.0f;
        fog_blend_additive_color.b = 0.0f;
        fog_blend_base_color.a = 1.0f;
        fog_blend_base_color.r = 0.0f;
        fog_blend_base_color.g = 0.0f;
        fog_blend_base_color.b = 0.0f;
        fog_blend_extinction_color.r = 0.0f;
        fog_blend_extinction_color.g = 0.0f;
        fog_blend_extinction_color.b = 0.0f;
    }

    ps_constants.primary_color.r = animation_color.r;
    ps_constants.primary_color.g = animation_color.g;
    ps_constants.primary_color.b = animation_color.b;
    ps_constants.primary_color.a = is_zbuffer_disabled ? rasterizer_model_get_effect_opacity() : 1.0f;
    ps_constants.fog_blend_base.r = fog_blend_base_color.r;
    ps_constants.fog_blend_base.g = fog_blend_base_color.g;
    ps_constants.fog_blend_base.b = fog_blend_base_color.b;
    ps_constants.fog_blend_base.a = fog_blend_base_color.a;
    ps_constants.fog_blend_extinction.r = fog_blend_extinction_color.r; // original implementation didn't set this field?
    ps_constants.fog_blend_extinction.g = fog_blend_extinction_color.g;
    ps_constants.fog_blend_extinction.b = fog_blend_extinction_color.b;
    ps_constants.fog_blend_extinction.a = 1.0f;
    ps_constants.fog_blend_additive.r = fog_blend_additive_color.r;
    ps_constants.fog_blend_additive.g = fog_blend_additive_color.g;
    ps_constants.fog_blend_additive.b = fog_blend_additive_color.b;
    ps_constants.fog_blend_additive.a = 1.0f;

    rasterizer_dx9_set_pixel_shader_constant_f(PSH_CONSTANTS_MODEL_OFFSET, &ps_constants, PSH_CONSTANTS_MODEL_COUNT);

    IDirect3DVertexShader9 *vertex_shader = rasterizer_dx9_shader_get_vertex_shader(vs_index);
    rasterizer_dx9_set_vertex_shader(vertex_shader);

    rasterizer_dx9_set_vertex_declaration(VERTEX_DECLARATION_MODEL_UNCOMPRESSED);

    // Texture scale vertex shader constants
    VertexShaderTexscaleConstants vsh_texscale_constants;
    vsh_texscale_constants.primary_detail_scale.i = shader->maps.detail_map_scale;
    vsh_texscale_constants.primary_detail_scale.j = shader->maps.detail_map_v_scale * shader->maps.detail_map_scale;
    vsh_texscale_constants.secondary_detail_scale.i = 1.0f;
    vsh_texscale_constants.secondary_detail_scale.j = 1.0f;
    vsh_texscale_constants.base_map_transform_x.i = 1.0f;
    vsh_texscale_constants.base_map_transform_x.j = 0.0f;
    vsh_texscale_constants.base_map_transform_x.k = 0.0f;
    vsh_texscale_constants.base_map_transform_x.w = 0.0f;
    vsh_texscale_constants.base_map_transform_y.i = 0.0f;
    vsh_texscale_constants.base_map_transform_y.j = 1.0f;
    vsh_texscale_constants.base_map_transform_y.k = 0.0f;
    vsh_texscale_constants.base_map_transform_y.w = 0.0f;
    shader_texture_animation_evaluate(
        render_params->texture_scale.x * shader->maps.map_u_scale,
        render_params->texture_scale.y * shader->maps.map_v_scale,
        0.0f, 0.0f, 0.0f, frame_parameters->elapsed_time_sec,
        (ShaderTransparentMapAnimation *)&shader->texture_scrolling_animation,
        &render_params->animation,
        &vsh_texscale_constants.base_map_transform_x.i, &vsh_texscale_constants.base_map_transform_y.i
    );
    vsh_texscale_constants.translucency = shader->properties.translucency;
    rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_TEXSCALE_OFFSET, &vsh_texscale_constants, VSH_CONSTANTS_TEXSCALE_COUNT);

    // Effect vertex shader constants
    ColorARGB *ambient_reflection_tint = rasterizer_model_get_reflection_tint_color();
    VertexShaderEffectConstants vs_effect_constants = {0};
    if(ambient_reflection_tint == NULL || color_encode_a8r8g8b8(ambient_reflection_tint) == 0) {
        ShaderSpecularProperties *cube_map_properties = &shader->reflection.cube_map_properties;
        ColorRGB *perpendicular_tint_color = &cube_map_properties->perpendicular_tint_color;
        ColorRGB *parallel_tint_color = &cube_map_properties->parallel_tint_color;
        ColorARGB *reflection_tint_color = &render_params->lighting.reflection_tint_color;
        float perpendicular_brightness = cube_map_properties->perpendicular_brightness;
        float parallel_brightness = cube_map_properties->parallel_brightness;

        vs_effect_constants.parallel_tint_color.a = parallel_brightness * reflection_tint_color->a * reflection_strength;
        vs_effect_constants.parallel_tint_color.r = parallel_tint_color->r * reflection_tint_color->r;
        vs_effect_constants.parallel_tint_color.g = parallel_tint_color->g * reflection_tint_color->g;
        vs_effect_constants.parallel_tint_color.b = parallel_tint_color->b * reflection_tint_color->b;

        ColorARGB color_aux;
        color_aux.a = perpendicular_brightness * reflection_tint_color->a * reflection_strength;
        color_aux.r = perpendicular_tint_color->r * reflection_tint_color->r;
        color_aux.g = perpendicular_tint_color->g * reflection_tint_color->g;
        color_aux.b = perpendicular_tint_color->b * reflection_tint_color->b;

        vs_effect_constants.perpendicular_tint_color.a = color_aux.a - vs_effect_constants.parallel_tint_color.a;
        vs_effect_constants.perpendicular_tint_color.r = color_aux.r - vs_effect_constants.parallel_tint_color.r;
        vs_effect_constants.perpendicular_tint_color.g = color_aux.g - vs_effect_constants.parallel_tint_color.g;
        vs_effect_constants.perpendicular_tint_color.b = color_aux.b - vs_effect_constants.parallel_tint_color.b;
    }
    else {
        // Override ambient color if we have an ambient reflection tint
        vs_effect_constants.perpendicular_tint_color.a = ambient_reflection_tint->a;
        vs_effect_constants.perpendicular_tint_color.r = ambient_reflection_tint->r;
        vs_effect_constants.perpendicular_tint_color.g = ambient_reflection_tint->g;
        vs_effect_constants.perpendicular_tint_color.b = ambient_reflection_tint->b;
        vs_effect_constants.parallel_tint_color.a = COLOR_ARGB_BLACK.a;
        vs_effect_constants.parallel_tint_color.r = COLOR_ARGB_BLACK.r;
        vs_effect_constants.parallel_tint_color.g = COLOR_ARGB_BLACK.g;
        vs_effect_constants.parallel_tint_color.b = COLOR_ARGB_BLACK.b;
    }
    rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_EFFECT_OFFSET, &vs_effect_constants, VSH_CONSTANTS_EFFECT_COUNT);

    rasterizer_model_draw_triangles(param_3, param_4, param_6, param_5, param_7, NULL);

    if(shader->properties.flags.two_sided != 0 && disable_two_sided_culling) {
        vsh_texscale_constants.primary_detail_scale.i = shader->maps.detail_map_scale;
        vsh_texscale_constants.primary_detail_scale.j = shader->maps.detail_map_v_scale * shader->maps.detail_map_scale;
        vsh_texscale_constants.secondary_detail_scale.i = 1.0f;
        vsh_texscale_constants.secondary_detail_scale.j = -1.0f;
        vsh_texscale_constants.base_map_transform_x.i = 1.0f;
        vsh_texscale_constants.base_map_transform_x.j = 0.0f;
        vsh_texscale_constants.base_map_transform_x.k = 0.0f;
        vsh_texscale_constants.base_map_transform_x.w = 0.0f;
        vsh_texscale_constants.base_map_transform_y.i = 0.0f;
        vsh_texscale_constants.base_map_transform_y.j = 1.0f;
        vsh_texscale_constants.base_map_transform_y.k = 0.0f;
        vsh_texscale_constants.base_map_transform_y.w = 0.0f;

        shader_texture_animation_evaluate(
            render_params->texture_scale.x * shader->maps.map_u_scale,
            render_params->texture_scale.y * shader->maps.map_v_scale,
            0.0f, 0.0f, 0.0f, frame_parameters->elapsed_time_sec,
            (ShaderTransparentMapAnimation *)&shader->texture_scrolling_animation,
            &render_params->animation,
            &vsh_texscale_constants.base_map_transform_x.i, &vsh_texscale_constants.base_map_transform_y.i
        );

        vsh_texscale_constants.translucency = shader->properties.translucency;

        rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_TEXSCALE_OFFSET, &vsh_texscale_constants, VSH_CONSTANTS_TEXSCALE_COUNT);

        rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_CW);
        
        rasterizer_model_draw_triangles(param_3, param_4, param_6, param_5, param_7, NULL);
    }

    rasterizer_model_disable_depth_bias();
}

#include <stdio.h>
#include <math.h>
#include "../bitmap/bitmap.h"
#include "../exception/exception.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/tag.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../rasterizer/rasterizer_dx9_vertex.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../rasterizer/rasterizer_text.h"
#include "../render/render.h"
#include "../text/font.h"
#include "../text/text.h"
#include "hud.h"

extern HUDGlobals **hud_globals;

VectorXYInt hud_interface_safe_zones = { 0, 0 };

HUDGlobals *hud_get_globals(void) {
    return *hud_globals;
}

void hud_calculate_point(HUDInterfaceAnchor *absolute_placement, HUDMeterDefinition *meter_definition,
                         Bounds2D *anchor_adjustments, bool override_scale, float custom_scale, VectorXYInt *out_position) {
    
    float scale = 1.0f;
    if(override_scale && !nan_f32(custom_scale) && custom_scale != 0.0f) {
        scale = custom_scale;
    }

    ASSERT(absolute_placement != NULL);
    ASSERT(meter_definition != NULL);

    const uint16_t HUD_INTERFACE_MARGIN = 8;
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();
    uint16_t left_offset = HUD_INTERFACE_MARGIN + hud_interface_safe_zones.x;
    uint16_t top_offset = HUD_INTERFACE_MARGIN + hud_interface_safe_zones.y;
    uint16_t right_offset = screen_width - HUD_INTERFACE_MARGIN - hud_interface_safe_zones.x;
    uint16_t bottom_offset = screen_height - hud_interface_safe_zones.y;

    float pos_x = 0.0f;
    float pos_y = 0.0f;

    switch(*absolute_placement) {
        case HUD_INTERFACE_ANCHOR_TOP_LEFT: {
            pos_x = meter_definition->anchor_offset.x * scale + left_offset;
            pos_y = meter_definition->anchor_offset.y * scale + top_offset;
            break;
        }
        case HUD_INTERFACE_ANCHOR_TOP_RIGHT: {
            pos_x = (meter_definition->anchor_offset.x * -1.0f) * scale + right_offset;
            pos_y = meter_definition->anchor_offset.y * scale + top_offset;
            break;
        }
        case HUD_INTERFACE_ANCHOR_BOTTOM_LEFT: {
            pos_x = meter_definition->anchor_offset.x * scale + left_offset;
            pos_y = (meter_definition->anchor_offset.y * -1.0f) * scale + bottom_offset;
            break;
        }
        case HUD_INTERFACE_ANCHOR_BOTTOM_RIGHT: {
            pos_x = (meter_definition->anchor_offset.x * -1.0f) * scale + right_offset;
            pos_y = (meter_definition->anchor_offset.y * -1.0f) * scale + bottom_offset;
            break;
        }
        case HUD_INTERFACE_ANCHOR_CENTER: {
            RenderGlobals *render_globals = render_get_globals();
            pos_x = (meter_definition->anchor_offset.x * scale) + ((screen_width / 2) - render_globals->camera.viewport_bounds.left);
            pos_y = (meter_definition->anchor_offset.y * scale) + ((screen_height / 2) - render_globals->camera.viewport_bounds.top);
            break;
        }
        default: {
            CRASHF_DEBUG("Invalid HUD interface anchor: %d", *absolute_placement);
        }
    }

    // @todo: implement anchor adjustments (seems to be unused)
    if(anchor_adjustments != NULL) {
        CRASHF_DEBUG("Anchor adjustments are not implemented");
    }

    out_position->x = math_float_to_long(pos_x);
    out_position->y = math_float_to_long(pos_y);
}

void hud_calculate_bitmap_bounds(BitmapData *bitmap_data, HUDInterfaceAnchor absolute_placement, Bounds2D *screen_coords, Bounds2D *output, bool is_interface_bitmap) {
    uint16_t width_factor = 1;
    uint16_t height_factor = 1;
    if(!is_interface_bitmap) {
        width_factor = bitmap_data->width;
        height_factor = bitmap_data->height;
    }

    float width = (screen_coords->right - screen_coords->left) * width_factor;
    float height = (screen_coords->bottom - screen_coords->top) * height_factor;

    switch(absolute_placement) {
        case HUD_INTERFACE_ANCHOR_TOP_LEFT: {
            output->left = 0.0f;
            output->right = width;
            output->top = 0.0f;
            output->bottom = height;
            break;
        }
        case HUD_INTERFACE_ANCHOR_TOP_RIGHT: {
            output->left = width * -1.0f;
            output->right = 0.0f;
            output->top = 0.0f;
            output->bottom = height;
            break;
        }
        case HUD_INTERFACE_ANCHOR_BOTTOM_LEFT: {
            output->left = 0.0f;
            output->right = width;
            output->top = height * -1.0f;
            output->bottom = 0.0f;
            break;
        }
        case HUD_INTERFACE_ANCHOR_BOTTOM_RIGHT: {
            output->left = width * -1.0f;
            output->right = 0.0f;
            output->top = height * -1.0f;
            output->bottom = 0.0f;
            break;
        }
        case HUD_INTERFACE_ANCHOR_CENTER: {
            output->left = (width * -1.0f) / 2.0f;
            output->right = width / 2.0f;
            output->top = (height * -1.0f) / 2.0f;
            output->bottom = height / 2.0f;
            break;
        }
        default: {
            CRASHF_DEBUG("Invalid HUD interface anchor: %d", absolute_placement);
        }
    }
}

void hud_draw_bitmap_internal(RasterizerMeterParams *meter_params, BitmapData *bitmap, Bounds2D *texture_coords, 
                            Bounds2D *screen_coords, float rotation, ColorARGBInt color, VectorXY *scale, VectorXYInt *offset) {
    
    float sin_rotation = sin(rotation);
    float cos_rotation = cos(rotation);

    RasterizerDynamicVertex vertices[4];
    for(size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        float texture_pos_x, texture_pos_y;
        float screen_pos_x, screen_pos_y;
        if(((i + 1) & 2) == 0) {
            texture_pos_x = texture_coords->left;
            screen_pos_x = screen_coords->left;
        }
        else {
            texture_pos_x = texture_coords->right;
            screen_pos_x = screen_coords->right;
        }
        if(i < 2) {
            texture_pos_y = texture_coords->top;
            screen_pos_y = screen_coords->top;
        }
        else {
            texture_pos_y = texture_coords->bottom;
            screen_pos_y = screen_coords->bottom;
        }

        RasterizerDynamicVertex *vertex = &vertices[i];
        vertex->position.x = offset->x + math_float_to_long((screen_pos_x * cos_rotation - screen_pos_y * sin_rotation) * scale->x);
        vertex->position.y = offset->y + math_float_to_long((screen_pos_x * sin_rotation + screen_pos_y * cos_rotation) * scale->y);
        vertex->position.z = 0.0f;
        vertex->texture_pos.x = texture_pos_x;
        vertex->texture_pos.y = texture_pos_y;
        vertex->color = color;
    }

    RasterizerDynamicScreenGeometryParams screen_geometry_parameters;
    memset(&screen_geometry_parameters, 0, sizeof(screen_geometry_parameters));
    screen_geometry_parameters.meter_parameters = meter_params;
    screen_geometry_parameters.map_texture_scale[0].x = 1.0f;
    screen_geometry_parameters.map_texture_scale[0].y = 1.0f;
    screen_geometry_parameters.map_scale[0].x = 1.0f;
    screen_geometry_parameters.map_scale[0].y = 1.0f;
    screen_geometry_parameters.point_sampled = false;
    screen_geometry_parameters.framebuffer_blend_function = FRAMEBUFFER_BLEND_FUNCTION_ALPHA_MULTIPLY_ADD;
    screen_geometry_parameters.map[0] = bitmap;
    rasterizer_screen_geometry_draw(&screen_geometry_parameters, vertices);
}

void hud_draw_bitmap_with_meter(RasterizerMeterParams *meter_params, BitmapData *bitmap_data, HUDInterfaceAnchor *meter_anchor,
                                float scale, float rotation, ColorARGBInt color_mask, bool scale_meter_offset, 
                                HUDMeterDefinition *meter_definition, Bounds2D *screen_coords, bool is_interface_bitmap) {

    Bounds2D default_bounds;
    default_bounds.left = 0.0f;
    default_bounds.right = 1.0f;
    default_bounds.top = 0.0f;
    default_bounds.bottom = 1.0f;
    if(is_interface_bitmap != false) {
        default_bounds.right = bitmap_data->width;
        default_bounds.bottom = bitmap_data->height;
    }
    if(screen_coords == NULL) {
        screen_coords = &default_bounds;
    }

    VectorXY scale_vector;
    VectorXYInt offset;
    Bounds2D bitmap_bounds;
    bool should_scale = scale_meter_offset && !meter_definition->scaling_flags.dont_scale_offset;
    math_vector_2d_scale(&meter_definition->width_scale, scale, &scale_vector);
    hud_calculate_point(meter_anchor, meter_definition, NULL, should_scale, 0.0f, &offset);
    hud_calculate_bitmap_bounds(bitmap_data, *meter_anchor, screen_coords, &bitmap_bounds, is_interface_bitmap);
    hud_draw_bitmap_internal(meter_params, bitmap_data, screen_coords, &bitmap_bounds, rotation, color_mask, &scale_vector, &offset);
}

static uint8_t calculate_meter_alpha(int input_alpha, const HUDMeterDefinition *meter_data) {
    int alpha = input_alpha * meter_data->alpha_multiplier + meter_data->alpha_bias;
    alpha = clamp_i32(alpha, 0, 255);
    if(alpha < meter_data->minimum_meter_value) {
        alpha = meter_data->minimum_meter_value;
    }
    return alpha;
}

void hud_draw_meter(HUDInterfaceAnchor *anchor, uint8_t min_alpha, uint8_t max_alpha, HUDMeterDrawFlags flags, float fade, float interpolated_value, HUDMeterDefinition *meter_definition) {
    ASSERT(anchor != NULL);

    TagHandle meter_bitmap_tag_handle = meter_definition->meter_bitmap.tag_handle;
    if(HANDLE_IS_NULL(meter_bitmap_tag_handle)) {
        return;
    }

    Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, meter_bitmap_tag_handle);
    BitmapData *bitmap_data = bitmap_group_sequence_get_bitmap_for_frame(meter_bitmap_tag_handle, meter_definition->sequence_index, 0);
    if(bitmap_data == NULL) {
        return;
    }
    bool bitmap_loaded = bitmap_load(false, true, bitmap_data);
    if(!bitmap_loaded) {
        return;
    }

    Bounds2D sprite_bounds;
    int16_t sequence_index = meter_definition->sequence_index;
    if(sequence_index != -1) {
        if(sequence_index < bitmap->bitmap_group_sequence.count) {
            BitmapGroupSequence *sequence = &bitmap->bitmap_group_sequence.elements[sequence_index];
            int16_t sprites_count = sequence->sprites.count;
            if(sprites_count != 0) {
                BitmapGroupSprite *sprite = &sequence->sprites.elements[0];
                sprite_bounds.left = sprite->left;
                sprite_bounds.top = sprite->top;
                sprite_bounds.right = sprite->right;
                sprite_bounds.bottom = sprite->bottom;
            }
        }
    }

    int8_t meter_min_alpha = calculate_meter_alpha(min_alpha, meter_definition);
    int8_t meter_max_alpha = calculate_meter_alpha(max_alpha, meter_definition);

    float scale = 1.0f;
    if(meter_definition->scaling_flags.use_high_res_scale) {
        scale = 0.5f;
    }

    RasterizerMeterParams meter_params;

    if(!flags.disabled) {
        if(!meter_definition->flags.use_min_max_for_state_changes) {
            ColorARGB flash_color;
            color_decode_a8r8g8b8(meter_definition->flash_color, &flash_color);
            float flash_color_opacity = 0.0f;
            if(!nan_f32(fade)) {
                flash_color_opacity = clamp_f32(1.0f - fade, 0.0f, 1.0f);
            }
            flash_color.a = meter_max_alpha / 255.0f;
            flash_color.r *= flash_color_opacity;
            flash_color.g *= flash_color_opacity;
            flash_color.b *= flash_color_opacity;

            meter_params.flash_color = color_encode_a8r8g8b8(&flash_color);
            meter_params.gradient_min_color = (meter_definition->color_at_meter_minimum & 0xFFFFFF) | (meter_min_alpha << 24);
            meter_params.gradient_max_color = meter_definition->color_at_meter_maximum & 0xFFFFFF;
        }
        else {
            if(!flags.flashing) {
                meter_params.gradient_max_color = meter_definition->color_at_meter_minimum & 0xFFFFFF;
                meter_params.flash_color = meter_min_alpha << 24;
                meter_params.gradient_min_color = meter_params.gradient_max_color | meter_params.flash_color;
            }
            else {
                if(meter_definition->flags.interpolate_between_min_max_flash_colors_as_state_changes) {
                    ColorRGB min_color, max_color;
                    color_decode_r8g8b8(meter_definition->color_at_meter_minimum, &min_color);
                    color_decode_r8g8b8(meter_definition->color_at_meter_maximum, &max_color);
                    float progress = 0.0f;
                    if(!nan_f32(interpolated_value)) {
                        if(!meter_definition->flags.invert_interpolation) {
                            progress = interpolated_value;
                        }
                        else {
                            progress = 1.0f - interpolated_value;
                        }
                    }
                    ColorRGB interpolated_color;
                    color_interpolate(&min_color, &max_color, &interpolated_color, 0, progress);
                    uint32_t interpolated_color_int = color_encode_r8g8b8(&interpolated_color);
                    meter_params.gradient_min_color = interpolated_color_int | meter_min_alpha << 24;
                    meter_params.gradient_max_color = interpolated_color_int;
                    meter_params.flash_color = meter_min_alpha << 24;
                }
                else {
                    meter_params.gradient_max_color = meter_definition->color_at_meter_maximum & 0xFFFFFF;
                    meter_params.flash_color = meter_min_alpha << 24;
                    meter_params.gradient_min_color = meter_params.gradient_max_color | meter_params.flash_color;
                }
            }
        }
    }
    else {
        meter_params.gradient_min_color = 0;
        meter_params.gradient_max_color = 0;
        meter_params.flash_color = 0;
    }

    ColorARGB color_mask;
    color_mask.a = meter_definition->translucency;
    color_mask.r = 1.0 - meter_definition->opacity;
    color_mask.g = color_mask.r;
    color_mask.b = color_mask.r;
    meter_params.tint_color = color_encode_a8r8g8b8(&color_mask);
    meter_params.background_color = meter_definition->empty_color;
    meter_params.tint_mode_2 = meter_definition->flags.use_xbox_shading;

    hud_draw_bitmap_with_meter(&meter_params, bitmap_data, anchor, scale, 0.0f, 0xFFFFFFFF, flags.in_multiplayer, meter_definition, &sprite_bounds, bitmap->type == BITMAP_TYPE_INTERFACE_BITMAPS);
}

void hud_draw_message(wchar_t *message, float fade) {
    HUDGlobals *hud_globals = hud_get_globals();
    
    TagHandle font = font = hud_globals->messaging_parameters.splitscreen_font.tag_handle;
    if(HANDLE_IS_NULL(font)) {
        font = font_get_default_small();
    }

    ColorARGB text_color;
    text_color.a = fade;
    text_color.r = color_rgb_hud_text.r;
    text_color.g = color_rgb_hud_text.g;
    text_color.b = color_rgb_hud_text.b;

    Rectangle2D rect;
    rect.left = hud_interface_safe_zones.x;
    rect.top = 70;
    rect.right = render_get_screen_width() - hud_interface_safe_zones.x;
    rect.bottom = rect.top + font_get_height(font);

    text_set_drawing_parameters(-1, 2, 8, font, &text_color);
    rasterizer_draw_unicode_string(&rect, NULL, NULL, 0, message);
    text_set_drawing_parameters(-1, 0, 0, font, &text_color);
}

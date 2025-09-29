#include <stdio.h>
#include <math.h>
#include "../bitmap/bitmap.h"
#include "../debug/assertion.h"
#include "../exception/exception.h"
#include "../game/game_time.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/tag.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../rasterizer/rasterizer_dx9_vertex.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../render/render.h"
#include "../font/font.h"
#include "../text/text.h"
#include "hud.h"

extern HUDGlobals **hud_globals;
extern HUDGlobalsMessagingParameters **hud_messaging_parameters;

VectorXYInt hud_interface_safe_zones = { 0, 0 };

const HUDGlobals *hud_get_globals(void) {
    return *hud_globals;
}

const HUDGlobalsMessagingParameters *hud_get_messaging_parameters(void) {
    return *hud_messaging_parameters;
}

VectorXYInt hud_get_safe_zones(void) {
    return hud_interface_safe_zones;
}

void hud_set_safe_zones(int16_t horizontal, int16_t vertical) {
    hud_interface_safe_zones.x = horizontal;
    hud_interface_safe_zones.y = vertical;
}

float hud_get_canvas_scale(void) {
    const HUDGlobals *globals = hud_get_globals();
    if(globals != NULL) {
        const HUDGlobalsMessagingParameters *messaging_parameters = &globals->messaging_parameters;
        if(messaging_parameters != NULL) {
            switch(messaging_parameters->canvas_size) {
                case HUD_INTERFACE_CANVAS_SIZE_480_P:
                    return 1.0f;
                case HUD_INTERFACE_CANVAS_SIZE_960_P:
                    return 2.0f;
            }
        }
    }
    return 1.0f;
}

uint32_t hud_get_canvas_width(void) {
    return render_get_screen_width() * hud_get_canvas_scale();
}

uint32_t hud_get_canvas_height(void) {
    return render_get_screen_height() * hud_get_canvas_scale();
}

void hud_calculate_point(HUDInterfaceAnchor *absolute_placement, HUDMeterDefinition *meter_definition,
                         HudPointCalculationMode calculation_mode, bool override_scale, float custom_scale, 
                         VectorXYInt *out_position) {
    
    float scale = 1.0f;
    if(override_scale && !nan_f32(custom_scale) && custom_scale != 0.0f) {
        scale = custom_scale;
    }

    ASSERT(absolute_placement != NULL);
    ASSERT(meter_definition != NULL);

    const uint16_t HUD_INTERFACE_MARGIN = 8;
    uint16_t screen_width, screen_height, left_offset, top_offset, right_offset, bottom_offset;
    float canvas_scale = hud_get_canvas_scale();

    switch(calculation_mode) {
        case HUD_POINT_CALCULATION_MODE_SCALED_CANVAS:
        case HUD_POINT_CALCULATION_MODE_INVERSE_SCALED_CANVAS:
            screen_width = hud_get_canvas_width();
            screen_height = hud_get_canvas_height();
            left_offset = (HUD_INTERFACE_MARGIN + hud_interface_safe_zones.x) * canvas_scale;
            top_offset = (HUD_INTERFACE_MARGIN + hud_interface_safe_zones.y) * canvas_scale;
            right_offset = screen_width - ((HUD_INTERFACE_MARGIN + hud_interface_safe_zones.x) * canvas_scale);
            bottom_offset = screen_height - (hud_interface_safe_zones.y * canvas_scale);
            break;

        case HUD_POINT_CALCULATION_MODE_BASE_CANVAS:
        default:
            screen_width = render_get_screen_width();
            screen_height = render_get_screen_height();
            left_offset = HUD_INTERFACE_MARGIN + hud_interface_safe_zones.x;
            top_offset = HUD_INTERFACE_MARGIN + hud_interface_safe_zones.y;
            right_offset = screen_width - HUD_INTERFACE_MARGIN - hud_interface_safe_zones.x;
            bottom_offset = screen_height - hud_interface_safe_zones.y;
            break;
    }

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
            exception_throw_runtime_error("Invalid HUD interface anchor: %d", *absolute_placement);
        }
    }

    out_position->x = math_float_to_long(pos_x);
    out_position->y = math_float_to_long(pos_y);

    // If using inverse scaled canvas, we need to divide the position by the canvas scale
    if(calculation_mode == HUD_POINT_CALCULATION_MODE_INVERSE_SCALED_CANVAS) {
        out_position->x /= canvas_scale;
        out_position->y /= canvas_scale;
    }
}

void hud_calculate_bitmap_bounds(HUDInterfaceAnchor absolute_placement, BitmapData *bitmap_data, Bounds2D *screen_coords, Bounds2D *output, bool is_interface_bitmap) {
    uint16_t bitmap_width = 1;
    uint16_t bitmap_height = 1;
    if(!is_interface_bitmap) {
        bitmap_width = bitmap_data->width;
        bitmap_height = bitmap_data->height;
        
        Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, bitmap_data->bitmap_tag_id);
        if(bitmap) {
            if(bitmap->flags.half_hud_scale == 1) {
                bitmap_width /= 2.0f;
                bitmap_height /= 2.0f;
            }
            if(bitmap->flags.force_hud_use_highres_scale == 1) {
                bitmap_width /= 2.0f;
                bitmap_height /= 2.0f;
            }
        }
    }

    float width = (screen_coords->right - screen_coords->left) * bitmap_width;
    float height = (screen_coords->bottom - screen_coords->top) * bitmap_height;

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
            exception_throw_runtime_error("Invalid HUD interface anchor: %d", absolute_placement);
        }
    }
}

void hud_draw_bitmap_internal(const RasterizerMeterParams *meter_params, const BitmapData *bitmap, const Bounds2D *texture_bounds, 
                            const Bounds2D *screen_coords, float rotation, Pixel32 color, const VectorXY *scale, const VectorXYInt *offset) {

    float sin_rotation = sin(rotation);
    float cos_rotation = cos(rotation);

    RasterizerDynamicVertex vertices[4];
    for(size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        float texture_pos_x, texture_pos_y;
        float screen_pos_x, screen_pos_y;
        if(((i + 1) & 2) == 0) {
            texture_pos_x = texture_bounds->left;
            screen_pos_x = screen_coords->left;
        }
        else {
            texture_pos_x = texture_bounds->right;
            screen_pos_x = screen_coords->right;
        }
        if(i < 2) {
            texture_pos_y = texture_bounds->top;
            screen_pos_y = screen_coords->top;
        }
        else {
            texture_pos_y = texture_bounds->bottom;
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
    rasterizer_screen_geometry_draw_with_custom_canvas(&screen_geometry_parameters, vertices, hud_get_canvas_width(), hud_get_canvas_height());
}

void hud_draw_bitmap_with_meter(RasterizerMeterParams *meter_params, BitmapData *bitmap_data, HUDInterfaceAnchor *meter_anchor,
                                float scale, float rotation, Pixel32 color_mask, bool scale_meter_offset, 
                                HUDMeterDefinition *meter_definition, Bounds2D *sprite_texture_bounds, bool is_interface_bitmap) {

    Bounds2D default_bounds;
    default_bounds.left = 0.0f;
    default_bounds.right = 1.0f;
    default_bounds.top = 0.0f;
    default_bounds.bottom = 1.0f;
    if(is_interface_bitmap != false) {
        default_bounds.right = bitmap_data->width;
        default_bounds.bottom = bitmap_data->height;
    }
    if(sprite_texture_bounds == NULL) {
        sprite_texture_bounds = &default_bounds;
    }

    VectorXY scale_vector;
    VectorXYInt offset;
    Bounds2D bitmap_bounds;
    bool should_scale = scale_meter_offset && !meter_definition->scaling_flags.dont_scale_offset;
    math_vector_2d_scale(&meter_definition->width_scale, scale, &scale_vector);
    hud_calculate_point(meter_anchor, meter_definition, true, should_scale, 0.0f, &offset);
    hud_calculate_bitmap_bounds(*meter_anchor, bitmap_data, sprite_texture_bounds, &bitmap_bounds, is_interface_bitmap);
    hud_draw_bitmap_internal(meter_params, bitmap_data, sprite_texture_bounds, &bitmap_bounds, rotation, color_mask, &scale_vector, &offset);
}

void hud_draw_bitmap(HUDInterfaceAnchor anchor, VectorXYInt *offset, float scale, float rotation, Pixel32 color, 
                        Bounds2D *param_4, Bounds2D *sprite_texture_bounds, bool is_interface_bitmap, BitmapData *bitmap_data) {
    
    Bounds2D default_bounds;
    default_bounds.left = 0.0f;
    default_bounds.right = 1.0f;
    default_bounds.top = 0.0f;
    default_bounds.bottom = 1.0f;
    if(is_interface_bitmap != false) {
        default_bounds.right = bitmap_data->width;
        default_bounds.bottom = bitmap_data->height;
    }
    if(sprite_texture_bounds == NULL) {
        sprite_texture_bounds = &default_bounds;
    }

    VectorXY scale_vector = { scale, scale };
    Bounds2D bitmap_bounds;
    hud_calculate_bitmap_bounds(anchor, bitmap_data, sprite_texture_bounds, &bitmap_bounds, is_interface_bitmap);
    hud_draw_bitmap_internal(NULL, bitmap_data, sprite_texture_bounds, &bitmap_bounds, rotation, color, &scale_vector, offset);
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

    void *texture = bitmap_load(false, true, bitmap_data);
    if(texture == NULL) {
        return;
    }

    Bounds2D *sprite_bounds = NULL;
    int16_t sequence_index = meter_definition->sequence_index;
    if(sequence_index != -1) {
        if(sequence_index < bitmap->bitmap_group_sequence.count) {
            BitmapGroupSequence *sequence = &bitmap->bitmap_group_sequence.elements[sequence_index];
            int16_t sprites_count = sequence->sprites.count;
            if(sprites_count != 0) {
                BitmapGroupSprite *sprite = &sequence->sprites.elements[0];
                sprite_bounds = (Bounds2D *)&sprite->left;
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
                    color_rgb_interpolate(&min_color, &max_color, &interpolated_color, 0, progress);
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

    hud_draw_bitmap_with_meter(&meter_params, bitmap_data, anchor, scale, 0.0f, 0xFFFFFFFF, flags.in_multiplayer, meter_definition, sprite_bounds, bitmap->type == BITMAP_TYPE_INTERFACE_BITMAPS);
}

void hud_draw_message(wchar_t *message, float fade) {
    const HUDGlobals *hud_globals = hud_get_globals();
    
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
    text_draw_unicode_string(&rect, NULL, NULL, 0, message);
    text_set_drawing_parameters(-1, 0, 0, font, &text_color);
}

void hud_draw_message_icon(Rectangle2D *text_bounds, Pixel32 color, HUDInterfaceMessagingIcon *icon, Bounds2D *input_bounds) {
    const HUDGlobals *hud_globals = hud_get_globals();
    const PlayersGlobals *players_globals = player_get_globals();
    
    uint16_t frame_index;    
    if(icon->frame_rate == '\0') {
        frame_index = 0;
    }
    else {
        frame_index = game_time_get_local_time() / (int)icon->frame_rate;
    }

    BitmapData *bitmap;
    Bounds2D *bounds;
    TagHandle icon_bitmap = hud_globals->messaging_parameters.icon_bitmap.tag_handle;
    bitmap_group_sequence_get_bitmap_and_bounds_for_frame(icon_bitmap, icon->sequence_index, &bitmap, &bounds, frame_index);
    if(bitmap == NULL || bitmap_load(false, true, bitmap) == NULL) {
        return;
    }

    float scale = 1.0f; 
    if(players_globals->local_player_count > 1) {
        scale = 0.75f; // some day!
    }

    int16_t pos_x = text_bounds->left + icon->offset_from_reference_corner.x * scale;
    int16_t pos_y = text_bounds->bottom - icon->offset_from_reference_corner.y * scale;

    VectorXYInt offset;
    offset.x = pos_x * hud_get_canvas_scale();
    offset.y = pos_y * hud_get_canvas_scale();

    Pixel32 final_color = color;
    if(icon->flags.override_default_color) {
        final_color = icon->override_icon_color;
    }

    hud_draw_bitmap(HUD_INTERFACE_ANCHOR_BOTTOM_LEFT, &offset, scale, 0.0f, final_color, input_bounds, bounds, false, bitmap);

    if(icon->flags.width_offset_is_absolute_icon_width != 0) {
        text_bounds->left = icon->width_offset * scale + pos_x;
    }
    else if(bounds != NULL) {
        text_bounds->left = ((bounds->right - bounds->left) * bitmap->width + icon->width_offset) * scale + pos_x;
    }
    else {
        text_bounds->left = (bitmap->width + icon->width_offset) * scale + pos_x;
    }
}

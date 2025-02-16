#ifndef RINGWORLD__INTERFACE__HUD_H
#define RINGWORLD__INTERFACE__HUD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/weapon_hud_interface.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../memory/memory.h"

typedef struct HUDMeterDefinition { 
    VectorXYInt anchor_offset;
    float width_scale;
    float height_scale;
    HUDInterfaceScalingFlags scaling_flags;
    char pad1[2];
    char pad2[20];
    TagReference meter_bitmap;
    ColorARGBInt color_at_meter_minimum;
    ColorARGBInt color_at_meter_maximum;
    ColorARGBInt flash_color;
    ColorARGBInt empty_color;
    HUDInterfaceMeterFlags flags;
    int8_t minimum_meter_value;
    uint16_t sequence_index;
    int8_t alpha_multiplier;
    int8_t alpha_bias;
    int16_t value_scale;
    float opacity;
    float translucency;
    ColorARGBInt disabled_color;
} HUDMeterDefinition;
_Static_assert(sizeof(HUDMeterDefinition) == 0x58);

typedef struct HUDMeterDrawFlags {
    bool flashing : 1;
    bool disabled : 1;
    bool in_multiplayer : 1;
    uint8_t pad0[3];
} HUDMeterDrawFlags;
_Static_assert(sizeof(HUDMeterDrawFlags) == 4);

/**
 * Calculate the position of a HUD element on the screen.
 * @param absolute_placement The absolute placement of the HUD element.
 * @param meter_definition The definition of the meter.
 * @param anchor_adjustments Adjustments to the anchor.
 * @param override_scale Whether to override the scale.
 * @param custom_scale The custom scale to use.
 * @param out_position The output position of the HUD element.
 */
void hud_calculate_point(HUDInterfaceAnchor *absolute_placement, HUDMeterDefinition *meter_definition, Bounds2D *anchor_adjustments, bool override_scale, float custom_scale, VectorXYInt *out_position);

/**
 * Calculate the bounds of a HUD bitmap.
 * @param bitmap_data The bitmap data.
 * @param absolute_placement The absolute placement of the HUD element.
 * @param screen_coords The screen coordinates of the HUD element.
 * @param output The output bounds of the HUD bitmap.
 * @param is_interface_bitmap Whether the bitmap is an interface bitmap.
 */
void hud_calculate_bitmap_bounds(BitmapData *bitmap_data, HUDInterfaceAnchor absolute_placement, Bounds2D *screen_coords, Bounds2D *output, bool is_interface_bitmap);

/**
 * Draw a HUD bitmap on the screen.
 * @param meter_params Parameters for drawing the meter; can be NULL if not applicable.
 * @param bitmap The bitmap to be drawn.
 * @param texture_coords The texture coordinates of the bitmap within the texture atlas.
 * @param screen_coords The screen coordinates where the bitmap should be drawn.
 * @param rotation The rotation angle of the bitmap in radians.
 * @param color The color mask to apply to the bitmap for tinting.
 * @param scale The scaling factors for the bitmap in both x and y directions.
 * @param position The position on the screen where the bitmap should be drawn.
 */
void hud_draw_bitmap_internal(RasterizerMeterParams *meter_params, BitmapData *bitmap, Bounds2D *texture_coords, Bounds2D *screen_coords, float rotation, ColorARGBInt color, VectorXY *scale, VectorXYInt *position);

/**
 * Draw a HUD bitmap with a meter on the screen.
 * @param meter_params Parameters for drawing the meter; can be NULL if not applicable.
 * @param bitmap_data The bitmap data to be drawn.
 * @param meter_anchor The anchor point for positioning the meter.
 * @param scale The scaling factor to apply to the bitmap.
 * @param rotation The rotation angle of the bitmap in radians.
 * @param color_mask The color mask to apply to the bitmap for tinting.
 * @param scale_meter_offset Whether to apply scaling to the meter offset.
 * @param meter_definition The definition and properties of the meter.
 * @param screen_coords The screen coordinates where the bitmap should be drawn.
 * @param is_interface_bitmap Whether the bitmap is part of the user interface.
 */
void hud_draw_bitmap_with_meter(RasterizerMeterParams *meter_params, BitmapData *bitmap_data, HUDInterfaceAnchor *meter_anchor, float scale, float rotation, ColorARGBInt color_mask, bool scale_meter_offset, HUDMeterDefinition *meter_definition, Bounds2D *screen_coords, bool is_interface_bitmap);

#ifdef __cplusplus
}
#endif

#endif

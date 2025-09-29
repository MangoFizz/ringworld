#ifndef RINGWORLD__INTERFACE__HUD_H
#define RINGWORLD__INTERFACE__HUD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../player/player.h"
#include "../tag/definitions/weapon_hud_interface.h"
#include "../tag/definitions/hud_globals.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../types/types.h"
#include "../memory/memory.h"

static ColorRGB color_rgb_hud_text = { 0.4588235f, 0.7294118f, 1.0f };

typedef struct HUDMeterDefinition { 
    VectorXYInt anchor_offset;
    float width_scale;
    float height_scale;
    HUDInterfaceScalingFlags scaling_flags;
    char pad1[2];
    char pad2[20];
    TagReference meter_bitmap;
    Pixel32 color_at_meter_minimum;
    Pixel32 color_at_meter_maximum;
    Pixel32 flash_color;
    Pixel32 empty_color;
    HUDInterfaceMeterFlags flags;
    int8_t minimum_meter_value;
    uint16_t sequence_index;
    int8_t alpha_multiplier;
    int8_t alpha_bias;
    int16_t value_scale;
    float opacity;
    float translucency;
    Pixel32 disabled_color;
} HUDMeterDefinition;
_Static_assert(sizeof(HUDMeterDefinition) == 0x58);

typedef struct HUDMeterDrawFlags {
    bool flashing : 1;
    bool disabled : 1;
    bool in_multiplayer : 1;
    uint8_t pad0[3];
} HUDMeterDrawFlags;
_Static_assert(sizeof(HUDMeterDrawFlags) == 4);

typedef struct HudMessage {
    int32_t time_displayed;
    wchar_t text[63];
    bool display;
    uint8_t display_index;
    int32_t next_message;
    uint32_t unk1;
} HudMessage; 
_Static_assert(sizeof(HudMessage) == 0x8C);

typedef struct HudMessagingActionPrompt {
    wchar_t message_buffer[256];
    union {
        HUDInterfaceMessagingIcon *icon;
        int16_t string_index;
    };
    uint32_t unk1[8]; // this seems to be another union like above 
    void *state;
    uint32_t unk2;
} HudMessagingActionPrompt;

typedef struct HudMessagingPlayerData {
    HudMessage messages[4];
    HudMessagingActionPrompt action_prompt;
    uint32_t unk1;
} HudMessagingPlayerData;

typedef struct HudMessagingGlobals {
    HudMessagingPlayerData player_messages[MAX_LOCAL_PLAYERS];
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    void *unk4;
    void *unk5;
    uint32_t unk6;
    uint32_t unk7[4]; // I give up for now
} HudMessagingGlobals;
_Static_assert(sizeof(HudMessagingGlobals) == 0x488);

typedef enum PACKED_ENUM HudPointCalculationMode {
    HUD_POINT_CALCULATION_MODE_BASE_CANVAS = 0,
    HUD_POINT_CALCULATION_MODE_SCALED_CANVAS,
    HUD_POINT_CALCULATION_MODE_INVERSE_SCALED_CANVAS,
    HUD_POINT_CALCULATION_MODE_MAX_VALUE,
    HUD_POINT_CALCULATION_MODE_SIZE = 0xFF
} HudPointCalculationMode;
_Static_assert(sizeof(HudPointCalculationMode) == sizeof(uint8_t));

/**
 * Get the global HUD settings.
 * @return A pointer to the global HUD settings.
 */
const HUDGlobals *hud_get_globals(void);

/**
 * Get the global HUD messaging parameters.
 * @return A pointer to the global HUD messaging parameters.
 */
const HUDGlobalsMessagingParameters *hud_get_messaging_parameters(void);

/**
 * Get the HUD safe zones.
 * @return The HUD safe zones.
 */
VectorXYInt hud_get_safe_zones(void);

/**
 * Set the HUD safe zones.
 * @param horizontal The horizontal safe zone in pixels.
 * @param vertical The vertical safe zone in pixels.
 */
void hud_set_safe_zones(int16_t horizontal, int16_t vertical);

/**
 * Initialize the HUD resources.
 */
void hud_initialize(void);

/**
 * Initialize the HUD for a new map.
 */
void hud_initialize_for_new_map(void);

/**
 * Calculate the position of a HUD element on the screen.
 * @param absolute_placement The absolute placement of the HUD element.
 * @param meter_definition The definition of the meter.
 * #@param anchor_adjustments Adjustments to the anchor.
 * @param calculation_mode The calculation mode to use for positioning.
 * @param override_scale Whether to override the scale.
 * @param custom_scale The custom scale to use.
 * @param out_position The output position of the HUD element.
 * @note The anchor adjustments parameter is currently not implemented, as 
 *       it seems to be unused in the original code. The parameter has been
 *       reused for the calculation_mode instead.
 */
void hud_calculate_point(HUDInterfaceAnchor *absolute_placement, HUDMeterDefinition *meter_definition, HudPointCalculationMode calculation_mode, bool override_scale, float custom_scale, VectorXYInt *out_position);

/**
 * Calculate the bounds of a HUD bitmap.
 * @param absolute_placement The absolute placement of the HUD element.
 * @param bitmap_data The bitmap data.
 * @param screen_coords The screen coordinates of the HUD element.
 * @param output The output bounds of the HUD bitmap.
 * @param is_interface_bitmap Whether the bitmap is an interface bitmap.
 */
void hud_calculate_bitmap_bounds(HUDInterfaceAnchor absolute_placement, BitmapData *bitmap_data, Bounds2D *screen_coords, Bounds2D *output, bool is_interface_bitmap);

/**
 * Draw a HUD bitmap on the screen.
 * @param meter_params Parameters for drawing the meter; can be NULL if not applicable.
 * @param bitmap The bitmap to be drawn.
 * @param texture_bounds The rectangle from the texture that will be rendered.
 * @param screen_coords The screen coordinates where the bitmap should be drawn.
 * @param rotation The rotation angle of the bitmap in radians.
 * @param color The color mask to apply to the bitmap for tinting.
 * @param scale The scaling factors for the bitmap in both x and y directions.
 * @param position The position on the screen where the bitmap should be drawn.
 */
void hud_draw_bitmap_internal(const RasterizerMeterParams *meter_params, const BitmapData *bitmap, const Bounds2D *texture_bounds, const Bounds2D *screen_coords, float rotation, Pixel32 color, const VectorXY *scale, const VectorXYInt *position);

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
 * @param sprite_texture_bounds The rectangle from the texture corresponding to the sprite to be rendered.
 * @param is_interface_bitmap Whether the bitmap is part of the user interface.
 */
void hud_draw_bitmap_with_meter(RasterizerMeterParams *meter_params, BitmapData *bitmap_data, HUDInterfaceAnchor *meter_anchor, float scale, float rotation, Pixel32 color_mask, bool scale_meter_offset, HUDMeterDefinition *meter_definition, Bounds2D *sprite_texture_bounds, bool is_interface_bitmap);

#ifdef __cplusplus
}
#endif

#endif

#ifndef RINGWORLD__RASTERIZER__RASTERIZER_MOTION_SENSOR_H
#define RINGWORLD__RASTERIZER__RASTERIZER_MOTION_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"

/**
 * Begins rendering the motion sensor blip.
 */
void rasterizer_hud_motion_sensor_blip_begin(void);

/**
 * Draws a blip on the motion sensor.
 * @param opacity The opacity of the blip.
 * @param blip_size The size of the blip.
 * @param position The position of the blip.
 * @param color_tint The color tint of the blip.
 */
void rasterizer_hud_motion_sensor_blip_draw(float opacity, float blip_size, VectorXY *position, ColorRGB *color_tint);

/**
 * Ends rendering of the motion sensor blips and draws the motion sensor sweep.
 * @param sweep_progress The progress of the sweep.
 * @param offset The offset of the blip.
 */
void rasterizer_hud_motion_sensor_blip_end(float sweep_progress, VectorXY *offset);

#ifdef __cplusplus
}
#endif

#endif

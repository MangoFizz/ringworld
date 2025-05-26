#include <math.h>
#include <stdbool.h>

#include "../bitmap/bitmap.h"
#include "../math/math.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../render/render.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/tag.h"
#include "ui_cursor.h"

#define UI_CURSOR_ACCELERATION 0.025
#define UI_CURSOR_SIZE_IN_PIXELS 32

extern int32_t *menu_cursor_x;
extern int32_t *menu_cursor_y;
extern int32_t *mouse_x_delta;
extern int32_t *mouse_y_delta;
extern uint8_t *cursor_should_activate_widgets;

TagHandle cursor_bitmap_tag_handle = NULL_HANDLE;
float ui_cursor_sensitivity_value = 0.75;
float ui_cursor_size = 28;

void ui_cursor_displace(int32_t dx, int32_t dy) {
    if(dx == 0 && dy == 0) {
        *cursor_should_activate_widgets = false;
        return;
    }

    int32_t new_position_x = *menu_cursor_x + dx;
    int32_t new_position_y = *menu_cursor_y + dy;

    *menu_cursor_x = max_i32(min_i32(new_position_x, render_get_screen_width()), 0);
    *menu_cursor_y = max_i32(min_i32(new_position_y, render_get_screen_height()), 0);

    *cursor_should_activate_widgets = true;
}

void ui_cursor_update(void) {
    double x_delta = (double)(*mouse_x_delta) * ui_cursor_sensitivity_value;
    double y_delta = (double)(*mouse_y_delta) * ui_cursor_sensitivity_value;

    x_delta *= (1.0 + fabs(x_delta) * UI_CURSOR_ACCELERATION);
    y_delta *= (1.0 + fabs(y_delta) * UI_CURSOR_ACCELERATION);

    if(x_delta * x_delta < 1) {
        x_delta = *mouse_x_delta;
    }
    if(y_delta * y_delta < 1) {
        y_delta = *mouse_y_delta;
    }

    // move the cursor, and 'activate' widgets if we moved the cursor
    ui_cursor_displace(+x_delta, -y_delta);
}

VectorXYInt ui_cursor_get_position(void) {
    VectorXYInt position = { *menu_cursor_x, *menu_cursor_y };
    return position;
}

void ui_cursor_set_bitmap_tag_handle(TagHandle tag_handle) {
    cursor_bitmap_tag_handle = tag_handle;
}

void ui_cursor_render(void) {
    VectorXYInt cursor_position = ui_cursor_get_position();
    Rectangle2D bounds;
    bounds.left = cursor_position.x;
    bounds.top = cursor_position.y;
    if(!HANDLE_IS_NULL(cursor_bitmap_tag_handle)) {
        BitmapData *cursor_bitmap = bitmap_group_sequence_get_bitmap_for_frame(cursor_bitmap_tag_handle, 0, 0);
        if(cursor_bitmap != NULL) {
            bounds.right = cursor_position.x + ui_cursor_size;
            bounds.bottom = cursor_position.y + ui_cursor_size;
            bitmap_draw_in_rect(cursor_bitmap, NULL, 0xFFFFFFFF, &bounds, NULL);
            return;
        }
    }
    bounds.right = cursor_position.x + 16;
    bounds.bottom = cursor_position.y + 16;
    rasterizer_screen_geometry_draw_quad(&bounds, 0x80FF0000);
}

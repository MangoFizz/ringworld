#include <math.h>
#include <stdbool.h>

#include <ringworld/math/math.h>
#include <ringworld/input/mouse.h>

extern int32_t *menu_cursor_x;
extern int32_t *menu_cursor_y;
extern uint8_t *cursor_should_activate_widgets;

// to be changed dynamically when we have widescreen support
#define UI_WIDTH 640
#define UI_HEIGHT 480

// base acceleration factor in the menu
#define MENU_MOUSE_ACCELERATION 0.025

float menu_mouse_sensitivity_value = 0.6;

static bool move_cursor(int32_t dx, int32_t dy) {
    if(dx == 0 && dy == 0) {
        return false;
    }

    int32_t new_position_x = *menu_cursor_x + dx;
    int32_t new_position_y = *menu_cursor_y + dy;

    *menu_cursor_x = max_i32(min_i32(new_position_x, UI_WIDTH), 0);
    *menu_cursor_y = max_i32(min_i32(new_position_y, UI_HEIGHT), 0);

    return true;
}

extern int32_t *mouse_x_delta;
extern int32_t *mouse_y_delta;

void update_menu_cursor_coords(void) {
    double x_delta = (double)(*mouse_x_delta) * menu_mouse_sensitivity_value;
    double y_delta = (double)(*mouse_y_delta) * menu_mouse_sensitivity_value;

    x_delta *= (1.0 + fabs(x_delta) * MENU_MOUSE_ACCELERATION);
    y_delta *= (1.0 + fabs(y_delta) * MENU_MOUSE_ACCELERATION);

    // move the cursor, and 'activate' widgets if we moved the cursor
    *cursor_should_activate_widgets = move_cursor(+x_delta, -y_delta);
}

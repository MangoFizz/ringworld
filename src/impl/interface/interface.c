#include "../game/game_globals.h"
#include "../math/color.h"
#include "../text/text.h"
#include "../terminal/termina.h"
#include "first_person_weapon.h"
#include "hud.h"
#include "interface.h"

void interface_initialize(void) {
    terminal_initialize();
    hud_initialize();
    first_person_weapon_initialize();
}

void interface_initialize_for_new_map(void) {
    hud_initialize_for_new_map();
    text_draw_initialize_for_new_map();
    first_person_weapon_initialize_for_new_map();
    TagHandle font_tag = NULL_HANDLE;
    Globals *game_globals = game_globals_get();
    if(game_globals->interface_bitmaps.elements > 0) {
        GlobalsInterfaceBitmaps *interface_bitmaps = TAG_BLOCK_GET_ELEMENT(game_globals->interface_bitmaps, 0);
        font_tag = interface_bitmaps->font_terminal.tag_handle;
    }
    text_set_drawing_parameters(-1, 0, 0, font_tag, &color_argb_white);
}

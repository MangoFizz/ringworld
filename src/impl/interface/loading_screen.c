#include <stdio.h>
#include <windows.h>
#include "../bitmap/bitmap.h"
#include "../debug/assertion.h"
#include "../font/font.h"
#include "../main/main_globals.h"
#include "../terminal/terminal.h"
#include "../time/time.h"
#include "../network/network_game.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../text/text.h"
#include "../text/unicode_string_list.h"
#include "../render/render.h"
#include "loading_screen.h"

extern uint32_t *loading_screen_state;
extern int32_t *loading_screen_time_counter_ms_0;
extern int32_t *loading_screen_time_counter_ms_1;
extern int32_t *loading_screen_time_counter_ms_2;
extern bool *loading_screen_unk_flag;
extern uint32_t *server_connection_attempts;
extern wchar_t *loading_screen_server_address;
extern wchar_t *loading_screen_map_name;
extern bool *devmode_enabled;

void loading_screen_abort_connection();
void loading_screen_timeout_connection(uint32_t counter);

void loading_screen_draw_background(float alpha) {
    TagHandle bitmap_tag = tag_lookup("ui\\shell\\bitmaps\\background", TAG_GROUP_BITMAP);
    ASSERT_OR_RETURN(!HANDLE_IS_NULL(bitmap_tag));

    Rectangle2D rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = render_get_screen_width();
    rect.bottom = render_get_screen_height();

    ColorARGB color_mask = {alpha, 1.0f, 1.0f, 1.0f};
    BitmapData *background_bitmap = bitmap_group_sequence_get_bitmap_for_frame(bitmap_tag, 0, 0);
    if(background_bitmap) {
        Rectangle2D texture_rect = rect;
        texture_rect.right = RASTERIZER_SCREEN_BASE_WIDTH;
        Pixel32 color_int = color_encode_a8r8g8b8(&color_mask);
        bitmap_draw_in_rect(background_bitmap, NULL, color_int, &rect, &texture_rect);
    }
}

void loading_screen_render(void) {
    MainGlobals *main_globals = main_get_globals();

    if(*loading_screen_state == LOADING_SCREEN_STATE_OFF) {
        return;
    }

    if(*loading_screen_time_counter_ms_1 == -1) {
        *loading_screen_time_counter_ms_1 = time_query_performance_counter_ms();
    }

    float alpha = 1.0f;

    if(*loading_screen_time_counter_ms_0 != -1) {
        uint32_t counter = time_query_performance_counter_ms();
        if(*loading_screen_time_counter_ms_0 <= counter) {
            *loading_screen_time_counter_ms_0 = -1;
            *loading_screen_time_counter_ms_1 = -1;
            *loading_screen_time_counter_ms_2 = -1;
            *loading_screen_state = LOADING_SCREEN_STATE_OFF;
            *server_connection_attempts = 0;
            loading_screen_server_address[0] = '\0';
            loading_screen_map_name[0] = '\0';
            return;
        }
        alpha = *loading_screen_time_counter_ms_0 - counter;
        if(alpha < 0) {
            alpha += UINT32_MAX;
        }
        alpha *= 0.001333333f;
        if(!nan_f32(alpha)) {
            alpha = clamp_f32(alpha, 0.0f, 1.0f);
        }
        else {
            alpha = 1.0f;
        }
    }

    if(*loading_screen_time_counter_ms_0 == -1) {
        if(*loading_screen_unk_flag) {
            switch(*loading_screen_state) {
                case LOADING_SCREEN_STATE_SERVER:
                case LOADING_SCREEN_STATE_CONNECTING:
                case LOADING_SCREEN_STATE_RETRYING:
                case LOADING_SCREEN_STATE_CONNECTED:
                case LOADING_SCREEN_STATE_WAITING_FOR_NEW_GAME: {
                    if(*devmode_enabled == false) {
                        terminal_printf(&COLOR_ARGB_WHITE, "%s", "Connect functions unavailable in -devmode.");
                    }
                    main_globals->connect_address[0] = '\0';
                    main_globals->connect_password[0] = '\0';
                    main_globals->unk12 = false;
                    network_game_issue_disconnect();
                    loading_screen_abort_connection();
                    return;
                }

                case LOADING_SCREEN_STATE_RESOLVING: {
                    if(*devmode_enabled == false) {
                        terminal_printf(&COLOR_ARGB_WHITE, "%s", "Connect functions unavailable in -devmode.");
                    }
                    main_globals->map.switch_to_structure_bsp = -1;
                    main_globals->connect_address[0] = '\0';
                    main_globals->connect_password[0] = '\0';
                    main_globals->unk12 = false;
                    main_globals->map.save_map = false;
                    main_globals->map.main_menu_scenario_load = true;
                    return;
                }

                case LOADING_SCREEN_STATE_NEGOTIATING: {
                    if(*loading_screen_time_counter_ms_2 != -1) {
                        loading_screen_timeout_connection(*loading_screen_time_counter_ms_2);
                        *loading_screen_time_counter_ms_2 = -1;
                        main_globals->map.switch_to_structure_bsp = -1;
                        main_globals->map.save_map = false;
                        main_globals->map.main_menu_scenario_load = true;
                    }
                }
            }
        }
    }

    loading_screen_draw_background(alpha);

    TagHandle font_tag = font_get_default_large();
    TagHandle strings = tag_lookup("ui\\shell\\strings\\loading", TAG_GROUP_UNICODE_STRING_LIST);
    ASSERT_OR_RETURN(!HANDLE_IS_NULL(font_tag) && !HANDLE_IS_NULL(strings));

    ColorARGB text_color = {alpha, 1.0f, 1.0f, 1.0f};
    text_set_drawing_parameters(-1, 2, 0, font_tag, &text_color);

    Rectangle2D rect;
    rect.left = 0;
    rect.top = 410;
    rect.right = render_get_screen_width();
    rect.bottom = 430;

    switch(*loading_screen_state) {
        case LOADING_SCREEN_STATE_SERVER: {
            const wchar_t *text = unicode_string_list_get_string(strings, 1);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }

        case LOADING_SCREEN_STATE_RESOLVING: 
        case LOADING_SCREEN_STATE_CONNECTING: {
            const wchar_t *format = unicode_string_list_get_string(strings, 2);
            wchar_t text[512];
            swprintf_s(text, 512, format, loading_screen_server_address);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }

        case LOADING_SCREEN_STATE_NEGOTIATING: {
            const wchar_t *format = unicode_string_list_get_string(strings, 0);
            wchar_t text[512];
            swprintf_s(text, 512, format, loading_screen_server_address);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }

        case LOADING_SCREEN_STATE_RETRYING: {
            const wchar_t *format = unicode_string_list_get_string(strings, 3);
            wchar_t text[512];
            swprintf_s(text, 512, format, loading_screen_server_address, *server_connection_attempts);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }

        case LOADING_SCREEN_STATE_CONNECTED: {
            const wchar_t *text = unicode_string_list_get_string(strings, 4);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }

        case LOADING_SCREEN_STATE_LOADING_MP_MAP: {
            int16_t text_index = 6;
            if(main_globals->game_connection != GAME_CONNECTION_NETWORK_SERVER) {
                text_index = 5;
            }
            const wchar_t *format = unicode_string_list_get_string(strings, text_index);
            wchar_t text[512];
            swprintf_s(text, 512, format, loading_screen_map_name);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }

        case LOADING_SCREEN_STATE_WAITING_FOR_NEW_GAME: {
            const wchar_t *format = unicode_string_list_get_string(strings, 8);
            wchar_t text[512];
            swprintf_s(text, 512, format, loading_screen_map_name);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
            break;
        }
    }

    switch(*loading_screen_state) {
        case LOADING_SCREEN_STATE_SERVER:
        case LOADING_SCREEN_STATE_RESOLVING:
        case LOADING_SCREEN_STATE_NEGOTIATING:
        case LOADING_SCREEN_STATE_CONNECTING:
        case LOADING_SCREEN_STATE_RETRYING:
        case LOADING_SCREEN_STATE_CONNECTED:
        case LOADING_SCREEN_STATE_WAITING_FOR_NEW_GAME: {
            rect.top = 430;
            rect.bottom = 450;
            const wchar_t *text = unicode_string_list_get_string(strings, 7);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
        }

        case LOADING_SCREEN_STATE_LOADING_MP_MAP: {
            rect.top = 460;
            rect.bottom = 480;
            const wchar_t *text = unicode_string_list_get_string(strings, 9);
            text_draw_unicode_string(&rect, NULL, NULL, 0, text);
        }
    }
}


#include <string.h>
#include "../exception/exception.h"
#include "../math/color.h"
#include "../bitmap/bitmap.h"
#include "../font/font.h"
#include "../input/input.h"
#include "../render/render.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../saved_games/player_profile.h"
#include "../saved_games/game_variant.h"
#include "../text/text.h"
#include "../text/string.h"
#include "../text/unicode_string_list.h"
#include "../time/time.h"
#include "input_event.h"
#include "ui_error_modal.h"
#include "ui_widget.h"
#include "ui_virtual_keyboard.h"

extern VirtualKeyboardGlobals *virtual_keyboard_globals;
extern wchar_t **virtual_keyboard_labels;

const ColorARGBInt TEXT_SELECTION_COLOR = 0x7F7F7F7F;

VirtualKeyboardGlobals *ui_virtual_keyboard_get_globals(void) {
    return virtual_keyboard_globals;
}

bool ui_virtual_keyboard_initialize(void) {
    virtual_keyboard_globals->active = false;
    virtual_keyboard_globals->shift_active = false;
    virtual_keyboard_globals->caps_active = false;
    virtual_keyboard_globals->symbols_active = false;
    TagHandle virtual_keyboard_tag = lookup_tag("ui\\english", TAG_GROUP_VIRTUAL_KEYBOARD);
    bool virtual_keyboard_found = false;
    if(!HANDLE_IS_NULL(virtual_keyboard_tag)) {
        virtual_keyboard_globals->keyboard = tag_get_data(TAG_GROUP_VIRTUAL_KEYBOARD, virtual_keyboard_tag);
        virtual_keyboard_globals->row = 0;
        virtual_keyboard_globals->column = 0;
        virtual_keyboard_globals->buffer_size = 0;
        virtual_keyboard_globals->last_event = -1;
        virtual_keyboard_globals->last_key = -1;
        virtual_keyboard_globals->number_of_event_repeats = 0;
        virtual_keyboard_globals->caption_index = 0;
        virtual_keyboard_globals->text_buffer = NULL;
        virtual_keyboard_globals->insertion_point = NULL;
        virtual_keyboard_globals->cursor_blink_time = 1;
        virtual_keyboard_found = true;
    }
    virtual_keyboard_globals->caret_bitmap_tag = lookup_tag("ui\\shell\\bitmaps\\white", TAG_GROUP_BITMAP);
    return virtual_keyboard_found;
}

bool ui_virtual_keyboard_launch(uint16_t buffer_size, int16_t caption_index, wchar_t *text) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    uint8_t *input_flags = input_get_flags();
    if(globals->active || globals->keyboard == NULL) {
        return false;
    }
    globals->active = true;
    globals->shift_active = false;
    globals->caps_active = false;
    globals->symbols_active = false;
    globals->row = 0;
    globals->column = 0;
    globals->buffer_size = min_i32(buffer_size, UI_VIRTUAL_KEYBOARD_BUFFER_LEN * sizeof(wchar_t));
    globals->last_event = -1;
    globals->last_key = -1;
    globals->number_of_event_repeats = 0;
    globals->caption_index = caption_index;
    globals->buffer_modified = false;
    globals->text_is_selected = true;
    globals->text_buffer = text;
    globals->insertion_point = text + wcslen(text);
    globals->cursor_blink_time = time_query_performance_counter_ms();
    wcsncpy(globals->original_buffer, text, UI_VIRTUAL_KEYBOARD_BUFFER_LEN);
    globals->original_buffer[31] = L'\0';
    globals->validate_mode = true;
    globals->caption_font_tag = font_get_default_large();
    if(buffer_size <= UI_VIRTUAL_KEYBOARD_FONT_SIZE_THRESHOLD) {
        globals->text_font_tag = font_get_default_large();
    }
    else {
        globals->text_font_tag = font_get_default_small();
    }
    *input_flags = *input_flags | 4;
    input_flush_keyboard_buffered_keys();
    input_events_flush();
    ui_widget_play_feedback_sound(FEEDBACK_SOUND_FORWARD);
    return true;
}

void ui_virtual_keyboard_render(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    VirtualKeyboard *keyboard = globals->keyboard;

    TagHandle background_bitmap_handle = keyboard->background_bitmap.tag_handle;
    if(!HANDLE_IS_NULL(background_bitmap_handle)) {
        Rectangle2D screen_rect;
        screen_rect.left = 0;
        screen_rect.top = 0;
        screen_rect.right = render_get_screen_width();
        screen_rect.bottom = render_get_screen_height();
        Rectangle2D texture_rect;
        texture_rect.left = 0;
        texture_rect.top = 0;
        texture_rect.right = RASTERIZER_SCREEN_BASE_WIDTH;
        texture_rect.bottom = RASTERIZER_SCREEN_BASE_HEIGHT;
        ColorARGBInt color = color_encode_a8r8g8b8(&color_argb_white);
        BitmapData *background_bitmap = bitmap_group_sequence_get_bitmap_for_frame(background_bitmap_handle, 0, 0);
        bitmap_draw_in_rect(background_bitmap, NULL, color, &screen_rect, &texture_rect);
    }

    ColorARGB text_color;
    text_color.a = 1.0f;
    text_color.r = 0.9f;
    text_color.g = 0.9f;
    text_color.b = 0.9f;
    text_set_drawing_parameters(-1, 2, 0, globals->caption_font_tag, &text_color);

    TagHandle special_key_labels_strings_tag = keyboard->special_key_labels_string_list.tag_handle;
    wchar_t *label = virtual_keyboard_labels[0];
    if(!HANDLE_IS_NULL(special_key_labels_strings_tag)) {
        const wchar_t *string = unicode_string_list_get_string(special_key_labels_strings_tag, globals->caption_index);
        Rectangle2D bounds;
        bounds.left = 0;
        bounds.top = 78;
        bounds.right = RASTERIZER_SCREEN_BASE_WIDTH;
        bounds.bottom = 110;
        text_draw_unicode_string(&bounds, NULL, NULL, 0, string);

        UnicodeStringList *string_list = tag_get_data(TAG_GROUP_UNICODE_STRING_LIST, special_key_labels_strings_tag);
        if(string_list->strings.count >= 15) {
            UnicodeStringListString *string = TAG_BLOCK_GET_ELEMENT(string_list->strings, 14);
            if(string->string.size > 0) {
                label = string->string.pointer;
                label[string->string.size] = L'\0';
            }
        }
    }

    text_set_drawing_parameters(-1, TEXT_JUSTIFICATION_RIGHT, 0, globals->caption_font_tag, &text_color);

    Rectangle2D bounds;
    uint16_t screen_width = RASTERIZER_SCREEN_BASE_WIDTH;
    if(render_widescreen_support_enabled()) {
        screen_width = render_get_screen_width();
    }
    bounds.left = 0;
    bounds.top = 414;
    bounds.right = screen_width - 30;
    bounds.bottom = 630;
    
    text_draw_unicode_string(&bounds, NULL, NULL, 0, label);

    bounds.top = 118;
    bounds.left = 0;
    bounds.right = render_get_screen_width();
    TagHandle large_font_tag = font_get_default_large();
    TagHandle small_font_tag = font_get_default_small();
    if(!HANDLE_IS_NULL(large_font_tag) && !HANDLE_IS_NULL(small_font_tag)) {
        if(globals->text_font_tag.value == small_font_tag.value) {
            int32_t small_font_height = font_get_height(small_font_tag);
            int32_t large_font_height = font_get_height(large_font_tag);
            bounds.top += (large_font_height - small_font_height) / 2;
        }
    }
    bounds.bottom = bounds.top + font_get_height(globals->text_font_tag);
    ui_virtual_keyboard_render_prompt(&bounds);
}

void ui_virtual_keyboard_render_input_cursor(const Rectangle2D *bounds) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    TextDrawGlobals *text_drawing_globals = text_get_drawing_globals();
    uint32_t blink_time_delta = time_query_performance_counter_ms() - globals->cursor_blink_time;
    bool should_blink = ((blink_time_delta / UI_VIRTUAL_KEYBOARD_CURSOR_BLINK_TIME) % 2) == 0;

    if(!globals->text_is_selected && !HANDLE_IS_NULL(globals->caret_bitmap_tag) && should_blink) {
        wchar_t text[globals->buffer_size >> 1];
        wchar_t *c = globals->text_buffer;
        size_t i;
        for(i = 0; c != globals->insertion_point; c++, i++) {
            text[i] = *c;
        }
        text[i] = L'\0';

        Rectangle2D cursor_bounds;
        text_drawing_globals->justification = TEXT_JUSTIFICATION_LEFT;
        text_calculate_unicode_string_draw_bounds(text, bounds, &cursor_bounds, NULL);
        math_rectangle_2d_translate(&cursor_bounds, -3, 0);

        BitmapData *caret_bitmap = bitmap_group_sequence_get_bitmap_for_frame(globals->caret_bitmap_tag, 0, 0);
        bitmap_draw_in_rect(caret_bitmap, NULL, 0xFFFFFFFF, &cursor_bounds, NULL);
    }
}

void ui_virtual_keyboard_render_prompt(const Rectangle2D *bounds) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    TextDrawGlobals *text_drawing_globals = text_get_drawing_globals();

    text_drawing_globals->font = globals->text_font_tag;
    text_drawing_globals->flags = 0;
    text_drawing_globals->style = FONT_STYLE_PLAIN;
    text_drawing_globals->justification = TEXT_JUSTIFICATION_CENTER;
    text_drawing_globals->color.a = 1.0;
    text_drawing_globals->color.b = 0.9;
    text_drawing_globals->color.r = 0.9;
    text_drawing_globals->color.g = 0.9;

    Rectangle2D text_rect;
    text_calculate_unicode_string_draw_bounds(globals->text_buffer, bounds, NULL, &text_rect);
    math_rectangle_2d_translate(&text_rect, -3, 0);

    if(globals->text_is_selected) {
        Rectangle2D selection_rect = text_rect;
        selection_rect.left -= UI_VIRTUAL_KEYBOARD_INPUT_PADDING;
        selection_rect.right += UI_VIRTUAL_KEYBOARD_INPUT_PADDING;  
        BitmapData *selection_bitmap = bitmap_group_sequence_get_bitmap_for_frame(globals->caret_bitmap_tag, 0, 0);
        if(selection_bitmap) {
            bitmap_draw_in_rect(selection_bitmap, NULL, TEXT_SELECTION_COLOR, &selection_rect, bounds);
        }
        else {
            rasterizer_screen_geometry_draw_quad(&selection_rect, TEXT_SELECTION_COLOR);
        }
    }
    
    Rectangle2D adjusted_text_rect = text_rect;
    if(tag_get_primary_group(globals->text_font_tag) == TAG_GROUP_FONT) {
        Font *font = tag_get_data(TAG_GROUP_FONT, globals->text_font_tag);
        text_drawing_globals->justification = TEXT_JUSTIFICATION_LEFT;
        adjusted_text_rect.left -= font->leading_width;
    } 
    text_draw_unicode_string(&adjusted_text_rect, NULL, NULL, 0, globals->text_buffer);

    ui_virtual_keyboard_render_input_cursor(&text_rect);
}

void ui_virtual_keyboard_reset_input_cursor_blink(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    globals->cursor_blink_time = time_query_performance_counter_ms();
}

bool ui_virtual_keyboard_cancel(void) {
    VirtualKeyboardGlobals *virtual_keyboard_globals = ui_virtual_keyboard_get_globals();
    uint8_t *input_flags = input_get_flags();

    virtual_keyboard_globals->active = false;
    if(virtual_keyboard_globals->text_buffer != NULL) {
        wcsncpy(virtual_keyboard_globals->text_buffer, virtual_keyboard_globals->original_buffer, virtual_keyboard_globals->buffer_size >> 1);
        virtual_keyboard_globals->text_buffer[(virtual_keyboard_globals->buffer_size >> 1) - 1] = L'\0';
    }
    virtual_keyboard_globals->text_buffer = NULL;
    virtual_keyboard_globals->original_buffer[0] = L'\0';
    virtual_keyboard_globals->buffer_modified = false;
    *input_flags = *input_flags & 0xFB;

    input_flush_keyboard_buffered_keys();    
}

bool ui_virtual_keyboard_backspace(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    if(globals->text_buffer == globals->insertion_point) {
        return false;
    } 
    int32_t text_size = wcslen(globals->insertion_point);
    memmove(globals->insertion_point - 1, globals->insertion_point, (text_size + 1) << 1);
    globals->insertion_point--;
    return true;
}

bool ui_virtual_keyboard_delete(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    if(globals->insertion_point[0] == L'\0') {
        return false;
    }
    int32_t text_size = wcslen(globals->insertion_point);
    memmove(globals->insertion_point, globals->insertion_point + 1, (text_size + 1) << 1);
    return true;
}

void ui_virtual_keyboard_clear_text_buffer(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    memset(globals->text_buffer, 0, globals->buffer_size);
    globals->insertion_point = globals->text_buffer;
    globals->text_is_selected = false;
}

bool ui_virtual_keyboard_insert(char character) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    if(globals->text_buffer == NULL || globals->insertion_point == NULL) {
        exception_throw_runtime_error("Virtual keyboard text buffer or insertion point is NULL");
        return false;
    }

    if(!font_character_exists(globals->text_font_tag, character)) {
        return false;
    }

    if(!ui_virtual_keyboard_validate_input_character(character, globals->validate_mode)) {
        return false;
    }

    if(globals->validate_mode == INPUT_VALIDATION_NOT_EMPTY && globals->insertion_point == globals->text_buffer) {
        if(character == ' ' || character == '\xA0') {
            // if the first character is a space or non-breaking space, do not allow it
            return false;
        }
    }

    size_t length = wcslen(globals->text_buffer);
    if(length >= (globals->buffer_size >> 1) - 1) {
        return false;
    }

    if(globals->text_is_selected) {
        ui_virtual_keyboard_clear_text_buffer();
    }

    // Insert character at the insertion point
    int32_t text_size = (int32_t)(globals->text_buffer + globals->buffer_size - globals->insertion_point);
    memmove(globals->insertion_point + 1, globals->insertion_point, (text_size + 1) << 1);
    *globals->insertion_point = character;
    globals->insertion_point++;

    return true;
}

bool ui_virtual_keyboard_move_cursor_left(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    globals->text_is_selected = false;
    if(globals->insertion_point > globals->text_buffer) {
        globals->insertion_point--;
        return true;
    }
    return false;
}

bool ui_virtual_keyboard_move_cursor_right(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    globals->text_is_selected = false;
    if(globals->insertion_point < globals->text_buffer + wcslen(globals->text_buffer)) {
        globals->insertion_point++;
        return true;
    }
    return false;
}

void ui_virtual_keyboard_save_changes(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    uint8_t *input_flags = input_get_flags();
    
    if(wcscmp(globals->text_buffer, globals->original_buffer) == 0) {
        ui_virtual_keyboard_cancel();
        globals->buffer_modified = false;
        return;
    }

    switch(globals->validate_mode) {
        case INPUT_VALIDATION_PROFILE_NAME: {
            string_trim_whitespaces(globals->text_buffer);
            if(wcslen(globals->text_buffer) == 0) {
                ui_widget_play_feedback_sound(FEEDBACK_SOUND_FLAG_FAILURE);
                ui_error_modal_show(UI_ERROR_MODAL_EMPTY_NAME, NULL_HANDLE, 1, 0); 
                ui_virtual_keyboard_cancel();
                return;
            }
            if(!saved_game_file_name_is_unique(globals->text_buffer)) {
                if(!saved_games_name_is_from_player_profile(globals->text_buffer)) {
                    ui_widget_play_feedback_sound(FEEDBACK_SOUND_FLAG_FAILURE);
                    ui_error_modal_show(UI_ERROR_MODAL_SAVE_FILE_ALREADY_EXISTS, NULL_HANDLE, 1, 0); 
                    ui_virtual_keyboard_cancel();
                    return;
                }
            }
            break;
        }

        case INPUT_VALIDATION_GAME_VARIANT_NAME: {
            string_trim_whitespaces(globals->text_buffer);
            if(!saved_game_file_name_is_unique(globals->text_buffer)) {
                if(!saved_games_name_is_from_player_profile(globals->text_buffer)) {
                    if(!game_variant_is_not_stock(globals->text_buffer)) {
                        ui_widget_play_feedback_sound(FEEDBACK_SOUND_FLAG_FAILURE);
                        ui_error_modal_show(UI_ERROR_MODAL_EMPTY_NAME, NULL_HANDLE, 1, 0); 
                        ui_virtual_keyboard_cancel();
                        return;
                    }
                }
            }
            break;
        }

        case INPUT_VALIDATION_NOT_EMPTY: {
            if(globals->text_buffer[0] == L'\0') {
                ui_virtual_keyboard_cancel();
                return;
            }
            break;
        }
        
        default:
            break;
    }

    *input_flags = *input_flags & 0xFB;
    globals->active = false;
    globals->buffer_modified = true;
    ui_widget_play_feedback_sound(FEEDBACK_SOUND_BACK);
    input_flush_keyboard_buffered_keys();
}

void ui_virtual_keyboard_process(void) {
    VirtualKeyboardGlobals *globals = ui_virtual_keyboard_get_globals();
    InputGlobals *input = input_get_globals();
    uint8_t *input_flags = input_get_flags();

    if(!globals->active || globals->keyboard == NULL) {
        return;
    }

    while((*input_flags != 1 && (*input_flags & 8) == 0 && (*input_flags & 4) != 0) && (input->buffered_key_write_index > input->buffered_key_read_index)) {
        int index = input->buffered_key_read_index++;
        BufferedKey key_event = input->buffered_keys[index];
        int8_t feedback_sound = -1;
        bool reset_cursor_blink = false;

        switch(key_event.key_code) {
            case BUFFERED_KEY_CODE_ESCAPE: {
                ui_virtual_keyboard_cancel();
                feedback_sound = FEEDBACK_SOUND_BACK;
                break;
            }
            case BUFFERED_KEY_CODE_BACKSPACE: {
                if(globals->text_is_selected) {
                    ui_virtual_keyboard_clear_text_buffer();
                    feedback_sound = FEEDBACK_SOUND_CURSOR;
                } 
                else {
                    bool success = ui_virtual_keyboard_backspace();
                    if(success) {
                        feedback_sound = FEEDBACK_SOUND_CURSOR;
                    }
                }
                reset_cursor_blink = true;
                break;
            }
            case BUFFERED_KEY_CODE_ENTER:
            case BUFFERED_KEY_CODE_NUM_ENTER: {
                ui_virtual_keyboard_save_changes();
                break;
            }
            case BUFFERED_KEY_CODE_LEFT_ARROW: {
                ui_virtual_keyboard_move_cursor_left();
                reset_cursor_blink = true;
                break;
            }
            case BUFFERED_KEY_CODE_RIGHT_ARROW: {
                ui_virtual_keyboard_move_cursor_right();
                reset_cursor_blink = true;
                break;
            }
            case BUFFERED_KEY_CODE_DEL: {
                bool success = ui_virtual_keyboard_delete();
                if(success) {
                    feedback_sound = FEEDBACK_SOUND_CURSOR;
                }
                reset_cursor_blink = true;
                break;
            }
            case BUFFERED_KEY_CODE_HOME: {
                globals->insertion_point = globals->text_buffer;
                reset_cursor_blink = true;
                break;
            }
            case BUFFERED_KEY_CODE_END: {
                globals->insertion_point = globals->text_buffer + wcslen(globals->text_buffer);
                reset_cursor_blink = true;
                break;
            }
            default: {
                uint8_t character = key_event.state;
                if(character >= 0x20 && character != 0xFF) {
                    bool success = ui_virtual_keyboard_insert(character);
                    if(success) {
                        feedback_sound = FEEDBACK_SOUND_CURSOR;
                    }
                    else {
                        feedback_sound = FEEDBACK_SOUND_FLAG_FAILURE;
                    }
                    reset_cursor_blink = true;
                }
                break;
            }
        }

        if(feedback_sound != -1) {
            ui_widget_play_feedback_sound(feedback_sound);
        }

        if(reset_cursor_blink) {
            ui_virtual_keyboard_reset_input_cursor_blink();
        }
    }
}

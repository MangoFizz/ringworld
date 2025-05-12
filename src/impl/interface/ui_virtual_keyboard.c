#include "../math/color.h"
#include "../bitmap/bitmap.h"
#include "../render/render.h"
#include "../text/text.h"
#include "../text/unicode_string_list.h"
#include "ui_widget.h"
#include "ui_virtual_keyboard.h"

extern VirtualKeyboardGlobals *virtual_keyboard_globals;
extern wchar_t **virtual_keyboard_labels;

VirtualKeyboardGlobals *ui_virtual_keyboard_get_globals(void) {
    return virtual_keyboard_globals;
}

bool ui_virtual_keyboard_initialize(void) {
    virtual_keyboard_globals->active = false;
    virtual_keyboard_globals->shift_active = false;
    virtual_keyboard_globals->caps_active = false;
    virtual_keyboard_globals->symbols_active = false;
    TagHandle vk_tag_handle = lookup_tag("ui\\english", TAG_GROUP_VIRTUAL_KEYBOARD);
    bool virtual_keyboard_found = false;
    if(!HANDLE_IS_NULL(vk_tag_handle)) {
        virtual_keyboard_globals->keyboard = tag_get_data(TAG_GROUP_VIRTUAL_KEYBOARD, vk_tag_handle);
        virtual_keyboard_globals->row = 0;
        virtual_keyboard_globals->column = 0;
        virtual_keyboard_globals->buffer_size = 0;
        virtual_keyboard_globals->last_event = -1;
        virtual_keyboard_globals->last_key = -1;
        virtual_keyboard_globals->number_of_event_repeats = 0;
        virtual_keyboard_globals->caption_index = 0;
        virtual_keyboard_globals->text_buffer = NULL;
        virtual_keyboard_globals->insertion_point = NULL;
        virtual_keyboard_globals->unk3 = 0;
        virtual_keyboard_found = true;
    }
    virtual_keyboard_globals->caret_bitmap_tag = lookup_tag("ui\\shell\\bitmaps\\white", TAG_GROUP_BITMAP);
    return virtual_keyboard_found;
}

void ui_virtual_keyboard_render(void) {
    VirtualKeyboardGlobals *virtual_keyboard_globals = ui_virtual_keyboard_get_globals();
    VirtualKeyboard *keyboard = virtual_keyboard_globals->keyboard;

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
    text_set_drawing_parameters(-1, 2, 0, virtual_keyboard_globals->caption_font_tag, &text_color);

    TagHandle special_key_labels_strings_tag = keyboard->special_key_labels_string_list.tag_handle;
    wchar_t *label = virtual_keyboard_labels[0];
    if(!HANDLE_IS_NULL(special_key_labels_strings_tag)) {
        const wchar_t *string = unicode_string_list_get_string(special_key_labels_strings_tag, virtual_keyboard_globals->caption_index);
        Rectangle2D bounds;
        bounds.left = 0;
        bounds.top = 78;
        bounds.right = render_get_screen_width();
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

    text_set_drawing_parameters(-1, 1, 0, virtual_keyboard_globals->caption_font_tag, &text_color);

    Rectangle2D bounds;
    bounds.left = 0;
    bounds.top = 414;
    bounds.right = 450;
    bounds.bottom = 630;
    text_draw_unicode_string(&bounds, NULL, NULL, 0, label);

    bounds.top = 118;
    bounds.left = 0;
    bounds.bottom = 143;
    bounds.right = render_get_screen_width();
    ui_virtual_keyboard_render_prompt(&bounds);
}

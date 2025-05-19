#include "../math/color.h"
#include "../font/font.h"
#include "../font/bitmap_font.h"
#include "../font/vector_font.h"
#include "string.h"

void string_parse_state_init(TagHandle font_tag, ColorARGB *color, const wchar_t *string, TextJustification justification, FontStyle style, StringParseState *state) {
    state->regular_font_tag = font_tag;
    state->font_data = font_get_data(font_tag, style);
    state->string = string;
    state->offset = 0;
    state->style = style;
    state->justification = justification;
    state->character = L'\0';
    state->result = -1;
    state->color = color_encode_a8r8g8b8(color);
}

StringParseResult string_parse_unicode_string(StringParseState *state) {
    uint16_t offset = state->offset;
    wchar_t character = state->string[offset];
    state->character = character;
    state->offset = offset + 1;
    switch(character) {
        case L'\0':
            state->result = STRING_PARSE_RESULT_END;
            break;
        case L'\t':
            state->result = STRING_PARSE_RESULT_TAB;
            break;
        case L'\r':
            state->result = STRING_PARSE_RESULT_LINE_BREAK;
            break;
        case L'|':
            character = state->string[offset + 1];
            state->offset = offset + 2;
            if(character == L'n') {
                state->character = L'\r';
                state->result = STRING_PARSE_RESULT_LINE_BREAK;
            }
            else if(character == L't') {
                state->character = L'\t';
                state->result = STRING_PARSE_RESULT_TAB;
            }
            else {
                state->result = STRING_PARSE_RESULT_CHARACTER;
            }
            state->offset = offset + 1;
            break;
        default:
            state->result = STRING_PARSE_RESULT_CHARACTER;
            break;
    }
    return state->result;
}

/**
 * This function is only used by the gamespy thing update dialog
 */
uint16_t string_calculate_line_break(wchar_t *string, uint32_t *width) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    StringParseState parse_state;
    string_parse_state_init(text_globals->font, &text_globals->color, string, text_globals->justification, text_globals->style, &parse_state);
    size_t string_length = wcslen(string);
    uint16_t string_width = 0;
    uint16_t offset = 0;
    while(true) {
        string_parse_unicode_string(&parse_state);
        switch(parse_state.result) {
            case STRING_PARSE_RESULT_END:
                *width -= string_width;
                return 0;
            case STRING_PARSE_RESULT_LINE_BREAK:
            case 2:
            case STRING_PARSE_RESULT_TAB:
            case 4:
            case 5:
                offset += parse_state.offset;
                break;
            case STRING_PARSE_RESULT_CHARACTER: {
                bool is_bitmap_font = tag_get_primary_group(parse_state.regular_font_tag) == TAG_GROUP_FONT;
                if(is_bitmap_font) {
                    FontCharacter *character = bitmap_font_get_character(parse_state.font_data, parse_state.character);
                    if(character != NULL) {
                        if(string_width + character->bitmap_width > *width) {
                            *width -= string_width;
                            return offset;
                        }
                        offset = parse_state.offset;
                        string_width += character->character_width;
                    }
                }
                else {
                    VectorFont *font = parse_state.font_data;
                    wchar_t character_str[2];
                    character_str[0] = parse_state.character;
                    character_str[1] = L'\0';
                    uint16_t char_width = vector_font_calculate_unicode_string_width(character_str, font, parse_state.style);
                    if(string_width + char_width > *width) {
                        *width -= string_width;
                        return offset;
                    }
                    offset = parse_state.offset;
                    string_width += char_width;
                }
            }
        }
    }
}

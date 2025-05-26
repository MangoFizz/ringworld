#ifndef RINGWORLD__TEXT_STRING_H
#define RINGWORLD__TEXT_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/memory.h"
#include "../tag/tag.h"
#include "../tag/definitions/font.h"
#include "text.h"

/**
 * @todo Investigate the other values of this enum.
 */
typedef enum PACKED_ENUM StringParseResult {
    STRING_PARSE_RESULT_END = 0,
    STRING_PARSE_RESULT_LINE_BREAK,
    STRING_PARSE_RESULT_TAB = 3,
    STRING_PARSE_RESULT_CHARACTER = 6,
    STRING_PARSE_RESULT_SIZE = 0xFFFF
} StringParseResult;

typedef struct StringParseState {
    TagHandle regular_font_tag;
    void *font_data;
    const wchar_t *string;
    int16_t offset;
    FontStyle style;
    TextJustification justification;
    wchar_t character;
    StringParseResult result;
    ColorARGBInt color; // I'm not sure if this is actually a color but what else could it be?
} StringParseState;
_Static_assert(sizeof(StringParseState) == 28);

/**
 * This function initializes the string parsing state.
 * @param font_tag The tag handle of the font to be used.
 * @param color The color of the text in ARGB format.
 * @param string The string to be parsed.
 * @param justification The justification of the text.
 * @param style The style of the text.
 * @param state A pointer to the StringParseState structure to be initialized.
 */
void string_parse_state_init(TagHandle font_tag, ColorARGB *color, const wchar_t *string, TextJustification justification, FontStyle style, StringParseState *state);

/**
 * This function parses a Unicode string and updates the state.
 * @param state A pointer to the StringParseState structure containing the string to be parsed.
 * @return The result of the parsing operation.
 */
StringParseResult string_parse_unicode_string(StringParseState *state);

/**
 * Trims leading and trailing whitespace characters from a wide character string.
 * @param str The wide character string to be trimmed.
 */
void string_trim_whitespaces(wchar_t *str);

#ifdef __cplusplus
}
#endif

#endif

#ifndef RINGWORLD__TEXT__UNICODE_STRING_LIST_H
#define RINGWORLD__TEXT__UNICODE_STRING_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"
#include "../tag/definitions/unicode_string_list.h"
#include "../types/types.h"

/**
 * Gets the Unicode string from the specified Unicode string list at the given index.
 * @param unicode_string_list_tag The tag handle of the Unicode string list.
 * @param string_index The index of the string to retrieve.
 * @return The Unicode string at the specified index; "<missing string>" if the string is not found.
 */
const wchar_t *unicode_string_list_get_string(TagHandle unicode_string_list_tag, int16_t string_index);

/**
 * Gets the Unicode string from the specified Unicode string list at the given index.
 * @param unicode_string_list_tag The tag handle of the Unicode string list.
 * @param string_index The index of the string to retrieve.
 * @return The Unicode string at the specified index; NULL if the string is not found.
 */
const wchar_t *unicode_string_list_get_string_or_null(TagHandle unicode_string_list_tag, int16_t string_index);

#ifdef __cplusplus
}
#endif

#endif

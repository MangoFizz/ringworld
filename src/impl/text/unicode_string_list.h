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
 * @param index The index of the string to retrieve.
 * @return The Unicode string at the specified index.
 */
wchar_t *unicode_string_list_get_string(TagHandle unicode_string_list_tag, size_t index);

#ifdef __cplusplus
}
#endif

#endif

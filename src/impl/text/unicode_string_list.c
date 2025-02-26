#include "../exception/exception.h"
#include "unicode_string_list.h"

const wchar_t *unicode_string_list_get_string_or_null(TagHandle unicode_string_list_tag, int16_t string_index) {
    if(!HANDLE_IS_NULL(unicode_string_list_tag)) {
        UnicodeStringList *unicode_string_list = tag_get_data(TAG_GROUP_UNICODE_STRING_LIST, unicode_string_list_tag);
        if(string_index >= 0 && string_index < unicode_string_list->strings.count) {
            UnicodeStringListString *string_list_item = TAG_BLOCK_GET_ELEMENT(unicode_string_list->strings, string_index);
            wchar_t *string = string_list_item->string.pointer;
            if(string == NULL) {
                const char *tag_path = tag_get_path(unicode_string_list_tag);
                CRASHF_DEBUG("Unicode string list string at index %d in tag %s is NULL", string_index, tag_path);
            }
            return string;
        }
    }
    return NULL;
}

const wchar_t *unicode_string_list_get_string(TagHandle unicode_string_list_tag, int16_t string_index) {
    const wchar_t *string = unicode_string_list_get_string_or_null(unicode_string_list_tag, string_index);
    if(string == NULL) {
        return L"<missing string>";
    }
    return string;
}

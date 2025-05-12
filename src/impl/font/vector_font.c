#include <stdio.h>
#include <windows.h>
#include "../exception/exception.h"
#include "../memory/table.h"
#include "../tag/definitions/vector_font.h"
#include "../tag/definitions/vector_font_data.h"
#include "../tag/tag.h"
#include "vector_font_file.h"

enum {
    VECTOR_FONT_INSTANCE_TABLE_SIZE = 32
};

typedef struct VectorFontCache {
    TagHandle tag_handle;
    HANDLE font_resource_handle;
    size_t num_fonts;
    wchar_t font_name[32];
} VectorFontCache;

MAKE_TABLE_STRUCT(VectorFontsCacheTable, VectorFontCache);

VectorFontsCacheTable *vector_fonts_cache = NULL;

void vector_font_initialize(void) {
    vector_fonts_cache = table_new("vector fonts", VECTOR_FONT_INSTANCE_TABLE_SIZE, sizeof(VectorFontCache));
}

VectorFontCache *vector_font_create_or_get_cache(TagHandle tag_handle) {
    VectorFontCache *font_cache = vector_fonts_cache->first_element;
    TABLE_FIND_ELEMENT(vector_fonts_cache, font_cache, font_cache->tag_handle.value == tag_handle.value);
    if(font_cache == NULL) {
        font_cache = table_add_element(vector_fonts_cache);
        if(font_cache == NULL) {
            CRASHF_DEBUG("Failed to allocate vector font cache");
        }

        VectorFontData *font_data = tag_get_data(TAG_GROUP_VECTOR_FONT_DATA, tag_handle);
        font_cache->tag_handle = tag_handle;
        void *font_data_blob = font_data->font_data.pointer;
        size_t font_data_size = font_data->font_data.size;
        
        bool font_is_valid = font_file_get_name_from_memory(font_data_blob, font_data_size, font_cache->font_name, SIZEOF_ARRAY(font_cache->font_name));
        font_cache->font_resource_handle = AddFontMemResourceEx(font_data_blob, font_data_size, NULL, (DWORD *)&font_cache->num_fonts);
        if(!font_is_valid || font_cache->font_resource_handle == NULL) {
            const char *tag_path = tag_get_path(tag_handle);
            THROW_EXCEPTION("Failed to load vector font from tag %s", tag_path);
        }
    }
    return font_cache;
}

void vector_font_release_cache(VectorFontCache *font_cache) {
    if(font_cache->num_fonts > 0) {
        font_cache->num_fonts--;
    }
    if(font_cache->num_fonts == 0) {
        RemoveFontMemResourceEx(font_cache->font_resource_handle);
        font_cache->tag_handle = NULL_HANDLE;
        font_cache->font_resource_handle = NULL;
        font_cache->num_fonts = 0;
    }
}

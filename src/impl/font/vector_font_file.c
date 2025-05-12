#include <string.h>
#include "../memory/memory.h"
#include "vector_font_file.h"

#pragma pack(push, 1)

typedef struct NameTableHeader {
    uint16_t format;
    uint16_t count;
    uint16_t string_offset;
} NameTableHeader;

typedef struct NameRecord {
    uint16_t platform_id;
    uint16_t encoding_id;
    uint16_t language_id;
    uint16_t name_id;
    uint16_t length;
    uint16_t offset;
} NameRecord;

#pragma pack(pop)

static uint16_t read_be16(const uint8_t *data) {
    return (uint16_t)(data[0] << 8 | data[1]);
}

static uint32_t read_be32(const uint8_t *data) {
    return (uint32_t)(data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]);
}

bool font_file_get_name_from_memory(const uint8_t *font_data, size_t font_size, wchar_t *out_name, size_t out_name_len) {
    if(font_data == NULL || out_name == NULL || out_name_len == 0 || font_size < 12) {
        return false;
    }

    uint16_t num_tables = read_be16(font_data + 4);
    size_t table_dir_offset = 12;
    size_t name_table_offset = 0;

    for(uint16_t i = 0; i < num_tables; i++) {
        size_t entry_offset = table_dir_offset + i * 16;
        if (entry_offset + 12 > font_size) {
            return false;
        }

        const uint8_t *entry = font_data + entry_offset;
        if (memcmp(entry, "name", 4) == 0) {
            name_table_offset = read_be32(entry + 8);
            break;
        }
    }

    if(name_table_offset == 0 || name_table_offset + sizeof(NameTableHeader) > font_size) {
        return false;
    }

    const NameTableHeader *header = (const NameTableHeader *)(font_data + name_table_offset);
    uint16_t count = (header->count << 8) | (header->count >> 8);
    uint16_t string_offset = (header->string_offset << 8) | (header->string_offset >> 8);

    const uint8_t *records_start = font_data + name_table_offset + sizeof(NameTableHeader);
    for(uint16_t i = 0; i < count; i++) {
        const NameRecord *record = (const NameRecord *)(records_start + i * sizeof(NameRecord));
        uint16_t platform_id = (record->platform_id << 8) | (record->platform_id >> 8);
        uint16_t encoding_id = (record->encoding_id << 8) | (record->encoding_id >> 8);
        uint16_t name_id     = (record->name_id << 8)     | (record->name_id >> 8);
        uint16_t length      = (record->length << 8)      | (record->length >> 8);
        uint16_t offset      = (record->offset << 8)      | (record->offset >> 8);

        if(name_id == 1 && platform_id == 3 && (encoding_id == 1 || encoding_id == 0)) {
            const uint8_t *str = font_data + name_table_offset + string_offset + offset;
            if((str + length) > (font_data + font_size)) {
                return false;
            }

            size_t wchar_count = length / 2;
            if(wchar_count >= out_name_len) {
                wchar_count = out_name_len - 1;
            }

            for(size_t j = 0; j < wchar_count; j++) {
                out_name[j] = (wchar_t)(str[j * 2] << 8 | str[j * 2 + 1]);  // UTF-16BE
            }
            out_name[wchar_count] = L'\0';
            return true;
        }
    }

    return false;
}

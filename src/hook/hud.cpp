/**
 * @todo THIS SHOULD BE REPLACED WITH REAL CODE ASAP!
 * This is just a workaround to fix the HUD messages with the new vector fonts
 */

#include <windows.h>
#include <cstdio>
#include "hook.hpp"
#include "codefinder.hpp"
#include "../impl/tag/definitions/font.h"
#include "../impl/tag/definitions/vector_font.h"
#include "../impl/interface/hud.h"

extern "C" {
    void hud_calculate_font_height_asm();
    
    uint32_t hud_calculate_font_height(Font *font_data, bool unk_flag) {
        TagEntry *tag_entry = NULL;
        const TagDataHeader *tag_header = tag_get_data_header();
        for(size_t i = 0; i < tag_header->tag_count; i++) {
            TagEntry *entry = &tag_header->tags[i];
            if(entry->data == font_data) {
                tag_entry = entry;
                break;
            }
        }
        if(tag_entry == NULL) {
            return 0;
        }
    
        if(tag_entry->primary_group == TAG_GROUP_FONT) {
            if(unk_flag) {
                return font_data->descending_height + font_data->ascending_height;
            }
            else {
                return font_data->leading_height + font_data->descending_height + font_data->ascending_height;
            }
        }
        else {
            return ((VectorFont *)font_data)->font_size;
        }
    }
}

void set_up_hud_hooks() {
    auto *address = reinterpret_cast<std::byte *>(0x4b1a05); // this function is huge, it will be reimplemented later
    auto *function = reinterpret_cast<std::byte *>(hud_calculate_font_height_asm);

    DWORD old_protection;
    VirtualProtect(reinterpret_cast<void *>(address), 16, PAGE_READWRITE, &old_protection);
    memset(address, 0x90, 16);
    *reinterpret_cast<std::uint8_t *>(address + 2) = 0xE8;
    *reinterpret_cast<std::uintptr_t *>(address + 2 + 1) = function - (address + 2 + 5);
    VirtualProtect(reinterpret_cast<void *>(address), 16, old_protection, &old_protection);
}

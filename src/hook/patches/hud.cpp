/**
 * @todo THIS SHOULD BE REPLACED WITH REAL CODE ASAP!
 * This is just a workaround to fix the HUD messages with the new vector fonts
 */

#include <windows.h>
#include <cstdio>
#include "../codefinder.hpp"
#include "../hook.hpp"
#include "../impl/tag/definitions/font.h"
#include "../impl/tag/definitions/vector_font.h"
#include "../impl/interface/hud.h"
#include "../impl/render/render.h"

static float *screen_center_x_constant_1 = reinterpret_cast<float *>(0x6128e4);
static uint32_t *screen_center_x_constant_2 = reinterpret_cast<uint32_t *>(0x4b2b06);
static float *screen_width_constant = reinterpret_cast<float *>(0x612268);

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

    void navpoints_update_screen_dimentions_hack() {
        float new_screen_width_constant = render_get_screen_width();
        float new_screen_center_x_constant_f = new_screen_width_constant / 2.0f;
        int new_screen_center_x_constant = static_cast<int>(new_screen_center_x_constant_f);
        overwrite(screen_center_x_constant_1, new_screen_center_x_constant_f);
        overwrite(screen_center_x_constant_2, new_screen_center_x_constant);
        overwrite(screen_width_constant, new_screen_width_constant);
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

    // Enable the use of the custom canvas scaling in the calls to hud_calculate_point
    uint8_t scaled_canvas_mode[] = { 0xB1, 0x01 }; // mov cl, 1
    uint8_t inverse_scaled_canvas_mode[] = { 0xB1, 0x02 }; // mov cl, 2
    overwrite(reinterpret_cast<uint8_t *>(0x4af614), scaled_canvas_mode, 2);
    overwrite(reinterpret_cast<uint8_t *>(0x4b19c9), inverse_scaled_canvas_mode, 2);
    overwrite(reinterpret_cast<uint8_t *>(0x4b4178), inverse_scaled_canvas_mode, 2);
}

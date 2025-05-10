/**
 * @todo THIS SHOULD BE REPLACED WITH REAL CODE ASAP!
 * This is just a workaround to make the shader transparent generic work.
 */

#include <windows.h>
#include "hook.hpp"

extern "C" {
    void shader_transparent_generic_switch_case_asm();
    void shader_transparent_generic_update_instances_asm();
    std::byte *map_load_thing = nullptr;
    std::byte *map_load_thing_return = nullptr;
}

void set_up_transparent_generic_hooks() {
    DWORD old_protection;
    auto *switch_case_address = reinterpret_cast<std::uint32_t *>(0x5381f4);
    VirtualProtect(reinterpret_cast<void *>(switch_case_address), 4, PAGE_READWRITE, &old_protection);
    *switch_case_address = reinterpret_cast<uint32_t>(shader_transparent_generic_switch_case_asm);
    VirtualProtect(reinterpret_cast<void *>(switch_case_address), 4, old_protection, &old_protection);

    auto *map_load_thing_call = reinterpret_cast<std::byte *>(0x442851);
    auto *instance_update = reinterpret_cast<std::byte *>(shader_transparent_generic_update_instances_asm);
    map_load_thing = reinterpret_cast<std::byte *>(0x443600);
    map_load_thing_return = reinterpret_cast<std::byte *>(0x442856);

    VirtualProtect(reinterpret_cast<void *>(map_load_thing_call), 5, PAGE_READWRITE, &old_protection);
    *reinterpret_cast<std::uint8_t *>(map_load_thing_call) = 0xE9;
    *reinterpret_cast<std::uintptr_t *>(map_load_thing_call + 1) = instance_update - (map_load_thing_call + 5);
    VirtualProtect(reinterpret_cast<void *>(map_load_thing_call), 5, old_protection, &old_protection);
}

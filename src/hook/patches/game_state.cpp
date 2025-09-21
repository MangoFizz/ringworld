/**
 * @todo THIS SHOULD BE REPLACED WITH REAL CODE ASAP!
 * This is just a workaround to make the game state expansion work.
 */

#include <windows.h>
#include "../codefinder.hpp"
#include "../hook.hpp"
#include "../impl/memory/memory.h"

void set_up_game_state_hooks() {
    // Replace all the hardcoded game state size values with the new size
    const short signature[] = { 0x00, 0x00, 0x44, 0x00 };
    auto finder = CodeFinder(GetModuleHandle(0), signature, sizeof(signature) / sizeof(signature[0]));
    auto results = finder.find();
    if(!results.empty()) {
        for(auto &result : results) {
            auto *address = reinterpret_cast<std::uint32_t *>(result);
            DWORD old_protect;
            VirtualProtect(address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &old_protect);
            *address = GAME_STATE_SIZE;
            VirtualProtect(address, sizeof(uint32_t), old_protect, nullptr);
        }
    }
}

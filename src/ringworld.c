#include <windows.h>
#include "impl/debug/debug_symbols.h"
#include "ringworld.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            debug_symbols_initialize();
            set_up_ringworld_hooks(RW_PLATFORM_GAME);
        default:
            break;
    }
    return TRUE;
}

#include <windows.h>
#include "ringworld.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            set_up_ringworld_hooks(RW_PLATFORM_GAME);
        default:
            break;
    }
    return TRUE;
}

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "../exception/exception.h"
#include "memory.h"
#include "table.h"

extern const bool ringworld_headless_mode;

extern void **texture_cache;
extern void **sound_cache;
extern uint16_t *sound_cache_mib;

extern ManagedHeapMap *managed_heap_map;

void memory_map_initialize(void) {
    managed_heap_map->game_state_base_address = VirtualAlloc(MANAGED_HEAP_BASE_ADDRESS, MANAGED_HEAP_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memset(managed_heap_map->game_state_base_address, 0, MANAGED_HEAP_SIZE);
    managed_heap_map->tag_data_base_address = TAG_DATA_LOAD_ADDRESS;
    *sound_cache = NULL;
    *texture_cache = NULL;

    if(!ringworld_headless_mode) {
        *sound_cache_mib = 64;
        *sound_cache = VirtualAlloc(0, *sound_cache_mib * 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        *texture_cache = VirtualAlloc(0, TEXTURE_CACHE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }

    if(managed_heap_map->game_state_base_address == NULL) {
        HMODULE psapi = LoadLibraryA("psapi.dll");
        if(psapi != NULL) {
            typedef DWORD (WINAPI *GetMappedFileNameA_t)(HANDLE, LPVOID, LPSTR, DWORD);
            GetMappedFileNameA_t GetMappedFileNameA = (GetMappedFileNameA_t)GetProcAddress(psapi, "GetMappedFileNameA");
            if(GetMappedFileNameA != NULL) {
                char filename[MAX_PATH];
                memset(filename, 0, sizeof(filename));
                GetMappedFileNameA(GetCurrentProcess(), MANAGED_HEAP_BASE_ADDRESS, filename, MAX_PATH);
                if(filename[0] != '\0') {
                    exception_throw_runtime_error("memory_map_initialize(): Address 0x%08X is already mapped to %s", MANAGED_HEAP_BASE_ADDRESS, filename);
                }
            }
            FreeLibrary(psapi);
        }
       exception_throw_runtime_error("memory_map_initialize(): Failed to allocate required memory at 0x%08X", MANAGED_HEAP_BASE_ADDRESS);
    }
}

ManagedHeapMap *memory_map_get(void) {
    return managed_heap_map;
}

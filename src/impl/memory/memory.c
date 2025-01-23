#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "../exception/exception.h"
#include "memory.h"
#include "table.h"

extern void **managed_heap;
extern void **tag_data;
extern void **texture_cache_unk1;
extern void **sound_cache;
extern uint16_t *sound_cache_mib;

void memory_map_initialize(void) {
    *sound_cache_mib = 64;
    *managed_heap = VirtualAlloc(MANAGED_HEAP_ADDRESS_START, BASE_HEAP_AMOUNT + TAG_DATA_LENGTH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    *tag_data = TAG_DATA_LOAD_ADDRESS;
    *texture_cache_unk1 = VirtualAlloc(0, TEXTURE_CACHE_UNK1_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); 
    *sound_cache = VirtualAlloc(0, *sound_cache_mib * 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if(*managed_heap == NULL) {
        HMODULE psapi = LoadLibraryA("psapi.dll");
        if(psapi != NULL) {
            typedef DWORD (WINAPI *GetMappedFileNameA_t)(HANDLE, LPVOID, LPSTR, DWORD);
            GetMappedFileNameA_t GetMappedFileNameA = (GetMappedFileNameA_t)GetProcAddress(psapi, "GetMappedFileNameA");
            if(GetMappedFileNameA != NULL) {
                char filename[MAX_PATH];
                memset(filename, 0, sizeof(filename));
                GetMappedFileNameA(GetCurrentProcess(), MANAGED_HEAP_ADDRESS_START, filename, MAX_PATH);
                if(filename[0] != '\0') {
                    crashf("memory_map_initialize(): Address 0x%08X is already mapped to %s", (uintptr_t)(*managed_heap), filename);
                }
            }
            FreeLibrary(psapi);
        }
       crashf("memory_map_initialize(): Failed to allocate required memory at 0x%08X", (uintptr_t)(*managed_heap));
    }
}


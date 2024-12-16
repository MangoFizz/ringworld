#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include <ringworld/memory/memory.h>
#include <ringworld/memory/table.h>

#include "../exception/exception.h"

#define BASE_HEAP_AMOUNT (size_t)(4352 * 1024) // 4.25 MiB

#ifndef RINGWORLD_ENABLE_ENHANCEMENTS
#define TAG_DATA_LENGTH (size_t)(23 * 1024 * 1024) // 23 MiB 
#else
#define TAG_DATA_LENGTH (size_t)(64 * 1024 * 1024) // 64 MiB 
#endif

#define MANAGED_HEAP_ADDRESS_START (void *)(0x40000000)
#define TAG_DATA_LOAD_ADDRESS (void *)(MANAGED_HEAP_ADDRESS_START + BASE_HEAP_AMOUNT) // 0x4BF10000

#define TEXTURE_CACHE_UNK1_SIZE (size_t)(16 * 1024) // 16 KiB

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

extern void **alloc_base_address;
extern uint32_t *alloc_size;
extern uint32_t *alloc_crc_checksum;

void *allocate_heap(size_t size) {
    size_t remaining_bytes = BASE_HEAP_AMOUNT - *alloc_size;
    if(remaining_bytes < size) {
        crashf("allocate_heap(): Can't allocate 0x%08zX bytes (only 0x%08zX bytes remaining)", size, remaining_bytes);
    }

    void *allocated = *alloc_base_address + *alloc_size;
    *alloc_size += size;

    malloc_crc_checksum_buffer(alloc_crc_checksum, &size, 4);

    return allocated;
}

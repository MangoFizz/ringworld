#ifndef RINGWORLD__GAME__GAME_STATE_H
#define RINGWORLD__GAME__GAME_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define SAVEGAME_FILE_SIZE (GAME_STATE_SIZE + 0x40000)

typedef struct MemoryPool MemoryPool;

typedef struct SaveGameGlobals {
    bool buffer_allocated;
    void *buffer;
    int buffer_size;
    bool file_open;
    bool file_valid_for_read;
    void *handle;
    char saved_game_path[256];
    char core_path[256];
} SaveGameGlobals;

typedef struct GameStateHeader {
    uint32_t allocation_size_checksum;
    char map_name[256];
    char build_number[32];
    int16_t player_count;
    int16_t difficulty;
    uint32_t cache_file_checksum;
    int32_t pad1[7];
    unsigned int checksum;
} GameStateHeader;

typedef struct GameStateGlobals {
    void *base_address;
    int32_t allocation_size;
    int32_t unk1;
    int32_t allocation_size_checksum;
    bool locked;
    bool saved_game_valid;
    bool unk2;
    bool unk3;
    int32_t revert_time;
    GameStateHeader *header;
} GameStateGlobals;

/**
 * Allocate memory in the heap.
 *
 * Take caution that there is not much space in this heap (only 4.25 MiB). If there is not enough space, NULL will be
 * returned, and an error will be logged in stderr.
 *
 * @param size number of bytes to allocate
 *
 * @return     pointer to new data or NULL if there is not enough space
 */
void *game_state_allocate_heap(size_t size);

/**
 * Allocate a table of objects.
 * @param name          name of table
 * @param maximum_count maximum element count
 * @param element_size  size of each element
 * @return table
 */
void *game_state_table_new(const char *name, uint16_t maximum_count, uint16_t element_size);

/**
 * Allocate a memory pool for game state.
 * @param name name of the memory pool
 * @param size size of the memory pool
 */
MemoryPool *game_state_memory_pool_new(const char *name, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif


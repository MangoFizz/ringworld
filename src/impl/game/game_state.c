#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <process.h>
#include "../memory/table.h"
#include "../crypto/crc32.h"
#include "../exception/exception.h"
#include "../saved_games/player_profile.h"
#include "game_state.h"

extern SaveGameGlobals *saved_game_state_globals;
extern GameStateGlobals *game_state_globals;
extern void **saved_game_state_buffer;
extern bool *saved_game_state_is_being_written;
extern HANDLE *savegame_file_read_event_handle;

void game_state_savegame_write_thread(void);

void *game_state_allocate_heap(size_t size) {
    size_t remaining_bytes = GAME_STATE_SIZE - game_state_globals->allocation_size;
    if(remaining_bytes < size) {
        exception_throw_runtime_error("allocate_heap(): Can't allocate 0x%08zX bytes (only 0x%08zX bytes remaining)", size, remaining_bytes);
    }
    void *allocated = game_state_globals->base_address + game_state_globals->allocation_size;
    game_state_globals->allocation_size += size;
    crc32_calculate(&game_state_globals->allocation_size_checksum, &size, 4);
    return allocated;
}

void *game_state_table_new(const char *name, uint16_t maximum_count, uint16_t element_size) {
    size_t allocation_amount = CALCULATE_ALLOCATION_SIZE(maximum_count, element_size);
    GenericTable *table = game_state_allocate_heap(allocation_amount);
    table_initialize(table, name, maximum_count, element_size);
    return table;
}

void *game_state_initialize_savegame(size_t game_state_size, size_t hs_state_size) {
    ManagedHeapMap *managed_heap = memory_map_get();
    saved_game_state_globals->buffer = managed_heap->game_state_base_address;
    saved_game_state_globals->buffer_size = game_state_size + hs_state_size;
    saved_game_state_globals->buffer_allocated = true;
    
    const char *profile_path = saved_games_get_active_player_profile_path();
    snprintf(saved_game_state_globals->saved_game_path, 255, "%s\\%s", profile_path, "savegame.bin");
    snprintf(saved_game_state_globals->core_path, 255, "%s\\%s", profile_path, "core");
    
    *saved_game_state_buffer = GlobalAlloc(GMEM_FIXED, saved_game_state_globals->buffer_size);
    *saved_game_state_is_being_written = false;
    
    *savegame_file_read_event_handle = CreateEventA(NULL, FALSE, FALSE, NULL);
    _beginthread((_beginthread_proc_type)game_state_savegame_write_thread, 0x1000, NULL);
    
    return managed_heap->game_state_base_address;
}

void game_state_create_savegame_file(void) {
    const char *path = saved_game_state_globals->saved_game_path;
    DWORD access = GENERIC_READ | GENERIC_WRITE;
    saved_game_state_globals->handle = CreateFileA(path, access, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(saved_game_state_globals->handle == INVALID_HANDLE_VALUE) {
        exception_throw_runtime_error("Failed to create savegame file: %s", path); // @todo define a way to handle errors
    }
    DWORD res = SetFilePointer(saved_game_state_globals->handle, SAVEGAME_FILE_SIZE, NULL, FILE_BEGIN);
    if(res == INVALID_SET_FILE_POINTER) {
        exception_throw_runtime_error("Failed to set file pointer for savegame file: %s", path); 
    }
    res = SetEndOfFile(saved_game_state_globals->handle);
    if(res == 0) {
        exception_throw_runtime_error("Failed to set end of file for savegame file: %s", path); 
    }
    saved_game_state_globals->file_open = true;
}

HANDLE game_state_open_savegame(const char *path) {
    char savegame_path[MAX_PATH];
    if(path == NULL) {
        uint32_t current_player_profile = saved_games_get_player_profile_id(0);
        bool res = saved_game_file_get_path_to_enclosing_directory(current_player_profile, savegame_path);
        if(!res) {
            return INVALID_HANDLE_VALUE;
        }
    }
    else {
        strcpy(savegame_path, path);
    }
    strcat(savegame_path, "savegame.bin");

    HANDLE file = CreateFileA(savegame_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
    if(file != INVALID_HANDLE_VALUE) {
        DWORD file_size = GetFileSize(file, NULL);
        if(file_size != SAVEGAME_FILE_SIZE) {
            char savegame_file_header[0x4000];
            memset(savegame_file_header, 0, sizeof(savegame_file_header));
            DWORD bytes_written;
            BOOL res = WriteFile(file, savegame_file_header, sizeof(savegame_file_header), &bytes_written, NULL);
            if(res == TRUE && bytes_written == sizeof(savegame_file_header)) {
                DWORD file_pointer = SetFilePointer(file, SAVEGAME_FILE_SIZE, NULL, FILE_BEGIN);
                if(file_pointer != INVALID_SET_FILE_POINTER) {
                    if(SetEndOfFile(file) != 0) {
                        return file;
                    }
                }
            }
        }
        else {
            return file;
        }
    }
    CloseHandle(file);
    DeleteFileA(savegame_path);

    exception_throw_runtime_error("Failed to open savegame file: %s", savegame_path);
    return INVALID_HANDLE_VALUE;
}

void game_state_initialize(void) {
    game_state_globals->allocation_size_checksum = -1;
    game_state_globals->base_address = game_state_initialize_savegame(GAME_STATE_SIZE, 0x40000);
    game_state_globals->header = game_state_allocate_heap(sizeof(GameStateHeader));
    game_state_create_savegame_file();
}

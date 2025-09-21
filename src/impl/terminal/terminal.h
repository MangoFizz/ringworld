#ifndef RINGWORLD__TERMINAL__TERMINAL_H
#define RINGWORLD__TERMINAL__TERMINAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../console/console.h"
#include "../memory/table.h"
#include "../types/types.h"

enum {
    TERMINAL_OUTPUT_MAX_DISPLAY_LINES = 32
};

typedef TableResourceHandle TableOutputEntryHandle;

typedef struct TerminalOutputEntry {
    TableOutputEntryHandle previous;
    TableOutputEntryHandle next; 
    uint32_t tab_stops;	
    char text[256];
    uint32_t unk1;
    ColorARGB color;
    uint32_t display_time;
} TerminalOutputEntry;
_Static_assert(sizeof(TerminalOutputEntry) == 0x124);

MAKE_TABLE_STRUCT(TerminalOutputTable, TerminalOutputEntry);

typedef struct TerminalGlobals {
    bool initialized;
    TerminalOutputTable *output_data;
    TableOutputEntryHandle first_line;
    TableOutputEntryHandle last_line;
    ConsoleInputState *current_state;
    bool receiving_input;
    uint32_t receiving_input_start_tick;
    bool is_active;
    int32_t rcon_machine_index;
} TerminalGlobals;
_Static_assert(sizeof(TerminalGlobals) == 0x24);

typedef struct TerminalScreen {
    char prompt[32];
    char text[256];
    // this should be uint32_t to set the alignment correctly and match the size of the struct
    uint32_t cursor_position; 
} TerminalScreen;
_Static_assert(sizeof(TerminalScreen) == 0x124);

typedef struct TerminalInput {
    void *stdin_handle;
    void *stdout_handle;
    uint32_t pad0;
} TerminalInput;
_Static_assert(sizeof(TerminalInput) == 0xC);

/**
 * Initialize the terminal.
 */
void terminal_initialize(void);

/**
 * Print a formatted line of text to the terminal.
 * @param color (optional)
 * @param fmt   format
 */
void terminal_printf(const struct ColorARGB *color, const char *fmt, ...);

/**
 * Print a gray formatted line of text to the terminal.
 * @param fmt   format
 * @param ...   arguments
 */
void terminal_info_printf(const char *fmt, ...);

/**
 * Print an orange formatted line of text to the terminal.
 * @param fmt   format
 * @param ...   arguments
 */
void terminal_warning_printf(const char *fmt, ...);

/**
 * Print a red formatted line of text to the terminal.
 * @param fmt   format
 * @param ...   arguments
 */
void terminal_error_printf(const char *fmt, ...);

/**
 * Mute or unmute terminal output.
 * @param mute  true to mute, false to unmute
 */
void terminal_mute_output(bool mute);

/**
 * Handler routine for console control events.
 * @param ctrl_type The type of control event.
 */
void terminal_handler_routine(uint32_t ctrl_type);

#ifdef __cplusplus
}
#endif

#endif

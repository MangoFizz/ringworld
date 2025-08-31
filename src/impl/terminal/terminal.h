#ifndef RINGWORLD__TERMINAL__TERMINAL_H
#define RINGWORLD__TERMINAL__TERMINAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
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
    void *current_state;
    bool receiving_input;
    uint32_t receiving_input_start_tick;
    bool is_active;
    int32_t rcon_machine_index;
} TerminalGlobals;
_Static_assert(sizeof(TerminalGlobals) == 0x24);

/**
 * Initialize the terminal.
 */
void terminal_initialize(void);

/**
 * Print a formatted line of text to the console.
 *
 * @param color (optional)
 * @param fmt   format
 */
void terminal_printf(const struct ColorARGB *color, const char *fmt, ...);

/**
 * Print a red formatted line of text to the console.
 *
 * @param fmt   format
 * @param ...   arguments
 */
void terminal_error_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

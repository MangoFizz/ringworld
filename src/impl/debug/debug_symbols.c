#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef RINGWORLD_NO_BACKTRACE
#include <backtrace.h>
#endif
#include "../error/error_code.h"
#include "../shell/shell.h"
#include "log.h"
#include "debug_symbols.h"

struct backtrace_state *backtrace_state = NULL;

static void backtrace_print_error_callback(void *data, const char *msg, int errnum) {
    log_debug("Backtrace: %s (%d)", msg, errnum);
}

static int backtrace_resolve_callback(void *data, uintptr_t pc, const char *filename, int lineno, const char *function) {
    DebugAddressSourceInfo *info = data;
    info->filename = filename;
    info->line_number = lineno;
    return 1;
}

bool debug_symbols_get_address_source_info(uintptr_t addr, DebugAddressSourceInfo *out_info) {
#ifndef RINGWORLD_NO_BACKTRACE
    if(!backtrace_state) {
        log_debug("Backtrace state is not initialized");
        return false;
    }
    memset(out_info, 0, sizeof(DebugAddressSourceInfo));
    int ret = backtrace_pcinfo(backtrace_state, addr, backtrace_resolve_callback, backtrace_print_error_callback, out_info);
    if(out_info->filename) {
        return true;
    }
#endif
    return false;
}

void debug_symbols_initialize(void) {
#ifndef RINGWORLD_NO_BACKTRACE
    if(backtrace_state) {
        log_debug("Backtrace state is already initialized");
        return;
    }
    char module_path[MAX_PATH_LENGTH];
    int res = shell_get_ringworld_module_path(module_path, sizeof(module_path));
    if(res != ERROR_CODE_SUCCESS) {
        log_error("Failed to get ringworld module path");
        return;
    }
    // If we don't lowercase the drive letter, backtrace will not find the file for some reason X_x
    if(module_path[1] == ':') {
        module_path[0] = tolower(module_path[0]);
    }
    backtrace_state = backtrace_create_state(module_path, 0, backtrace_print_error_callback, NULL);
    if(!backtrace_state) {
        log_error("Failed to create backtrace state");
        return;
    }
    // Force backtrace to load symbols at startup
    DebugAddressSourceInfo out_info;
    uintptr_t dummy_address = (uintptr_t)debug_symbols_initialize;
    backtrace_pcinfo(backtrace_state, dummy_address, backtrace_resolve_callback, backtrace_print_error_callback, &out_info);
#endif
}

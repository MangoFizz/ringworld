#include <string.h>
#include <windows.h>
#ifndef RINGWORLD_NO_BACKTRACE
#include <backtrace.h>
#endif
#include "../shell/shell.h"
#include "log.h"
#include "debug_symbols.h"

struct backtrace_state *backtrace_state = NULL;

static void backtrace_print_error_callback(void *data, const char *msg, int errnum) {
    log_error("Backtrace error: %s (%d)", msg, errnum);
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
        HMODULE module = (HMODULE)shell_get_current_module_handle();
        char module_path[MAX_PATH];
        if(GetModuleFileNameA(module, module_path, sizeof(module_path)) == 0) {
            log_error("Failed to get module file name");
            return false;
        }
        // If we don't lowercase the drive letter, backtrace will not find the file for some reason X_x
        if(module_path[1] == ':') {
            module_path[0] = tolower(module_path[0]);
        }
        backtrace_state = backtrace_create_state(module_path, 0, backtrace_print_error_callback, NULL);
        if(!backtrace_state) {
            log_error("Failed to create backtrace state");
            return false;
        }
    }
    memset(out_info, 0, sizeof(DebugAddressSourceInfo));
    int ret = backtrace_pcinfo(backtrace_state, addr, backtrace_resolve_callback, backtrace_print_error_callback, out_info);
    if(out_info->filename) {
        return true;
    }
#endif
    return false;
}

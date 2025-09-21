#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include "shell/shell.h"
#include "debug/log.h"
#include "debug/debug_symbols.h"
#include "debug/stacktrace.h"
#include "exception.h"

typedef enum ExceptionCode {
    EXCEPTION_RINGWORLD = 0xE000DEAD,
} ExceptionCode;

enum {
    MAX_STACKTRACE_STACK_FRAMES = 32
};

void exception_print_exception_info(EXCEPTION_RECORD *record) {
    log_debug("Code: 0x%08X", record->ExceptionCode);
    log_debug("Address: 0x%p", record->ExceptionAddress);
    log_debug("Flags: 0x%X", record->ExceptionFlags);
}

void exception_print_context(CONTEXT *context) {
    log_debug("CPU Registers (Context)");
    log_debug("  EAX = 0x%08X", context->Eax);
    log_debug("  EBX = 0x%08X", context->Ebx);
    log_debug("  ECX = 0x%08X", context->Ecx);
    log_debug("  EDX = 0x%08X", context->Edx);
    log_debug("  ESI = 0x%08X", context->Esi);
    log_debug("  EDI = 0x%08X", context->Edi);
    log_debug("  EBP = 0x%08X", context->Ebp);
    log_debug("  ESP = 0x%08X", context->Esp);
    log_debug("  EIP = 0x%08X", context->Eip);
    log_debug("  EFLAGS = 0x%08X", context->EFlags);
}

void exception_print_stack_trace(CONTEXT *context) {
    DebugStackFrame frames[MAX_STACKTRACE_STACK_FRAMES];
    stacktrace_build_trace(context, frames, MAX_STACKTRACE_STACK_FRAMES);

    log_debug("Stack Trace");

    for(int i = 0; i < MAX_STACKTRACE_STACK_FRAMES; i++) {
        DebugStackFrame *frame = &frames[i];
        if(frame->index == -1) {
            break; 
        }

        char symbol_name[256];
        bool has_symbol = strlen(frame->symbol) > 0;
        if(has_symbol) {
            debug_symbols_demangle(frame->symbol, symbol_name, sizeof(symbol_name));
        } 
        else {
            snprintf(symbol_name, sizeof(symbol_name), "??");
        }

        log_debug("  [%02d]: %s", frame->index, symbol_name);
        if(frame->address) {
            log_debug("      Address: 0x%.8X", frame->address);
        }
        if(has_symbol) {
            log_debug("      Raw Symbol: %s + 0x%X", frame->symbol, frame->offset);
        }
        if(frame->source_info.filename) {
            log_debug("      Source: %s:%d", frame->source_info.filename, frame->source_info.line_number);
        }
    }
}

void exception_print_report(EXCEPTION_RECORD *record, CONTEXT *context) {
    log_debug("EXCEPTION REPORT:");
    exception_print_exception_info(record);
    exception_print_context(context);
    exception_print_stack_trace(context);
}

EXCEPTION_DISPOSITION seh_exception_handler(EXCEPTION_RECORD *record, void *, CONTEXT *context, void *) {    
    switch(record->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            log_error("Access violation at address 0x%p", record->ExceptionAddress);
            break;
        case EXCEPTION_RINGWORLD:
            if(record->NumberParameters > 0 && record->ExceptionInformation[0] != 0) {
                log_error("ringworld exception: %s", (const char *)record->ExceptionInformation[0]);
            }
            else {
                log_error("ringworld exception: no message provided");
            }
            break;
        default:
            log_error("The game has encountered an exception!");
            log_error("Unhandled exception code: 0x%08X", record->ExceptionCode);
            break;
    }

    exception_print_report(record, context);

    shell_error_box("The game has encountered an exception!\n\nCheck the logs for details.");

    ExitProcess(127); 

    return ExceptionContinueSearch;
}

void exception_throw_runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char *message = malloc(1024);
    if(message == NULL) {
        message = "If you see this message, it means the game has run out of memory "
                    "and it don't even have enough memory to allocate memory for the "
                    "error message.";
    }
    vsnprintf(message, 1024, fmt, args);
    va_end(args);

    ULONG_PTR arguments[1];
    arguments[0] = (ULONG_PTR)message;
    RaiseException(EXCEPTION_RINGWORLD, EXCEPTION_NONCONTINUABLE, 1, arguments);
}

void exception_throw_forbidden_function_called(const char *function_name, void *from) {
    exception_throw_runtime_error("Forbidden function called! %s was called from 0x%08zX", function_name, from);
}

void unavailable_function_called() {
    exception_throw_runtime_error("Unexisting function called!");
}

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

enum {
    RINGWORLD_EXCEPTION_CODE = 0xE000DEAD,
    MAX_STACKTRACE_STACK_FRAMES = 32
};

void exception_print_exception_info(EXCEPTION_RECORD *record) {
    log_info("Code: 0x%08X", record->ExceptionCode);
    log_info("Address: 0x%p", record->ExceptionAddress);
    log_info("Flags: 0x%X", record->ExceptionFlags);
}

void exception_print_context(CONTEXT *context) {
    log_info("CPU Registers (Context)");
    log_info("  EAX = 0x%08X", context->Eax);
    log_info("  EBX = 0x%08X", context->Ebx);
    log_info("  ECX = 0x%08X", context->Ecx);
    log_info("  EDX = 0x%08X", context->Edx);
    log_info("  ESI = 0x%08X", context->Esi);
    log_info("  EDI = 0x%08X", context->Edi);
    log_info("  EBP = 0x%08X", context->Ebp);
    log_info("  ESP = 0x%08X", context->Esp);
    log_info("  EIP = 0x%08X", context->Eip);
    log_info("  EFLAGS = 0x%08X", context->EFlags);
}

void exception_print_stack_trace(CONTEXT *context) {
    log_info("Stack Trace");

    DebugStackFrame frames[MAX_STACKTRACE_STACK_FRAMES];
    stacktrace_build_trace(context, frames, MAX_STACKTRACE_STACK_FRAMES);

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

        log_info("  [%02d]: %s", frame->index, symbol_name);
        if(frame->address) {
            log_info("      Address: 0x%.8X", frame->address);
        }
        if(has_symbol) {
            log_info("      Raw Symbol: %s + 0x%X", frame->symbol, frame->offset);
        }
        if(frame->source_info.filename) {
            log_info("      Source: %s:%d", frame->source_info.filename, frame->source_info.line_number);
        }
    }
}

void exception_print_report(EXCEPTION_RECORD *record, CONTEXT *context) {
    log_info("EXCEPTION REPORT:");
    exception_print_exception_info(record);
    exception_print_context(context);
    exception_print_stack_trace(context);
}

EXCEPTION_DISPOSITION seh_exception_handler(EXCEPTION_RECORD *record, void *, CONTEXT *context, void *) {    
    switch(record->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            log_error("Access violation at address 0x%p", record->ExceptionAddress);
            break;
        case RINGWORLD_EXCEPTION_CODE:
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
    RaiseException(RINGWORLD_EXCEPTION_CODE, EXCEPTION_NONCONTINUABLE, 1, arguments);
}

void exception_throw_forbidden_function_called(const char *function_name, void *from) {
    exception_throw_runtime_error("Forbidden function called! %s was called from 0x%08zX", function_name, from);
}

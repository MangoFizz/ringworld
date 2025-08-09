#ifndef RINGWORLD__DEBUG__STACKTRACE_H
#define RINGWORLD__DEBUG__STACKTRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include "debug_symbols.h"

typedef struct DebugStackFrame {
    int32_t index;
    uintptr_t address;
    char symbol[256];
    DWORD64 offset;
    DebugAddressSourceInfo source_info;
} DebugStackFrame;

/**
 * Build a stack trace from the current context.
 * @param context The context to build the stack trace from.
 * @param frames The array of DebugStackFrame to fill with the stack trace.
 * @param max_frames The maximum number of frames to fill.
 */
void stacktrace_build_trace(CONTEXT *context, DebugStackFrame *frames, size_t max_frames);

#ifdef __cplusplus
}
#endif

#endif

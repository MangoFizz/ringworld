#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>
#include "debug_symbols.h"
#include "stacktrace.h"

void stacktrace_build_trace(CONTEXT *context, DebugStackFrame *frames, size_t max_frames) {
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    STACKFRAME stack = { 0 };
    stack.AddrPC.Offset = context->Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context->Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Esp;
    stack.AddrStack.Mode = AddrModeFlat;

    SymInitialize(hProcess, NULL, TRUE);

    memset(frames, 0, sizeof(DebugStackFrame) * max_frames);

    for(int i = 0; i < max_frames; i++) {
        DebugStackFrame *frame = &frames[i];
        
        if(!StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack, context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL)) {
            frame->index = -1;
            break;
        }

        if(stack.AddrPC.Offset == 0) {
            frame->index = -1;
            break;
        }

        BYTE symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {0};
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        frame->index = i;
        frame->address = stack.AddrPC.Offset;
        frame->offset = 0;
        frame->source_info.filename = NULL;
        frame->source_info.line_number = 0;
        if(SymFromAddr(hProcess, stack.AddrPC.Offset, &frame->offset, pSymbol)) {
            snprintf(frame->symbol, sizeof(frame->symbol), "%s", pSymbol->Name);
            debug_symbols_get_address_source_info(stack.AddrPC.Offset - 1, &frame->source_info);
        }
    }
}

void *stacktrace_get_caller_address(CONTEXT *context) {
    STACKFRAME stack = { 0 };
    stack.AddrPC.Offset = context->Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context->Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Esp;
    stack.AddrStack.Mode = AddrModeFlat;

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    if(StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack, context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL)) {
        return (void *)stack.AddrPC.Offset;
    }
    return NULL;
}

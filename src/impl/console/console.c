#include <stdio.h>
#include <stdarg.h>

#include "../types/types.h"
#include "console.h"

void console_printf_in(const ColorARGB *color, const char *fmt, const char *str);

void console_printf(const ColorARGB *color, const char *fmt, ...) {
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    console_printf_in(color, "%s", passed_text);
}

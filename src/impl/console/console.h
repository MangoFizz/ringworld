#ifndef RINGWORLD__CONSOLE__CONSOLE_H
#define RINGWORLD__CONSOLE__CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct ColorARGB;

/**
 * Print a formatted line of text to the console.
 *
 * @param color (optional)
 * @param fmt   format
 */
void console_printf(const struct ColorARGB *color, const char *fmt, ...);

/**
 * Print a red formatted line of text to the console.
 *
 * The syntax is the same as console_printf except no color is passed.
 */
#define console_printf_debug_err(...) { ColorARGB dbg_err_color_print = { 1.0, 1.0, 0.1, 0.1 }; console_printf(&dbg_err_color_print, __VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif

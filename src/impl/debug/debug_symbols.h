#ifndef RINGWORLD__DEBUG__DEBUG_SYMBOLS_H
#define RINGWORLD__DEBUG__DEBUG_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct DebugAddressSourceInfo {
    const char *filename;
    int line_number;
} DebugAddressSourceInfo;

/**
 * Demangle a C++ symbol. If the symbol is not a C++ symbol, it will be returned as is.
 * @param symbol The symbol to demangle.
 * @param buffer The buffer to store the demangled symbol.
 * @param buffer_size The size of the buffer.
 */
void debug_symbols_demangle(const char *symbol, char *buffer, size_t buffer_size);

/**
 * Get source information for a given address.
 * @param addr The address to resolve.
 * @param out_info Pointer to a DebugAddressSourceInfo structure to fill with the result.
 * @return true if the source information was found, false otherwise.
 */
bool debug_symbols_get_address_source_info(uintptr_t addr, DebugAddressSourceInfo *out_info);

#ifdef __cplusplus
}
#endif

#endif

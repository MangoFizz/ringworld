#include <string>
#include <cstdio>
#include <cstdlib>

#if defined(__GNUC__)

#include <cxxabi.h>

extern "C" void debug_symbols_demangle(const char *symbol, char *buffer, size_t buffer_size) {
    int status;
    auto symbol_name = "_" + std::string(symbol);
    char *demangled = abi::__cxa_demangle(symbol_name.c_str(), buffer, &buffer_size, &status);
    if(status == 0) {
        snprintf(buffer, buffer_size, "%s", demangled);
    } 
    else {
        snprintf(buffer, buffer_size, "%s", symbol); 
    }
    free(demangled);
}

#else

extern "C" void debug_symbols_demangle(const char *symbol, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%s", symbol); 
}

#endif

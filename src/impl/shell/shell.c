#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include <dwmapi.h>
#include <shlobj.h>
#include "debug/log.h"
#include "error/error_code.h"
#include "exception/exception.h"
#include "shell.h"


const size_t MAX_PATH_LENGTH = MAX_PATH;
const char *window_title = "ringworld";

static HMODULE get_ringworld_module_handle(void) {
    HMODULE hModule = NULL;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)get_ringworld_module_handle, &hModule);
    return hModule;
}

static HMODULE get_executable_module_handle(void) {
    return *executable_module;
}

int shell_get_ringworld_module_path(char *out_buffer, size_t buffer_size) {
    if(buffer_size < MAX_PATH_LENGTH) {
        log_error("Buffer size is too small to hold module path");
        return ERROR_CODE_INSUFFICIENT_BUFFER;
    }
    HMODULE module = get_ringworld_module_handle();
    char module_path[MAX_PATH_LENGTH];
    if(GetModuleFileNameA(module, module_path, sizeof(module_path)) == 0) {
        log_error("Failed to get module file name");
        return ERROR_CODE_FAILURE;
    }
    strncpy(out_buffer, module_path, buffer_size);
    return ERROR_CODE_SUCCESS;
}

int shell_get_documents_directory_path(char *out_buffer, size_t buffer_size) {
    if(buffer_size < MAX_PATH_LENGTH) {
        log_error("Buffer size is too small to hold documents directory path");
        return ERROR_CODE_INSUFFICIENT_BUFFER;
    }
    if(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, out_buffer) != S_OK) {
        return ERROR_CODE_FAILURE;
    }
    return ERROR_CODE_SUCCESS;
}

int shell_get_data_directory_path(char *out_buffer, size_t buffer_size) {
    if(buffer_size < MAX_PATH_LENGTH) {
        log_error("Buffer size is too small to hold data directory path");
        return ERROR_CODE_INSUFFICIENT_BUFFER;
    }
    char documents_path[MAX_PATH_LENGTH];
    int res = shell_get_documents_directory_path(documents_path, MAX_PATH_LENGTH);
    if(res != ERROR_CODE_SUCCESS) {
        return res;
    }
    sprintf_s(out_buffer, buffer_size, "%s\\My Games\\ringworld", documents_path);
    if(!CreateDirectoryA(out_buffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        exception_throw_runtime_error("Failed to create data directory: %s", out_buffer);
    }
    return ERROR_CODE_SUCCESS;
}

void shell_message_box(const char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    MessageBox(NULL, buffer, window_title, MB_OK);
}

void shell_error_box(const char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    MessageBox(NULL, buffer, window_title, MB_ICONERROR | MB_OK);
}

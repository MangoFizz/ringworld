#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#include "exception/exception.h"
#include "shell.h"

static const char *message_box_title = "ringworld";

void shell_get_documents_directory_path(char *buffer) {
    if(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, buffer) != S_OK) {
        exception_throw_runtime_error("Failed to get documents directory path");
    }
}

void shell_get_data_directory_path(char *buffer) {
    char documents_path[MAX_PATH];
    shell_get_documents_directory_path(documents_path);
    sprintf_s(buffer, MAX_PATH, "%s\\My Games\\ringworld", documents_path);
    if(!CreateDirectoryA(buffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        exception_throw_runtime_error("Failed to create data directory: %s", buffer);
    }
}

void shell_message_box(const char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    MessageBox(NULL, buffer, message_box_title, MB_OK);
}

void shell_error_box(const char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    MessageBox(NULL, buffer, message_box_title, MB_ICONERROR | MB_OK);
}

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

typedef enum StringResourceID {
    STRING_RESOURCE_ID_SPLASH_BMP = 0x86
} StringResourceID;

typedef enum ExecutableResourceID {
    EXECUTABLE_RESOURCE_ID_ICON = 0x66
} ExecutableResourceID;

extern HMODULE *executable_module;
extern HMODULE *strings_module;
extern WNDPROC *window_proc;
extern HWND *window_handle;
extern DWORD *window_style;
extern HBITMAP *window_background_bitmap;
extern HDC *window_memory_dc;
extern size_t *process_argc;
extern const char ***process_argv;

const size_t MAX_PATH_LENGTH = MAX_PATH;
const char *window_title = "Halo";
const char *window_class_name = "Halo";

static HMODULE get_ringworld_module_handle(void) {
    HMODULE hModule = NULL;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)get_ringworld_module_handle, &hModule);
    return hModule;
}

static HMODULE get_executable_module_handle(void) {
    return *executable_module;
}

static HMODULE get_strings_module_handle(void) {
    return *strings_module;
}

static void set_window_handle(HWND handle) {
    *window_handle = handle;
}

static void set_window_style(DWORD style) {
    *window_style = style;
}

static void set_window_background_bitmap(HBITMAP bitmap) {
    *window_background_bitmap = bitmap;
}

static void set_window_memory_dc(HDC context) {
    *window_memory_dc = context;
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

bool shell_create_window(int32_t width, int32_t height) {
    HMODULE executable_module = get_executable_module_handle();
    
    WNDCLASSEXA window_class;
    ZeroMemory(&window_class, sizeof(window_class));
    window_class.lpfnWndProc = *window_proc;
    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_CLASSDC;
    window_class.hInstance = executable_module;
    window_class.hIcon = LoadIconA(executable_module, MAKEINTRESOURCEA(EXECUTABLE_RESOURCE_ID_ICON));
    window_class.hIconSm = LoadIconA(executable_module, MAKEINTRESOURCEA(EXECUTABLE_RESOURCE_ID_ICON));
    window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);
    window_class.lpszClassName = window_class_name;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName = NULL;
    RegisterClassExA(&window_class);

    // Calculate window position and size
    RECT rect;
    HWND desktop_window = GetDesktopWindow();
    GetWindowRect(desktop_window, &rect);
    DWORD window_style = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
    rect.left = ((rect.right - rect.left) - width) / 2.0f;
    rect.right = rect.left + width;
    rect.top = ((rect.bottom - rect.top) - height) / 2.0f;
    rect.bottom = rect.top + height;
    AdjustWindowRect(&rect, window_style, FALSE);
    int32_t window_pos_x = rect.left;
    int32_t window_pos_y = rect.top;
    int32_t window_width = rect.right - rect.left;
    int32_t window_height = rect.bottom - rect.top;

    HWND window_handle = CreateWindowExA(0, window_class_name, window_title, window_style, window_pos_x, window_pos_y,
                                    window_width, window_height, desktop_window, NULL, executable_module, NULL);

    if(window_handle == NULL) {
        DWORD error_code = GetLastError();
        UnregisterClassA(window_class_name, executable_module);
        exception_throw_runtime_error("Failed to create window with error code: 0x%08X", error_code);
    }

    // Disable Windows 11 rounded corners
    DWORD window_corner_attr = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(window_handle, DWMWA_WINDOW_CORNER_PREFERENCE, &window_corner_attr, sizeof(window_corner_attr));

    HMODULE strings_module = get_strings_module_handle();
    HBITMAP background_bitmap = LoadBitmapA(strings_module, MAKEINTRESOURCEA(STRING_RESOURCE_ID_SPLASH_BMP));
    if(background_bitmap != NULL) {
        HDC window_dc = GetDC(window_handle);
        HDC window_memory_dc = CreateCompatibleDC(window_dc);
        set_window_memory_dc(window_memory_dc);
        SelectObject(window_memory_dc, background_bitmap);
    }
    set_window_style(window_style);
    set_window_handle(window_handle);
    
    SetForegroundWindow(window_handle);
    SetActiveWindow(window_handle);
    SetFocus(window_handle);
    ShowWindow(window_handle, SW_SHOW);

    return true;
}

size_t shell_get_process_argc(void) {
    return *process_argc;
}

const char **shell_get_process_argv(void) {
    return *process_argv;
}

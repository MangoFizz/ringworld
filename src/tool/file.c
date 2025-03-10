#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <shlwapi.h>
#include "../impl/exception/exception.h"
#include "file.h"

bool directory_exists(const char *path) {
    DWORD dir_attributes = GetFileAttributesA(path);
    return dir_attributes != INVALID_FILE_ATTRIBUTES;
}

bool file_exists(const char *path) {
    DWORD file_attributes = GetFileAttributesA(path);
    return file_attributes != INVALID_FILE_ATTRIBUTES && !(file_attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void create_directory(const char *path) {
    if(!directory_exists(path)) {
        CreateDirectoryA(path, NULL);
    }
}

void change_directory(const char *path) {
    if(!directory_exists(path)) {
        CRASHF_DEBUG("Directory \"%s\" does not exist.", path);
    }
    SetCurrentDirectoryA(path);
}

void remove_directory(const char *path) {
    if(directory_exists(path)) {
        SHFILEOPSTRUCTA file_op = {
            .wFunc = FO_DELETE,
            .pFrom = path,
            .fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT
        };
        SHFileOperationA(&file_op);
    }
}

void get_current_directory(char *path) {
    GetCurrentDirectoryA(MAX_PATH, path);
}

void write_file(const char *path, void *data, size_t lenght) {
    ASSERT(path != NULL);
    ASSERT(data != NULL);
    if(lenght > 0) {
        HANDLE file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(file == INVALID_HANDLE_VALUE) {
            CRASHF_DEBUG("Could not open file \"%s\" for writing.", path);
        }
        DWORD bytes_written;
        WriteFile(file, data, lenght, &bytes_written, NULL);
        CloseHandle(file);
        ASSERT(bytes_written > 0);
    }
}

void read_file(const char *path, void **data, size_t *lenght) {
    ASSERT(path != NULL);
    ASSERT(data != NULL);
    ASSERT(lenght != NULL);
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) {
        CRASHF_DEBUG("Could not open file \"%s\" for reading.", path);
    }
    *lenght = GetFileSize(file, NULL);
    *data = GlobalAlloc(GPTR, *lenght);
    DWORD bytes_read;
    ReadFile(file, *data, *lenght, &bytes_read, NULL);
    CloseHandle(file);
    ASSERT(bytes_read > 0);
}

void get_directory_name_from_path(const char *path, char *output) {
    const char *last_slash = strrchr(path, '\\');
    if(last_slash == NULL) {
        last_slash = path;
    }
    size_t length = last_slash - path;
    strncpy(output, path, length);
    output[length] = '\0';
}

void get_filename_without_extension_from_path(const char *path, char *output) {
    const char *last_slash = strrchr(path, '\\');
    if(last_slash == NULL) {
        last_slash = path;
    }
    const char *last_dot = strrchr(last_slash, '.');
    if(last_dot == NULL) {
        last_dot = path + strlen(path);
    }
    size_t length = last_dot - last_slash;
    strncpy(output, last_slash, length);
    output[length] = '\0';
}

const char *get_extension_from_path(const char *path) {
    const char *last_dot = strrchr(path, '.');
    if(last_dot == NULL) {
        return path + strlen(path);
    }
    return last_dot + 1;
}

void iterate_directories(const wchar_t *path, IterationCallback callback, void *userdata, IterationSortCompareFunc sort_compare) {
    WIN32_FIND_DATAW findData;
    wchar_t searchPath[MAX_PATH];
    swprintf_s(searchPath, MAX_PATH, L"%s\\*", path);

    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if(hFind == INVALID_HANDLE_VALUE) {
        fwprintf(stderr, L"Could not find any files in \"%s\".\n", path);
        return;
    }

    wchar_t **fileList = NULL;
    size_t fileCount = 0;

    do {
        if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if(wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
                fileList = realloc(fileList, (fileCount + 1) * sizeof(wchar_t *));
                fileList[fileCount] = _wcsdup(findData.cFileName);
                fileCount++;
            }
        }
    } while(FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);

    qsort(fileList, fileCount, sizeof(wchar_t *), sort_compare);

    for(size_t i = 0; i < fileCount; i++) {
        if(callback) {
            callback(fileList[i], userdata);
        }
        free(fileList[i]);
    }
    free(fileList);
}

void iterate_files_by_extension(const wchar_t *path, IterationCallback callback, void *userdata, IterationSortCompareFunc sort_compare) {
    WIN32_FIND_DATAW findData;
    wchar_t searchPath[MAX_PATH];
    swprintf_s(searchPath, MAX_PATH, L"%s\\*", path);

    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if(hFind == INVALID_HANDLE_VALUE) {
        fwprintf(stderr, L"Could not find any files in \"%s\".\n", path);
        return;
    }

    wchar_t **fileList = NULL;
    size_t fileCount = 0;

    do {
        if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            fileList = realloc(fileList, (fileCount + 1) * sizeof(wchar_t *));
            fileList[fileCount] = _wcsdup(findData.cFileName);
            fileCount++;
        }
    } while(FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);

    qsort(fileList, fileCount, sizeof(wchar_t *), sort_compare);

    for(size_t i = 0; i < fileCount; i++) {
        if(callback) {
            callback(fileList[i], userdata);
        }
        free(fileList[i]);
    }
    free(fileList);
}

size_t count_files_in_directory(const wchar_t *path) {
    WIN32_FIND_DATAW findData;
    wchar_t searchPath[MAX_PATH];
    swprintf_s(searchPath, MAX_PATH, L"%s\\*", path);

    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if(hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    size_t count = 0;
    do {
        if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            count++;
        }
    } while(FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);
    return count;
}

size_t count_directories_in_directory(const wchar_t *path) {
    WIN32_FIND_DATAW findData;
    wchar_t searchPath[MAX_PATH];
    swprintf_s(searchPath, MAX_PATH, L"%s\\*", path);

    HANDLE hFind = FindFirstFileW(searchPath, &findData);
    if(hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    int count = 0;
    do {
        if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
            wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
            count++;
        }
    } while(FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);
    return count;
}

void random_temp_name(char *output) {
    char temp_name[MAX_PATH];
    GetTempFileNameA(".", "tmp", 0, temp_name);
    DeleteFileA(temp_name);
    get_filename_without_extension_from_path(temp_name + 2, output);
}

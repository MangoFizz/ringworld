#ifndef RINGWORLD_TOOL__FILE_H
#define RINGWORLD_TOOL__FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef void (*IterationCallback)(const wchar_t *directory, void *userdata);
typedef int (*IterationSortCompareFunc)(const void *a, const void *b);

bool directory_exists(const char *path);
bool file_exists(const char *path);
void create_directory(const char *path);
void change_directory(const char *path);
void remove_directory(const char *path);
void get_current_directory(char *path);
void write_file(const char *path, void *data, size_t lenght);
void read_file(const char *path, void **data, size_t *lenght);
void get_directory_name_from_path(const char *path, char *output);
void get_filename_without_extension_from_path(const char *path, char *output);
const char *get_extension_from_path(const char *path);
void iterate_directories(const wchar_t *path, IterationCallback callback, void *userdata, IterationSortCompareFunc sort_compare);
void iterate_files_by_extension(const wchar_t *path, IterationCallback callback, void *userdata, IterationSortCompareFunc sort_compare);
size_t count_files_in_directory(const wchar_t *path);
size_t count_directories_in_directory(const wchar_t *path);
void random_temp_name(char *output);

#ifdef __cplusplus
}
#endif

#endif

#ifndef RINGWORLD__SHELL__SHELL_H
#define RINGWORLD__SHELL__SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct _iobuf FILE;

/**
 * Get the path to the documents directory.
 * @param buffer The buffer to store the path; should be at least MAX_PATH.
 */
void shell_get_documents_directory_path(char *buffer);

/**
 * Get the path to the data directory where the saved games and other data are stored.
 * @param buffer The buffer to store the path; should be at least MAX_PATH.
 */
void shell_get_data_directory_path(char *buffer);

/**
 * Display a message box with the given format.
 * @param fmt The format string.
 * @param ... The values to format.
 */
void shell_message_box(const char *fmt, ...);

/**
 * Display an error message box with the given format.
 * @param fmt The format string.
 * @param ... The values to format.
 */
void shell_error_box(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

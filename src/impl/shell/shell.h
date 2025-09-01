#ifndef RINGWORLD__SHELL__SHELL_H
#define RINGWORLD__SHELL__SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * Maximum path length.
 */
extern const size_t MAX_PATH_LENGTH;

/**
 * Get the path to the ringworld module.
 * @param out_buffer The buffer to store the path; should be at least MAX_PATH_LENGTH.
 * @param buffer_size The size of the buffer.
 * @return An error code indicating the success or failure of the operation.
 */
int shell_get_ringworld_module_path(char *out_buffer, size_t buffer_size);

/**
 * Get the path to the documents directory.
 * @param buffer The buffer to store the path; should be at least MAX_PATH.
 * @param buffer_size The size of the buffer.
 * @return An error code indicating the success or failure of the operation.
 */
int shell_get_documents_directory_path(char *out_buffer, size_t buffer_size);

/**
 * Get the path to the data directory where the saved games and other data are stored.
 * @param buffer The buffer to store the path; should be at least MAX_PATH.
 * @param buffer_size The size of the buffer.
 * @return An error code indicating the success or failure of the operation.
 * @throw Runtime error exception if the creation of the directory fails.
 */
int shell_get_data_directory_path(char *out_buffer, size_t buffer_size);

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

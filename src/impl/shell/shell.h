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

/**
 * Create a window with the given dimensions.
 * @param width The width of the window.
 * @param height The height of the window.
 * @return True if the window was created successfully, false otherwise.
 */
bool shell_create_window(int32_t width, int32_t height);

/**
 * Get the number of command line arguments passed to the process.
 * @return The number of command line arguments.
 */
size_t shell_get_process_argc(void);

/**
 * Get the command line arguments passed to the process.
 * @return An array of strings representing the command line arguments.
 */
const char **shell_get_process_argv(void);

/**
 * Get whether the game is running in headless mode.
 * @return True if the process is running in headless mode, false otherwise.
 */
bool shell_is_headless_mode(void);

#ifdef __cplusplus
}
#endif

#endif

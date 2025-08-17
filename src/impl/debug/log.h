#ifndef RINGWORLD__DEBUG__LOG_H
#define RINGWORLD__DEBUG__LOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

/**
 * Write a log message with a specific log level.
 * @param level The log level (LOG_INFO, LOG_WARN, LOG_ERROR).
 * @param fmt   The format string for the log message.
 * @param ...   The values to format into the log message.
 */
void log_write(LogLevel level, const char* fmt, ...);

/**
 * Write a debug log message.
 * @param fmt The format string for the log message.
 * @param ... The values to format into the log message.
 */
#define log_debug(...)  log_write(LOG_DEBUG, __VA_ARGS__)

/**
 * Write an info log message.
 * @param fmt The format string for the log message.
 * @param ... The values to format into the log message.
 */
#define log_info(...)  log_write(LOG_INFO,  __VA_ARGS__)

/**
 * Write a warning log message.
 * @param fmt The format string for the log message.
 * @param ... The values to format into the log message.
 */
#define log_warn(...)  log_write(LOG_WARN,  __VA_ARGS__)

/**
 * Write an error log message.
 * @param fmt The format string for the log message.
 * @param ... The values to format into the log message.
 */
#define log_error(...) log_write(LOG_ERROR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

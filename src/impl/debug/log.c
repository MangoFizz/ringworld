#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>
#include "shell/shell.h"
#include "version.h"
#include "log.h"

static FILE *log_file = NULL;
static int log_initialized = 0;

static void log_close_internal() {
    if(log_file) {
        fputs("\n\n", log_file);
        fclose(log_file);
        log_file = NULL;
    }
}

static void log_get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);
    snprintf(buffer, size, "%02d/%02d/%02d %02d:%02d:%02d", t.tm_mday, t.tm_mon + 1, t.tm_year % 100, t.tm_hour, t.tm_min, t.tm_sec);
}

int log_init() {
    if(log_initialized) {
        return 1;
    }

    char data_directory[MAX_PATH];
    shell_get_data_directory_path(data_directory);
    
    char filename[MAX_PATH];
    snprintf(filename, sizeof(filename), "%s\\debug.txt", data_directory);
    log_file = fopen(filename, "a");
    if(!log_file) {
        perror("Failed to open log file");
        return 0;
    }

    char time_str[32];
    log_get_timestamp(time_str, sizeof(time_str));
    fprintf(log_file, "%s ringworld %s -----------------------------------------------------------\n", time_str, RINGWORLD_VERSION_STRING);

    log_initialized = 1;
    atexit(log_close_internal); 
    return 1;
}

void log_close() {
    log_close_internal();
}

void log_write(LogLevel level, const char* fmt, ...) {
    if(!log_file) {
        log_init();
    }

    char timestamp[32];
    log_get_timestamp(timestamp, sizeof(timestamp));

    const char* level_str = "";
    switch(level) {
        case LOG_INFO:  
            level_str = "INFO";  
            break;
        case LOG_WARN:  
            level_str = "WARN";  
            break;
        case LOG_ERROR: 
            level_str = "ERROR"; 
            break;
    }

    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "%s  [%s]  ", timestamp, level_str);
    vfprintf(log_file, fmt, args);
    fputc('\n', log_file);
    fflush(log_file);
    va_end(args);

    va_start(args, fmt);
    vprintf(fmt, args);
    fputc('\n', stdout);
    va_end(args);
}

/**
 * @file log.c
 * @brief Logging utility for various log levels including DEBUG, INFO, and ERROR.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define MAX_LOG_MSG 1024

char *logPath = "general.log"; // Path to the log file.
int logLevel = 1;  // Log level: 3 (DEBUG), 2 (INFO), 1 (ERROR), 0 (NONE).

/**
 * @brief Retrieves the current time formatted as a string.
 *
 * This function populates the provided buffer with the current time
 * formatted as HH:MM:SS DD-MM-YYYY.
 *
 * @param buffer A buffer to store the formatted time string.
 * @param bufferSize The size of the buffer.
 * @return Returns 0 on success.
 */
int getCurrentTime(char *buffer, size_t bufferSize);

/**
 * @brief Logs a message with the specified log level.
 *
 * This function formats a log message and writes it to the log file with a timestamp
 * and the specified log level.
 *
 * @param level The log level as a string (e.g., "DEBUG", "INFO", "ERROR").
 * @param format A printf-style format string for the log message.
 * @return Returns 0 on success, or 1 if the log file cannot be opened.
 */
int logWithLevel(const char *level, const char *format, ...);

/**
 * @brief Logs a debug-level message.
 *
 * This function writes a debug message to the log file if the current log level
 * is set to DEBUG (3) or higher.
 *
 * @param format A printf-style format string for the log message.
 * @return Returns 0 on success, or 1 if the log file cannot be opened.
 */
int debug(const char *format, ...);

/**
 * @brief Logs an info-level message.
 *
 * This function writes an informational message to the log file if the current log level
 * is set to INFO (2) or higher.
 *
 * @param format A printf-style format string for the log message.
 * @return Returns 0 on success, or 1 if the log file cannot be opened.
 */
int info(const char *format, ...);

/**
 * @brief Logs an error-level message.
 *
 * This function writes an error message to the log file if the current log level
 * is set to ERROR (1) or higher.
 *
 * @param format A printf-style format string for the log message.
 * @return Returns 0 on success, or 1 if the log file cannot be opened.
 */
int error(const char *format, ...);

int getCurrentTime(char *buffer, size_t bufferSize) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, bufferSize, "%H:%M:%S %d-%m-%Y", timeinfo);
    return 0;
}

int logWithLevel(const char *level, const char *format, ...) {
    FILE *file = fopen(logPath, "a");
    if (!file) {
        printf("Error opening log file: %s\n", logPath);
        return 1;
    }

    char timeBuffer[80];
    getCurrentTime(timeBuffer, sizeof(timeBuffer));

    char logMsg[MAX_LOG_MSG];
    va_list args;
    va_start(args, format);
    vsnprintf(logMsg, MAX_LOG_MSG, format, args);
    va_end(args);

    fprintf(file, "[%6s | %19s] %s\n", level, timeBuffer, logMsg);

    fclose(file);
    return 0;
}

int debug(const char *format, ...) {
    if (logLevel >= 3) {
        va_list args;
        va_start(args, format);
        char logMsg[MAX_LOG_MSG];
        vsnprintf(logMsg, MAX_LOG_MSG, format, args);
        va_end(args);
        return logWithLevel("DEBUG", "%s", logMsg);
    }
    return 0;
}

int info(const char *format, ...) {
    if (logLevel >= 2) {
        va_list args;
        va_start(args, format);
        char logMsg[MAX_LOG_MSG];
        vsnprintf(logMsg, MAX_LOG_MSG, format, args);
        va_end(args);
        return logWithLevel("INFO", "%s", logMsg);
    }
    return 0;
}

int error(const char *format, ...) {
    if (logLevel >= 1) {
        va_list args;
        va_start(args, format);
        char logMsg[MAX_LOG_MSG];
        vsnprintf(logMsg, MAX_LOG_MSG, format, args);
        va_end(args);
        return logWithLevel("ERROR", "%s", logMsg);
    }
    return 0;
}

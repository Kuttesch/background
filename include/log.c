#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define MAX_LOG_MSG 1024

char *logPath = "general.log";
int logLevel = 1;  // 3: DEBUG, 2: INFO, 1: ERROR, 0: NONE

// Function to get the current time in a formatted string
int getCurrentTime(char *buffer, size_t bufferSize) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, bufferSize, "%H:%M:%S %d-%m-%Y", timeinfo);
    return 0;
}

// General-purpose logging function
int logWithLevel(const char *level, const char *format, ...) {
    FILE *file = fopen(logPath, "a");
    if (!file) {
        printf("Error opening log file: %s\n", logPath);
        return 1;
    }

    char timeBuffer[80];
    getCurrentTime(timeBuffer, sizeof(timeBuffer));

    // Format the log message
    char logMsg[MAX_LOG_MSG];
    va_list args;
    va_start(args, format);
    vsnprintf(logMsg, MAX_LOG_MSG, format, args);
    va_end(args);

    // Write the log entry to the file
    fprintf(file, "[%6s | %19s] %s\n", level, timeBuffer, logMsg);

    fclose(file);
    return 0;
}

// Debug log
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

// Info log
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

// Error log
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

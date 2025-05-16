#ifndef LOG_H
#define LOG_H

#include <stdio.h>


int setLogPath(char *path);
int setLogLevel(char *level);
int debug(const char *format, ...);
int error(const char *format, ...);
int info(const char *format, ...);
#endif // INI_H

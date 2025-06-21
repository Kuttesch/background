#ifndef INI_H
#define INI_H

#include <stdio.h>
#include <string.h>

// Function to read a specific key-value pair from an INI file
int readIniValue(const char *configPath, const char *section, const char *key, char *value);
int writeIniValue(const char *configPath, const char *section, const char *key, char *value);
#endif // INI_H

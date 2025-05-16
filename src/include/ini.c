/**
 * @file ini.c
 * @brief This file contains functions for reading and writing values in INI configuration files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

/**
 * @brief Reads a value associated with a given key from a specific section in an INI file.
 *
 * This function searches for a specified section and key in the INI file and retrieves the
 * corresponding value if found.
 *
 * @param configPath The path to the INI configuration file.
 * @param section The section in the INI file where the key is located.
 * @param key The key whose value needs to be read.
 * @param value A buffer to store the value read from the INI file.
 * @return Returns 0 if the value is successfully read, or 1 if an error occurs or the key is not found.
 */
int readIniValue(char *configPath, char *section, char *key, char *value);

/**
 * @brief Writes a value to a specified key in a given section of an INI file.
 *
 * This function updates the value of a key in the specified section. If the key does not exist,
 * it is added to the section. If the section does not exist, it is created.
 *
 * @param configPath The path to the INI file.
 * @param section The section in the INI file where the key is located.
 * @param key The key to which the value should be written.
 * @param value The value to write to the specified key.
 * @return Returns 0 on success, or 1 if an error occurs.
 */
int writeIniValue(const char *configPath, const char *section, const char *key, const char *value);

int readIniValue(char *configPath, char *section, char *key, char *value) {
    FILE *file = fopen(configPath, "r");
    if (file == NULL) {
        return 1;
    }

    char line[512];
    int insideSection = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (line[0] == '[') {
            if (strncmp(line + 1, section, strlen(section)) == 0) {
                insideSection = 1;
            } else {
                insideSection = 0;
            }
        }

        if (insideSection && strncmp(line, key, strlen(key)) == 0) {
            sscanf(line, "%*s = %s", value);
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int writeIniValue(const char *configPath, const char *section, const char *key, const char *value) {
    FILE *file = fopen(configPath, "r");
    if (file == NULL) {
        return 1;
    }

    char content[2048];
    content[0] = '\0';
    
    char line[512];
    int insideSection = 0;
    int keyFound = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (insideSection && strncmp(line, key, strlen(key)) == 0) {
            keyFound = 1;
            strcat(content, key);
            strcat(content, " = ");
            strcat(content, value);
            strcat(content, "\n");
            continue;
        }

        if (line[0] == '[' && strncmp(line + 1, section, strlen(section)) == 0) {
            insideSection = 1;
        }

        strcat(content, line);
        strcat(content, "\n");
    }

    if (!keyFound) {
        if (insideSection) {
            strcat(content, key);
            strcat(content, " = ");
            strcat(content, value);
            strcat(content, "\n");
        } else {
            strcat(content, "[");
            strcat(content, section);
            strcat(content, "]\n");
            strcat(content, key);
            strcat(content, " = ");
            strcat(content, value);
            strcat(content, "\n");
        }
    }

    fclose(file);

    file = fopen(configPath, "w");
    if (file == NULL) {
        return 1;
    }

    fputs(content, file);
    fclose(file);

    return 0;
}

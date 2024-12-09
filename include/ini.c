/**
 * @file ini.c
 * @brief This file contains functions for reading values from INI configuration files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

/**
 * @brief Reads a value from an INI configuration file.
 *
 * This function reads a value associated with a given key within a specified section
 * from an INI configuration file.
 *
 * @param configPath The path to the INI configuration file.
 * @param section The section in the INI file where the key is located.
 * @param key The key whose value needs to be read.
 * @param value A buffer to store the value read from the INI file.
 * @return Returns 0 if the value is successfully read, otherwise returns 1.
 */
int readIniValue(char *configPath, char *section, char *key, char *value);


/**
 * @brief Writes a value to a specified key in a given section of an INI file.
 *
 * @param configPath The path to the INI file.
 * @param section The section in the INI file where the key is located.
 * @param key The key within the section to which the value should be written.
 * @param value The value to write to the specified key.
 * @return int Returns 0 on success, or a non-zero error code on failure.
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

    // Read the entire file into memory
    char content[2048]; // Make sure this is large enough for your file
    content[0] = '\0';
    
    char line[512];
    int insideSection = 0;
    int keyFound = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove trailing newline

        // If inside the correct section, check if the key is present
        if (insideSection && strncmp(line, key, strlen(key)) == 0) {
            keyFound = 1;
            // Update the key-value pair
            strcat(content, key);
            strcat(content, " = ");
            strcat(content, value);
            strcat(content, "\n");
            continue;  // Skip appending the old line, as we just updated it
        }

        // If the line indicates the section, set insideSection to true
        if (line[0] == '[' && strncmp(line + 1, section, strlen(section)) == 0) {
            insideSection = 1;
        }

        // Append the line to the content buffer
        strcat(content, line);
        strcat(content, "\n");
    }

    // If the key was not found, append the key-value pair to the section
    if (!keyFound) {
        if (insideSection) {
            strcat(content, key);
            strcat(content, " = ");
            strcat(content, value);
            strcat(content, "\n");
        } else {
            // If the section is not found, add the section and the key-value pair
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

    // Write the modified content back to the file
    file = fopen(configPath, "w"); // Open file in write mode to overwrite content
    if (file == NULL) {
        return 1;
    }

    fputs(content, file);
    fclose(file);

    return 0;
}

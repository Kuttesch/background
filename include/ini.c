#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"


// Function to read a specific section and key from the INI file
int readIniValue(char *configPath, char *section, char *key, char *value) {
    FILE *file = fopen(configPath, "r");
    if (file == NULL) {
        // error("Failed to open file");
        return 1; // Return 1 for error in file opening
    }

    char line[512];
    int insideSection = 0;

    // Iterate over each line in the file
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if it exists
        line[strcspn(line, "\n")] = 0;

        // Check if the line marks a section
        if (line[0] == '[') {
            // Check if we are entering the requested section
            if (strncmp(line + 1, section, strlen(section)) == 0) {
                insideSection = 1;
            } else {
                insideSection = 0;
            }
        }

        // If we are inside the section, look for the key
        if (insideSection && strncmp(line, key, strlen(key)) == 0) {
            // Extract the value associated with the key
            sscanf(line, "%*s = %s", value);
            fclose(file);
            // debug("Value: %s", value);
            return 0; // Return 0 for successful reading
        }
    }

    fclose(file);
    return 1; // Return 1 if the key was not found in the specified section
}

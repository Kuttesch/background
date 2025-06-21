#include <stdio.h>
#include <windows.h>
#include "log.h"

#define MAX_PATH 260
#define MAX_LINE_LENGTH 256

/**
 * @brief Sets the desktop background and lock screen background using the specified image.
 * 
 * @param imagePath Path to the image file to be set as background.
 * @return 0 if successful, 1 if any operation fails.
 */
int setBackground(char *imagePath);

/**
 * @brief Sets the desktop wallpaper to the specified image.
 * 
 * @param imagePath Path to the image file to be set as the desktop wallpaper.
 * @return 0 if successful, 1 otherwise.
 */
int setDesktopBackground(char *imagePath);

/**
 * @brief Sets the lock screen background to the specified image.
 * 
 * @param imagePath Path to the image file to be set as the lock screen background.
 * @return 0 if successful, 1 otherwise.
 */
int setLockscreenBackground(char *imagePath);

int setBackground(char *imagePath) {
    if (setDesktopBackground(imagePath) != 0) {
        error("Failiure setting Desktop Background!");
        return 1;
    }

    if (setLockscreenBackground(imagePath) != 0) {
        error("Failiure setting Lockscreen Background!");
        return 1;
    }

    return 0;
}

int setDesktopBackground(char *imagePath) {
    char absolutePath[MAX_PATH];
    
    if (!GetFullPathNameA(imagePath, MAX_PATH, absolutePath, NULL)) {
        error("Failiure converting to absolute path: %ld", GetLastError());
        return 1;
    }
    if (!SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void *)absolutePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) {
        error("Failiure setting wallpaper: %ld", GetLastError());
        return 1;
    }
    return 0;
}

int setLockscreenBackground(char *imagePath) {
    return 0;
}
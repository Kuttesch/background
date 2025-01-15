#include <stdio.h>
#include <windows.h>
#include "log.h"

#define MAX_PATH 260
#define MAX_LINE_LENGTH 256

int setDesktopBackground(char *imagePath);
int setLockscreenBackground(char * imagePath);
int setBackground(char *imagePath);

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
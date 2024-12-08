#include <stdio.h>
#include <windows.h>

#define MAX_PATH 260
#define MAX_LINE_LENGTH 256

int changeBackground(char *nightImagePath, char *dayImagePath, int *backgroundStatePtr, int *fromTimePtr, int *toTimePtr);
int setBackground(char *nightImagePath, char *dayImagePath, int *backgroundStatePtr);
int getTime(int *backgroundStatePtr, int *fromTimePtr, int *toTimePtr);

int changeBackground(char *nightImagePath, char *dayImagePath, int *backgroundStatePtr, int *fromTimePtr, int *toTimePtr) {
    if (getTime(backgroundStatePtr, fromTimePtr, toTimePtr) != 0) {
        printf("Error getting time\n");
        return 1;
    }
    if (setBackground(nightImagePath, dayImagePath, backgroundStatePtr) != 0) {
        printf("Error setting background\n");
        return 1;
    }
    return 0;
}

int setBackground(char *nightImagePath, char *dayImagePath, int *backgroundStatePtr) {
    char absolutePath[MAX_PATH];
    const char *relativePath = *backgroundStatePtr == 1 ? nightImagePath : dayImagePath;

    if (!GetFullPathNameA(relativePath, MAX_PATH, absolutePath, NULL)) {
        printf("Error converting to absolute path: %ld\n", GetLastError());
        return 1;
    }
    if (!SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void *)absolutePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) {
        printf("Error setting wallpaper: %ld\n", GetLastError());
        return 1;
    }
    return 0;
}

int getTime(int *backgroundStatePtr, int *fromTimePtr, int *toTimePtr) {
    SYSTEMTIME time;
    GetLocalTime(&time);
    int hour = time.wHour;
    if (*fromTimePtr < 24 && *toTimePtr < 24 && *fromTimePtr < *toTimePtr) {
        if (hour >= *fromTimePtr && hour < *toTimePtr) {
            *backgroundStatePtr = 0;
        } else {
            *backgroundStatePtr = 1;
        }
    } else {
        printf("Invalid time\n");
        return 1;
    }
    return 0;
}

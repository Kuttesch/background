#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <windows.h>

// Constants
#define MAX_PATH 260
#define MAX_LINE_LENGTH 256

// Global Variables
extern int backgroundState;

// Function Declarations
int changeBackground(char *nightImagePath, char *dayImagePath, int *backgroundStatePtr, int *fromTimePtr, int *toTimePtr);
// int setBackground(char *nightImagePath, char *dayImagePath, int *backgroundStatePtr);
// int getTime(int *backgroundStatePtr);

#endif // BACKGROUND_H

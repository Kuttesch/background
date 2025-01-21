/**
 * @file systray.c
 * @brief This file contains the implementation of a system tray application that changes the desktop background based on the time of day.
 * 
 * The application reads configuration from an INI file, sets the background to a day or night image, and animates the system tray icon accordingly.
 * 
 * @include <stdio.h>
 * @include <windows.h>
 * @include "background.h"
 * @include <stdbool.h>
 * @include "resource.h"
 * @include "ini.h"
 * @include "log.h"
 * 
 * @global NOTIFYICONDATA notifData - Data structure for the system tray icon.
 * @global HINSTANCE hInstance - Handle to the application instance.
 * @global HWND hiddenWindow - Handle to the hidden window used for message processing.
 * @global HICON animationIcons[ANIMATION_FRAMES] - Array of icons for animation frames.
 * @global volatile bool stopThread - Flag to stop the background thread.
 * @global int sleepTime - Sleep time between background checks.
 * @global char nightPath[MAX_VALUE_LENGTH] - Path to the night background image.
 * @global char dayPath[MAX_VALUE_LENGTH] - Path to the day background image.
 * @global int fromTime - Start time for the day background.
 * @global int toTime - End time for the day background.
 * @global int backgroundState - Current background state (DAY or NIGHT).
 * @global volatile bool day2Night - Flag indicating if the transition is from day to night.
 * 
 * @define CONFIG_PATH - Path to the configuration file.
 * @define CONFIG_PATH_SIZE - Size of the configuration path.
 * @define MAX_VALUE_LENGTH - Maximum length of configuration values.
 * @define ANIMATION_FRAMES - Number of frames in the icon animation.
 * @define NIGHT - Constant representing night state.
 * @define DAY - Constant representing day state.
 * 
 * @function loadAnimationIcons - Loads the icons for the animation frames.
 * @function cleanupAnimationIcons - Cleans up the loaded animation icons.
 * @function animateIconDayToNight - Animates the icon from day to night.
 * @function animateIconNightToDay - Animates the icon from night to day.
 * @function changeBackground - Changes the desktop background based on the current state.
 * @function WinMain - Entry point for the application.
 * @function WindowProc - Window procedure for handling messages.
 * @function makeAbsolutePath - Converts a relative path to an absolute path.
 * @function createConfig - Creates a default configuration file.
 * @function readConfig - Reads the configuration from the INI file.
 * @function updateBackgroundStateConfig - Updates the background state in the configuration file.
 * @function checkIfConfig - Checks if the configuration file exists and creates it if necessary.
 * @function programLoop - Main loop for the background thread.
 * @function ProgramLoopThread - Thread function for the program loop.
 * @function initializeMain - Initializes the main components of the application.
 * @function initializeAnimation - Initializes the animation based on the current background state.
 * @function setBackgroundState - Sets the background state based on the current time.
 */

#include <stdio.h>
#include <windows.h>
#include "background.h"
#include <stdbool.h>
#include "resource.h"
#include "ini.h"
#include "log.h"

// Constants
#define CONFIG_PATH "./config.ini"
#define CONFIG_PATH_SIZE sizeof(CONFIG_PATH)
#define MAX_VALUE_LENGTH 128
#define ANIMATION_FRAMES 21
#define NIGHT 1
#define DAY 0

// Global variables
NOTIFYICONDATA notifData; // Data structure for the system tray icon.
HINSTANCE hInstance; // Handle to the application instance.
HWND hiddenWindow; // Handle to the hidden window used for message processing.
HICON animationIcons[ANIMATION_FRAMES]; // Array of icons for animation frames.
volatile bool stopThread = false; // Flag to stop the background thread.
int sleepTime = 30; // Sleep time between background checks.

char nightPath[MAX_VALUE_LENGTH]; // Path to the night background image.
char dayPath[MAX_VALUE_LENGTH]; // Path to the day background image.
int fromTime, toTime; // Time ranges for day and night backgrounds.
int backgroundState = DAY; // Current background state (DAY or NIGHT).
volatile bool day2Night = true; // Flag indicating if the transition is from day to night.


// ### Function definitions ### //


/**
 * @brief Loads the icons for the animation frames.
 */
void loadAnimationIcons();

/**
 * @brief Cleans up the loaded animation icons.
 */
void cleanupAnimationIcons();

/**
 * @brief Animates the system tray icon from day to night.
 */
void animateIconDayToNight();

/**
 * @brief Animates the system tray icon from night to day.
 */
void animateIconNightToDay();

/**
 * @brief Changes the desktop background based on the current state.
 * 
 * @return 0 on success, non-zero on failure.
 */
int changeBackground();

/**
 * @brief Entry point for the application.
 * 
 * @param hInst Handle to the application instance.
 * @param hPrevInst Handle to the previous instance (unused).
 * @param lpCmdLine Command line arguments.
 * @param nCmdShow Initial window show state.
 * 
 * @return Exit code.
 */
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow);

/**
 * @brief Window procedure for handling messages.
 * 
 * @param hiddenWindow Handle to the hidden window.
 * @param uMsg Message identifier.
 * @param wParam First message parameter.
 * @param lParam Second message parameter.
 * 
 * @return Result of the message processing.
 */
LRESULT CALLBACK WindowProc(HWND hiddenWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * @brief Converts a relative path to an absolute path.
 * 
 * @param relativePath The relative path to convert.
 * @param absolutePath Buffer to store the absolute path.
 * 
 * @return 0 on success, non-zero on failure.
 */
int makeAbsolutePath(char *relativePath, char *absolutePath);

/**
 * @brief Creates a default configuration file.
 * 
 * @return 0 on success, non-zero on failure.
 */
int createConfig();

/**
 * @brief Reads the configuration from the INI file.
 * 
 * @return 0 on success, non-zero on failure.
 */
int readConfig();

/**
 * @brief Updates the background state in the configuration file.
 * 
 * @return 0 on success, non-zero on failure.
 */
int updateBackgroundStateConfig();

/**
 * @brief Checks if the configuration file exists and creates it if necessary.
 * 
 * @return 0 on success, non-zero on failure.
 */
int checkIfConfig();

/**
 * @brief Main loop for the background thread.
 * 
 * @return 0 on success, non-zero on failure.
 */
int programLoop();

/**
 * @brief Thread function for the program loop.
 * 
 * @param lpParam Thread parameter (unused).
 * 
 * @return Exit code.
 */
DWORD WINAPI ProgramLoopThread(LPVOID lpParam);

/**
 * @brief Initializes the main components of the application.
 * 
 * @return 0 on success, non-zero on failure.
 */
int initializeMain();

/**
 * @brief Initializes the animation based on the current background state.
 * 
 * @return 0 on success, non-zero on failure.
 */
int initializeAnimation();


// ### Main Loop ### //


int programLoop() {
    
    if (readConfig(CONFIG_PATH, nightPath, dayPath, &fromTime, &toTime) != 0) {
        error("Failure reading config");
        return 1;
    }
    changeBackground();
    Sleep(1000); //!TODO Make mechanic for dynamic sleeptimes
    return 0;
}

int changeBackground() {
    int initialBackgroundState = backgroundState;
    debug("backgroundState: %d", backgroundState);

    setBackgroundState(&backgroundState, &fromTime, &toTime);
    if (backgroundState == NIGHT && initialBackgroundState == DAY) {
        setBackground(nightPath);
        animateIconDayToNight();
    } else if (backgroundState == DAY && initialBackgroundState == NIGHT ) {
        setBackground(dayPath);
        animateIconNightToDay();
    }
    return 0;
}


// ### Config ### //


//!TODO Overwork config
int checkIfConfig() {
    const char *configPathPtr = CONFIG_PATH;
    FILE *file = fopen(configPathPtr, "r");
    if (file == NULL) {
        if(createConfig() != 0) {
            error("Failed to create config file");
            return 1;
        } else {
            return checkIfConfig();
        }
    }
    fclose(file);
    return 0;

}

int createConfig() {
    const char *configPathPtr = CONFIG_PATH;
    FILE *file = fopen(configPathPtr, "w");
    if (file == NULL) {
        error("Failed to create config file");
        return 1;
    }
    fclose(file);
    writeIniValue(configPathPtr, "Path", "NIGHT", ".img\\night.jpg");
    writeIniValue(configPathPtr, "Path", "DAY", ".img\\day.jpg");
    writeIniValue(configPathPtr, "Time", "FROM", "0");
    writeIniValue(configPathPtr, "Time", "TO", "24");
    return 0;
}

int readConfig() {

    const char *configPathPtr = CONFIG_PATH;
    int *fromTimePtr = &fromTime;
    int *toTimePtr = &toTime;
    int *backgroundStatePtr = &backgroundState;

    const char *pathSection = "Path";
    const char *pathKeys[] = {"NIGHT", "DAY"};

    const char *timeSection = "Time";
    const char *timeKeys[] = {"FROM", "TO"};

    const char *stateSection = "State";
    const char *stateKeys[] = {"BACKGROUND"};

    for (int i = 0; i < sizeof(pathKeys) / sizeof(pathKeys[0]); i++) {
        char value[MAX_VALUE_LENGTH];
        if (readIniValue(configPathPtr, pathSection, pathKeys[i], value) != 0) {
            error("Failure reading path");
            return 1;
        }
        if (i == 0) {
            strcpy(nightPath, value);
        } else {
            strcpy(dayPath, value);
        }
    }

    for (int i = 0; i < sizeof(timeKeys) / sizeof(timeKeys[0]); i++) {
        char value[MAX_VALUE_LENGTH];
        if (readIniValue(configPathPtr, timeSection, timeKeys[i], value) != 0) {
            error("Failure reading time");
            return 1;
        }
        if (i == 0) {
            *fromTimePtr = atoi(value);
        } else {
            *toTimePtr = atoi(value);
        }
    }

    for (int i = 0; i < sizeof(stateKeys) / sizeof(timeKeys[0]); i++) {
        char value[MAX_VALUE_LENGTH];
        if (readIniValue(configPathPtr, stateSection, stateKeys[i], value) != 0) {
            error("Failiure reading state");
            //!TODO Add Error fallback
            return 1;
        } else {
            *backgroundStatePtr = atoi(value);
        }
    }
    
    return 0;
}

int updateBackgroundStateConfig() {

    const char *configPathPtr = CONFIG_PATH;
    char value[MAX_VALUE_LENGTH];
    int intValue;
    readIniValue(CONFIG_PATH, "State", "BACKGROUND", value);
    intValue = atoi(value);
    if (intValue != backgroundState) {
        char value[MAX_VALUE_LENGTH];
        snprintf (value, sizeof(value), "%d", backgroundState);
        writeIniValue(CONFIG_PATH, "State", "BACKGROUND", value);
    }
    return 0;
}


// ### WinAPI ### //


LRESULT CALLBACK WindowProc(HWND hiddenWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &notifData);
            PostQuitMessage(0);
            return 0;

        case WM_USER + 1:
            if (lParam == WM_RBUTTONDOWN) {
                HMENU hMenu = CreatePopupMenu();
                HMENU hSettingsMenu = CreatePopupMenu();
                HMENU hTimeMenuDay = CreatePopupMenu();
                HMENU hTimeMenuNight = CreatePopupMenu();


                AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSettingsMenu, TEXT("Settings"));
                    AppendMenu(hSettingsMenu, MF_POPUP, (UINT_PTR)hTimeMenuDay, TEXT("Set Day Time"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 100, TEXT("0"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 101, TEXT("1"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 102, TEXT("2"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 103, TEXT("3"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 104, TEXT("4"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 105, TEXT("5"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 106, TEXT("6"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 107, TEXT("7"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 108, TEXT("8"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 109, TEXT("9"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 110, TEXT("10"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 111, TEXT("11"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 112, TEXT("12"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 113, TEXT("13"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 114, TEXT("14"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 115, TEXT("15"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 116, TEXT("16"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 117, TEXT("17"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 118, TEXT("18"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 119, TEXT("19"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 120, TEXT("20"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 121, TEXT("21"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 122, TEXT("22"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 123, TEXT("23"));
                        AppendMenu(hTimeMenuDay, MF_STRING, 124, TEXT("24"));
                    AppendMenu(hSettingsMenu, MF_POPUP, (UINT_PTR)hTimeMenuNight, TEXT("Set Night Time"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 200, TEXT("0"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 201, TEXT("1"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 202, TEXT("2"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 203, TEXT("3"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 204, TEXT("4"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 205, TEXT("5"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 206, TEXT("6"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 207, TEXT("7"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 208, TEXT("8"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 209, TEXT("9"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 210, TEXT("10"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 211, TEXT("11"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 212, TEXT("12"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 213, TEXT("13"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 214, TEXT("14"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 215, TEXT("15"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 216, TEXT("16"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 217, TEXT("17"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 218, TEXT("18"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 219, TEXT("19"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 220, TEXT("20"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 221, TEXT("21"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 222, TEXT("22"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 223, TEXT("23"));
                        AppendMenu(hTimeMenuNight, MF_STRING, 224, TEXT("24"));

                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, 1, TEXT("Exit"));

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hiddenWindow);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hiddenWindow, NULL);
                DestroyMenu(hMenu);
            } else if (lParam == WM_LBUTTONDOWN) {
            }
            return 0;

        case WM_COMMAND:
            stopThread = true;
            if (LOWORD(wParam) == 1) {
                stopThread = true;      
                PostQuitMessage(0);     

            }else if (LOWORD(wParam) >= 100 && LOWORD(wParam) <= 124) {
                int param = LOWORD(wParam) - 100;
                info("Selected Day Time: %d", param);
                char value[MAX_VALUE_LENGTH];
                snprintf (value, sizeof(value), "%d", param);
                writeIniValue(CONFIG_PATH, "Time", "FROM", value);
                stopThread = false;
            } else if (LOWORD(wParam) >= 200 && LOWORD(wParam) <= 224) {
                int param = LOWORD(wParam) - 200;
                info("Selected Night Time: %d", param);
                char value[MAX_VALUE_LENGTH];
                snprintf (value, sizeof(value), "%d", param);
                writeIniValue(CONFIG_PATH, "Time", "TO", value);
                stopThread = false;
            }
            return 0;

    }
    return DefWindowProc(hiddenWindow, uMsg, wParam, lParam);
}

DWORD WINAPI ProgramLoopThread(LPVOID lpParam) {
    while (!stopThread) {
        if (programLoop() != 0) {
            error("Program loop encountered an error");
            break;
        }
        Sleep(10);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    hInstance = hInst;

    checkIfConfig(CONFIG_PATH);
    loadAnimationIcons();

    int *backgroundStatePtr = &backgroundState;


    char configPath[MAX_PATH] = CONFIG_PATH;

    if (readConfig(CONFIG_PATH, nightPath, dayPath, &fromTime, &toTime) != 0) {
        error("Failure initializing config");
        return 1;
    }

    if (makeAbsolutePath(configPath, configPath) != 0) {
        error("Failure converting config path to absolute");
        return 1;
    }

   
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = TEXT("WallCycle");
    RegisterClass(&wc);

   
    hiddenWindow = CreateWindow(wc.lpszClassName, TEXT("WallCycle"), 0, 0, 0, 0, 0, NULL, NULL, hInst, NULL);

   
    ZeroMemory(&notifData, sizeof(NOTIFYICONDATA));
    notifData.cbSize = sizeof(NOTIFYICONDATA);
    notifData.hWnd = hiddenWindow;

 
    notifData.uID = ANIMATION0;
    notifData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notifData.uCallbackMessage = WM_USER + 1;

    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_ID));
    lstrcpy(notifData.szTip, TEXT("WallCycle"));
    if (initializeMain() != 0) {
        error("Failiure Initializing!");
        return 1;
    }

    changeBackground(nightPath, dayPath, &backgroundState, &fromTime, &toTime);

    HANDLE hThread = CreateThread(NULL, 0, ProgramLoopThread, NULL, 0, NULL);
    if (hThread == NULL) {
        error("Failed to create thread for program loop");
        return 1;
    }

   
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

   
    stopThread = true;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

   
    Shell_NotifyIcon(NIM_DELETE, &notifData);
    return 0;

}


// ### initialization ### //


int initializeMain() {
    initializeAnimation();
    setBackgroundState(&backgroundState, &fromTime, &toTime);
    if (backgroundState == NIGHT) {
        setBackground(nightPath);
        animateIconDayToNight();
    } else if (backgroundState == DAY) {
        setBackground(dayPath);
        animateIconNightToDay();
    }
    return 0;
}


// ### Icon Animation ### //


int initializeAnimation () {
    setBackgroundState(&backgroundState, &fromTime, &toTime);
    if (backgroundState == DAY) {
        notifData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ANIMATION0));
        if (notifData.hIcon == NULL) {
            error("Failed to load tray icon");
            return 1;
        }
        Shell_NotifyIcon(NIM_ADD, &notifData);
    } else if (backgroundState == NIGHT) {
        notifData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ANIMATION20));
        if (notifData.hIcon == NULL) {
            error("Failed to load tray icon");
            return 1;
        }
        Shell_NotifyIcon(NIM_ADD, &notifData);
    } else {
        error("Invalid background state");
        return 1;
    }
    return 0;
}

void loadAnimationIcons() {
    int iconIds[ANIMATION_FRAMES] = {
        ANIMATION0, ANIMATION1, ANIMATION2, ANIMATION3, ANIMATION4,
        ANIMATION5, ANIMATION6, ANIMATION7, ANIMATION8, ANIMATION9,
        ANIMATION10, ANIMATION11, ANIMATION12, ANIMATION13, ANIMATION14,
        ANIMATION15, ANIMATION16, ANIMATION17, ANIMATION18, ANIMATION19, ANIMATION20
    };

    for (int i = 0; i < ANIMATION_FRAMES; i++) {
        animationIcons[i] = LoadIcon(hInstance, MAKEINTRESOURCE(iconIds[i]));
        if (animationIcons[i] == NULL) {
            error("Failed to load icon frame: %d", iconIds[i]);
        }
    }
}

void cleanupAnimationIcons() {
    for (int i = 0; i < ANIMATION_FRAMES; i++) {
        if (animationIcons[i] != NULL) {
            DestroyIcon(animationIcons[i]);
        }
    }
}

void animateIconDayToNight() {
    for (int i = ANIMATION_FRAMES - 1; i >= 0; i--) {
        notifData.hIcon = animationIcons[i];
        Shell_NotifyIcon(NIM_MODIFY, &notifData);
        Sleep(10);
    }
}

void animateIconNightToDay() {
    for (int i = 0; i < ANIMATION_FRAMES; i++) {
        notifData.hIcon = animationIcons[i];
        Shell_NotifyIcon(NIM_MODIFY, &notifData);
        Sleep(10);
    }
}


// ### 


int setBackgroundState(int *backgroundStatePtr, int *fromTimePtr, int *toTimePtr) {
    SYSTEMTIME time;
    GetLocalTime(&time);
    int hour = time.wHour;
    if (*fromTimePtr < 24 && *toTimePtr < 24 && *fromTimePtr < *toTimePtr) {
        if (hour >= *fromTimePtr && hour < *toTimePtr) {
            *backgroundStatePtr = DAY;
        } else {
            *backgroundStatePtr = NIGHT;
        }
        updateBackgroundStateConfig();
    } else {
        error("Invalid time");
        return 1;
    }
    return 0;
}

int makeAbsolutePath(char *relativePath, char *absolutePath) {
    if (!GetFullPathNameA(relativePath, MAX_PATH, absolutePath, NULL)) {
        error("Failiure converting to absolute path: %ld", GetLastError());
        return 1;
    }
    return 0;
}

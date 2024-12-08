#include <stdio.h>
#include <windows.h>
#include "background.h"
#include "ini.h"
#include "log.h"

// Global variables
NOTIFYICONDATA notifData;
HINSTANCE hInstance;
HWND hiddenWindow;

// Constants
#define CONFIG_PATH "./config.ini"
#define MAX_VALUE_LENGTH 128

int backgroundState = 0;
int sleepTime = 0;

char nightPath[MAX_VALUE_LENGTH];
char dayPath[MAX_VALUE_LENGTH];
int fromTime, toTime;

// Function declarations
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hiddenWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
// int initConfig(char *nightPath, char *dayPath, int *fromTime, int *toTime);
// int readConfig(char *configPath, const char *section, const char *key, char *value);
int makeAbsoluteChar(char *relativePath, char *absolutePath);

// Message handler for the window
LRESULT CALLBACK WindowProc(HWND hiddenWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &notifData); // Remove tray icon
            PostQuitMessage(0);
            return 0;

        case WM_USER + 1: // Custom message for tray icon
            if (lParam == WM_RBUTTONDOWN) {
                // Create a context menu
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, 1, TEXT("Exit"));

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hiddenWindow); // Required for TrackPopupMenu to work correctly
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hiddenWindow, NULL);
                DestroyMenu(hMenu);
            } else if (lParam == WM_LBUTTONDOWN) {
                // Handle left-click (e.g., show a message box)
                initConfig(CONFIG_PATH, nightPath, dayPath, &fromTime, &toTime);
                changeBackground(&nightPath, &dayPath, &backgroundState, &fromTime, &toTime);
                debug("changeBackground");
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Exit menu item ID
                PostQuitMessage(0); // Exit application
            }
            
            return 0;

    }
    return DefWindowProc(hiddenWindow, uMsg, wParam, lParam);
}

int initConfig(char *configPath, char *nightPath, char *dayPath, int *fromTime, int *toTime) {

    const char *pathSection = "Path";
    const char *pathKeys[] = {"NIGHT", "DAY"};

    const char *timeSection = "Time";
    const char *timeKeys[] = {"FROM", "TO"};

    // Read paths (NIGHT, DAY)
    for (int i = 0; i < sizeof(pathKeys) / sizeof(pathKeys[0]); i++) {
        char value[MAX_VALUE_LENGTH];
        if (readIniValue(configPath, pathSection, pathKeys[i], value) != 0) {
            error("Failure reading path");
            return 1;
        }
        if (i == 0) {
            strcpy(nightPath, value);
            debug("nightPath: %s", nightPath);
        } else {
            strcpy(dayPath, value);
            debug("dayPath: %s", dayPath);
        }
    }

    // Read times (FROM, TO)
    for (int i = 0; i < sizeof(timeKeys) / sizeof(timeKeys[0]); i++) {
        char value[MAX_VALUE_LENGTH];
        if (readIniValue(configPath, timeSection, timeKeys[i], value) != 0) {
            error("Failure reading time");
            return 1;
        }
        if (i == 0) {
            *fromTime = atoi(value);  // Convert value to integer
            debug("fromTime: %d", *fromTime);
        } else {
            *toTime = atoi(value);    // Convert value to integer
            debug("toTime: %d", *toTime);
        }
    }

    return 0;
}

int makeAbsoluteChar(char *relativePath, char *absolutePath) {
    if (!GetFullPathNameA(relativePath, MAX_PATH, absolutePath, NULL)) {
        error("Failiure converting to absolute path: %ld", GetLastError());
        return 1;
    }
    return 0;
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    hInstance = hInst;

    int *backgroundStatePtr = &backgroundState;


    char configPath[MAX_PATH] = CONFIG_PATH;  // Corrected configPath initialization

    if (makeAbsoluteChar(configPath, configPath) != 0) {
        error("Failure converting config path to absolute");
        return 1;
    }

    // if (initConfig(configPath, nightPath, dayPath, &fromTime, &toTime) != 0) {
    //     printf("Failed to initialize config.\n");
    //     return 1;
    // } else {
    //     info("Configuration read successfully");
    // }


    // Register window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = TEXT("TrayApp");
    RegisterClass(&wc);

    // Create a hidden window
    hiddenWindow = CreateWindow(wc.lpszClassName, TEXT("TrayApp"), 0, 0, 0, 0, 0, NULL, NULL, hInst, NULL);

    // Add icon to the system tray
    ZeroMemory(&notifData, sizeof(NOTIFYICONDATA));
    notifData.cbSize = sizeof(NOTIFYICONDATA);
    notifData.hWnd = hiddenWindow;
    notifData.uID = 1;
    notifData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notifData.uCallbackMessage = WM_USER + 1; // Custom message for tray icon events
    notifData.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpy(notifData.szTip, TEXT("Background"));
    Shell_NotifyIcon(NIM_ADD, &notifData);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    Shell_NotifyIcon(NIM_DELETE, &notifData);
    return 0;
}
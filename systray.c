#include <stdio.h>
#include <windows.h>
#include "background.h"
#include <stdbool.h>
#include <shlwapi.h>
#include "ini.h"
#include "log.h"

// Global variables
NOTIFYICONDATA notifData;
HINSTANCE hInstance;
HWND hiddenWindow;

// Constants
#define CONFIG_PATH "./config.ini"
#define MAX_VALUE_LENGTH 128

volatile bool stopThread = false; // Signal to stop the thread

int backgroundState = 0;
int sleepTime = 30;

char nightPath[MAX_VALUE_LENGTH];
char dayPath[MAX_VALUE_LENGTH];
int fromTime, toTime;

// Function declarations
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hiddenWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
// int initConfig(char *nightPath, char *dayPath, int *fromTime, int *toTime);
// int readConfig(char *configPath, const char *section, const char *key, char *value);
int makeAbsolutePath(char *relativePath, char *absolutePath);

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
                // AppendMenu(hMenu, MF_STRING, 3, TEXT("Config"));
                // AppendMenu(hMenu, MF_POPUP, 2, TEXT("Images"));
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, 1, TEXT("Exit"));

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hiddenWindow); // Required for TrackPopupMenu to work correctly
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hiddenWindow, NULL);
                DestroyMenu(hMenu);
            } else if (lParam == WM_LBUTTONDOWN) {
                // Handle left-click (e.g., show a message box)
                // if (initConfig(CONFIG_PATH, nightPath, dayPath, &fromTime, &toTime) != 0) {
                //     error("Failure initializing config");
                //     return 1;
                // }
                // changeBackground(&nightPath, &dayPath, &backgroundState, &fromTime, &toTime);
                // debug("changeBackground");
            }
            return 0;

        case WM_COMMAND:
            stopThread = true;
            if (LOWORD(wParam) == 1) { // Exit menu item ID
                stopThread = true;       // Signal the thread to stop
                PostQuitMessage(0);      // Exit the message loop
            // } else if (LOWORD(wParam) == 2) {
            //     char absolutePath[MAX_PATH];
            //     readIniValue(CONFIG_PATH, "Path", "NIGHT", nightPath);
            //     makeAbsolutePath(nightPath, absolutePath);
            //     char parentDir[MAX_PATH];
            //     strcpy_s(parentDir, sizeof(parentDir), absolutePath);
            //     PathRemoveFileSpec(parentDir);
            //     ShellExecute(NULL, "open", parentDir, NULL, NULL, SW_SHOWNORMAL);
            // } else if (LOWORD(wParam) == 3) {
            //     char absolutePath[MAX_PATH];
            //     makeAbsolutePath(CONFIG_PATH, absolutePath);
            //     char parentDir[MAX_PATH];
            //     strcpy_s(parentDir, sizeof(parentDir), absolutePath);
            //     PathRemoveFileSpec(parentDir);
            //     ShellExecute(NULL, "open", parentDir, NULL, NULL, SW_SHOWNORMAL);
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

int makeAbsolutePath(char *relativePath, char *absolutePath) {
    if (!GetFullPathNameA(relativePath, MAX_PATH, absolutePath, NULL)) {
        error("Failiure converting to absolute path: %ld", GetLastError());
        return 1;
    }
    return 0;
}

int programLoop() {
    if (initConfig(CONFIG_PATH, nightPath, dayPath, &fromTime, &toTime) != 0) {
        error("Failure initializing config");
        return 1;
    }
    changeBackground(&nightPath, &dayPath, &backgroundState, &fromTime, &toTime);
    debug("changeBackground");
    // Sleep(sleepTime * 1000 * 60);
    Sleep(1000);
    return 0;
}

int checkIfConfig(char *configPath) {
    FILE *file = fopen(configPath, "r");
    if (file == NULL) {
        if(createConfig(configPath) != 0) {
            error("Failed to create config file");
            return 1;
        } else {
            checkIfConfig(configPath);
        }
    }
    fclose(file);
    return 0;

}

int createConfig(char *configPath) {
    FILE *file = fopen(configPath, "w");
    if (file == NULL) {
        error("Failed to create config file");
        return 1;
    }
    fclose(file);
    writeIniValue(configPath, "Path", "NIGHT", ".img\night.jpg");
    writeIniValue(configPath, "Path", "DAY", ".img\day.jpg");
    writeIniValue(configPath, "Time", "FROM", "0");
    writeIniValue(configPath, "Time", "TO", "24");
    MessageBox(NULL, TEXT("Please fill out your created config file!"), TEXT("Info"), MB_OK | MB_ICONINFORMATION);
    return 0;
}

DWORD WINAPI ProgramLoopThread(LPVOID lpParam) {
    while (!stopThread) { // Continue running until stopThread is set to true
        if (programLoop() != 0) {
            error("Program loop encountered an error");
            break; // Exit the loop on error
        }
        Sleep(10); // Add a small delay to avoid busy-waiting
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    hInstance = hInst;

    // setLogLevel("INFO");
    checkIfConfig(CONFIG_PATH);

    int *backgroundStatePtr = &backgroundState;


    char configPath[MAX_PATH] = CONFIG_PATH;

    if (initConfig(CONFIG_PATH, nightPath, dayPath, &fromTime, &toTime) != 0) {
        error("Failure initializing config");
        return 1;
    }

    if (makeAbsolutePath(configPath, configPath) != 0) {
        error("Failure converting config path to absolute");
        return 1;
    }

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

    // Create a thread for the program loop
    HANDLE hThread = CreateThread(NULL, 0, ProgramLoopThread, NULL, 0, NULL);
    if (hThread == NULL) {
        error("Failed to create thread for program loop");
        return 1;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Signal the thread to stop and wait for it to finish
    stopThread = true;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    // Cleanup
    Shell_NotifyIcon(NIM_DELETE, &notifData);
    return 0;

}
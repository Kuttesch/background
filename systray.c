#include <stdio.h>
#include <windows.h>
#include "background.h"
// #include "globals.h"

// Global variables
NOTIFYICONDATA notifData;
HINSTANCE hInstance;
HWND hiddenWindow;

int backgroundState = 0;
int sleepTime = 0;

int fromTime = 6;
int toTime = 18;

char *nightPath = "./img/night.jpg";
char *dayPath = "./img/day.jpg";


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
                AppendMenu(hMenu, MF_STRING, 1, TEXT("Exit"));
                AppendMenu(hMenu, MF_STRING, 2, TEXT("Hallo"));

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hiddenWindow); // Required for TrackPopupMenu to work correctly
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hiddenWindow, NULL);
                DestroyMenu(hMenu);
            } else if (lParam == WM_LBUTTONDOWN) {
                // Handle left-click (e.g., show a message box)
                TCHAR buffer[256];
                MessageBox(hiddenWindow, buffer, TEXT("Info"), MB_OK | MB_ICONINFORMATION);
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Exit menu item ID
                PostQuitMessage(0); // Exit application
            } else if(LOWORD(wParam) == 2) {
                printf("changeBackground");
                changeBackground(nightPath, dayPath, &backgroundState, &fromTime, &toTime);
            }
            
            return 0;

    }
    return DefWindowProc(hiddenWindow, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    hInstance = hInst;

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

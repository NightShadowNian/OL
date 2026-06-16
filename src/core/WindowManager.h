#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <windows.h>
#include <vector>
#include <string>

struct WindowInfo {
    HWND hwnd;
    DWORD processId;
    std::wstring title;
    std::wstring processName;
    bool isMainWindow;
    bool isMerged;
};

class WindowManager {
public:
    static std::vector<WindowInfo> GetAllMainWindows();
    static bool IsMainWindow(HWND hwnd);
    static std::wstring GetWindowTitle(HWND hwnd);
    static std::wstring GetProcessName(DWORD processId);
    static void HideFromTaskbar(HWND hwnd);
    static void ShowInTaskbar(HWND hwnd);
    static void SetParentWindow(HWND child, HWND parent);
};

#endif
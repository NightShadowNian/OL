#include "WindowManager.h"
#include <psapi.h>

std::vector<WindowInfo> WindowManager::GetAllMainWindows() {
    std::vector<WindowInfo> windows;
    
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        std::vector<WindowInfo>* pWindows = reinterpret_cast<std::vector<WindowInfo>*>(lParam);
        
        if (!IsWindowVisible(hwnd)) return TRUE;
        if (GetParent(hwnd) != NULL) return TRUE;
        
        LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
        
        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);
        
        WindowInfo info;
        info.hwnd = hwnd;
        info.processId = processId;
        info.title = GetWindowTitle(hwnd);
        info.processName = GetProcessName(processId);
        info.isMainWindow = IsMainWindow(hwnd);
        info.isMerged = false;
        
        pWindows->push_back(info);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    
    return windows;
}

bool WindowManager::IsMainWindow(HWND hwnd) {
    LONG style = GetWindowLongPtr(hwnd, GWL_STYLE);
    return (style & WS_OVERLAPPEDWINDOW) != 0;
}

std::wstring WindowManager::GetWindowTitle(HWND hwnd) {
    int length = GetWindowTextLengthW(hwnd);
    if (length == 0) return L"";
    
    std::wstring title(length + 1, L'\0');
    GetWindowTextW(hwnd, &title[0], length + 1);
    return title;
}

std::wstring WindowManager::GetProcessName(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) return L"";
    
    wchar_t buffer[MAX_PATH];
    GetModuleBaseNameW(hProcess, NULL, buffer, MAX_PATH);
    
    CloseHandle(hProcess);
    return std::wstring(buffer);
}

void WindowManager::HideFromTaskbar(HWND hwnd) {
    LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
}

void WindowManager::ShowInTaskbar(HWND hwnd) {
    LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
}

void WindowManager::SetParentWindow(HWND child, HWND parent) {
    SetParent(child, parent);
}
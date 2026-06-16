#include <windows.h>
#include "core/Settings.h"
#include "core/WindowManager.h"
#include "core/AutoMerge.h"
#include "core/ShortcutManager.h"
#include "ui/MainWindow.h"
#include "ui/TrayIcon.h"

MainWindow* g_mainWindow = nullptr;
TrayIcon* g_trayIcon = nullptr;

LRESULT CALLBACK MessageHandler(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSG* pMsg = reinterpret_cast<MSG*>(lParam);
        ShortcutManager::ProcessMessage(*pMsg);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CoInitialize(NULL);
    
    AppSettings settings = Settings::load();
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WindowMergerMain";
    RegisterClassW(&wc);
    
    g_mainWindow = new MainWindow();
    g_trayIcon = new TrayIcon(g_mainWindow->GetHWND());
    
    ShortcutManager::RegisterHotkey("toggle", settings.hotkeys["toggle"].key, []() {
        if (g_mainWindow) g_mainWindow->Toggle();
    });
    
    ShortcutManager::RegisterHotkey("next_tab", settings.hotkeys["next_tab"].key, []() {
        if (g_mainWindow) g_mainWindow->SwitchToTab(0);
    });
    
    AutoMerge::LoadRules(settings.autoMergeRules);
    AutoMerge::SetOnMatchCallback([](HWND hwnd) {
        if (g_mainWindow) {
            g_mainWindow->AddWindow(hwnd);
        }
    });
    AutoMerge::StartMonitoring();
    
    HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, MessageHandler, NULL, GetCurrentThreadId());
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(hook);
    AutoMerge::StopMonitoring();
    ShortcutManager::UnregisterAll();
    
    delete g_trayIcon;
    delete g_mainWindow;
    
    CoUninitialize();
    
    return (int)msg.wParam;
}
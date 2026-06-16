#include <windows.h>
#include "core/Settings.h"
#include "core/WindowManager.h"
#include "core/AutoMerge.h"
#include "core/ShortcutManager.h"
#include "ui/MainWindow.h"
#include "ui/TrayIcon.h"

MainWindow* g_mainWindow = nullptr;
TrayIcon* g_trayIcon = nullptr;
AppSettings g_settings;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CoInitialize(NULL);
    
    g_settings = Settings::load();
    
    g_mainWindow = new MainWindow();
    g_trayIcon = new TrayIcon(g_mainWindow->GetHWND());
    
    ShortcutManager::RegisterHotkey("toggle", g_settings.hotkeys["toggle"].key, []() {
        if (g_mainWindow) g_mainWindow->Toggle();
    });
    
    ShortcutManager::RegisterHotkey("next_tab", g_settings.hotkeys["next_tab"].key, []() {
        if (g_mainWindow) g_mainWindow->NextTab();
    });
    
    ShortcutManager::RegisterHotkey("prev_tab", g_settings.hotkeys["prev_tab"].key, []() {
        if (g_mainWindow) g_mainWindow->PrevTab();
    });
    
    AutoMerge::LoadRules(g_settings.autoMergeRules);
    AutoMerge::SetOnMatchCallback([](HWND hwnd) {
        if (g_mainWindow) {
            g_mainWindow->AddWindow(hwnd);
        }
    });
    AutoMerge::StartMonitoring();
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            ShortcutManager::ProcessMessage(msg);
            continue;
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    AutoMerge::StopMonitoring();
    ShortcutManager::UnregisterAll();
    
    delete g_trayIcon;
    delete g_mainWindow;
    
    CoUninitialize();
    
    return (int)msg.wParam;
}
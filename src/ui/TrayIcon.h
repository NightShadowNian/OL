#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>
#include <string>

class TrayIcon {
public:
    TrayIcon(HWND parent);
    ~TrayIcon();
    
    void Show();
    void Hide();
    void SetTooltip(const std::wstring& tooltip);
    void ShowMenu();
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    HWND m_hwnd;
    NOTIFYICONDATAW m_nid;
};

#endif
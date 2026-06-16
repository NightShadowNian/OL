#include "TrayIcon.h"

TrayIcon::TrayIcon(HWND parent) : m_hwnd(NULL) {
    m_hwnd = CreateWindowEx(0, L"STATIC", L"TrayIconWindow",
        WS_POPUP, 0, 0, 0, 0, parent, NULL, NULL, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    
    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(m_nid);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_USER + 1;
    
    HICON hIcon = LoadIcon(NULL, IDI_APPLICATION);
    m_nid.hIcon = hIcon;
    
    wcscpy_s(m_nid.szTip, L"WindowMerger");
    
    Shell_NotifyIcon(NIM_ADD, &m_nid);
}

TrayIcon::~TrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &m_nid);
    if (m_nid.hIcon) DestroyIcon(m_nid.hIcon);
}

void TrayIcon::Show() {
    Shell_NotifyIcon(NIM_ADD, &m_nid);
}

void TrayIcon::Hide() {
    Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

void TrayIcon::SetTooltip(const std::wstring& tooltip) {
    wcscpy_s(m_nid.szTip, tooltip.c_str());
    Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}

LRESULT CALLBACK TrayIcon::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TrayIcon* pThis = reinterpret_cast<TrayIcon*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    if (msg == WM_USER + 1 && lParam == WM_RBUTTONUP) {
        if (pThis) pThis->ShowMenu();
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void TrayIcon::ShowMenu() {
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, 1, L"显示窗口");
    AppendMenu(hMenu, MF_STRING, 2, L"隐藏窗口");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, 3, L"设置");
    AppendMenu(hMenu, MF_STRING, 4, L"退出");
    
    SetForegroundWindow(m_hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, m_hwnd, NULL);
    DestroyMenu(hMenu);
}
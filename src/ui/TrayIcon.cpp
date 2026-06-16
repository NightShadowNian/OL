#include "TrayIcon.h"

static LRESULT CALLBACK TrayIconWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TrayIcon* pThis = (TrayIcon*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if (msg == WM_USER + 1) {
        if (lParam == WM_RBUTTONUP && pThis) {
            pThis->ShowMenu();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

TrayIcon::TrayIcon(HWND parent) : m_hwnd(NULL) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = TrayIconWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"WindowMergerTrayIcon";
    RegisterClassW(&wc);
    
    m_hwnd = CreateWindowExW(0, L"WindowMergerTrayIcon", L"TrayIconWindow",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    
    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_USER + 1;
    
    HICON hIcon = LoadIcon(NULL, IDI_APPLICATION);
    m_nid.hIcon = hIcon;
    
    wcscpy_s(m_nid.szTip, L"WindowMerger");
    
    Shell_NotifyIconW(NIM_ADD, &m_nid);
}

TrayIcon::~TrayIcon() {
    Shell_NotifyIconW(NIM_DELETE, &m_nid);
    if (m_nid.hIcon) DestroyIcon(m_nid.hIcon);
    DestroyWindow(m_hwnd);
}

void TrayIcon::Show() {
    Shell_NotifyIconW(NIM_ADD, &m_nid);
}

void TrayIcon::Hide() {
    Shell_NotifyIconW(NIM_DELETE, &m_nid);
}

void TrayIcon::SetTooltip(const std::wstring& tooltip) {
    wcscpy_s(m_nid.szTip, tooltip.c_str());
    Shell_NotifyIconW(NIM_MODIFY, &m_nid);
}

void TrayIcon::ShowMenu() {
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, 1, L"Show Window");
    AppendMenuW(hMenu, MF_STRING, 2, L"Hide Window");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, 3, L"Settings");
    AppendMenuW(hMenu, MF_STRING, 4, L"Exit");
    
    SetForegroundWindow(m_hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, m_hwnd, NULL);
    DestroyMenu(hMenu);
}
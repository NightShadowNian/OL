#include "WindowListPanel.h"
#include "../core/WindowManager.h"

extern void MergeWindow(HWND hwnd);

WindowListPanel::WindowListPanel(HWND parent) : m_hwnd(NULL) {
    m_hwnd = CreateWindowExW(0, L"STATIC", L"", 
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        0, 0, 0, 0, parent, NULL, NULL, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    
    Refresh();
}

WindowListPanel::~WindowListPanel() {
}

void WindowListPanel::Refresh() {
    m_windows.clear();
    std::vector<WindowInfo> windows = WindowManager::GetAllMainWindows();
    
    for (const auto& win : windows) {
        WindowItem item;
        item.hwnd = win.hwnd;
        item.title = win.title;
        item.processName = win.processName;
        m_windows.push_back(item);
    }
    
    Draw();
}

void WindowListPanel::Draw() {
    InvalidateRect(m_hwnd, NULL, TRUE);
    UpdateWindow(m_hwnd);
}

void WindowListPanel::OnMergeClicked(int index) {
    if (index >= 0 && index < (int)m_windows.size()) {
        MergeWindow(m_windows[index].hwnd);
    }
}

LRESULT CALLBACK WindowListPanel::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowListPanel* pThis = (WindowListPanel*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));
            
            HFONT hFont = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
            
            int y = 20;
            wchar_t buffer[512];
            
            for (size_t i = 0; i < pThis->m_windows.size() && y < rect.bottom - 40; i++) {
                swprintf_s(buffer, L"[%d] %s - %s", (int)i + 1, 
                    pThis->m_windows[i].title.c_str(), 
                    pThis->m_windows[i].processName.c_str());
                
                SetBkMode(hdc, TRANSPARENT);
                TextOutW(hdc, 20, y, buffer, wcslen(buffer));
                y += 30;
            }
            
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            int index = (y - 20) / 30;
            if (index >= 0 && index < (int)pThis->m_windows.size()) {
                pThis->OnMergeClicked(index);
            }
            return 0;
        }
        
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
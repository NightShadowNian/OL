#include "TabBar.h"

TabBar::TabBar(HWND parent) : m_currentIndex(-1) {
    m_hwnd = CreateWindowEx(0, L"STATIC", L"", 
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        0, 0, 0, 30, parent, NULL, NULL, NULL);
}

TabBar::~TabBar() {
    for (auto& tab : m_tabs) {
        if (tab.icon) DestroyIcon(tab.icon);
    }
}

void TabBar::AddTab(HWND targetHwnd, const std::wstring& title) {
    TabItem item;
    item.id = m_tabs.size();
    item.targetHwnd = targetHwnd;
    item.title = title;
    item.icon = NULL;
    item.isActive = (m_tabs.empty());
    
    m_tabs.push_back(item);
    m_currentIndex = m_tabs.size() - 1;
    
    DrawTabs();
}

void TabBar::RemoveTab(int index) {
    if (index < 0 || index >= (int)m_tabs.size()) return;
    
    if (m_tabs[index].icon) DestroyIcon(m_tabs[index].icon);
    m_tabs.erase(m_tabs.begin() + index);
    
    if (m_currentIndex >= (int)m_tabs.size()) {
        m_currentIndex = std::max(0, (int)m_tabs.size() - 1);
    }
    if (m_currentIndex >= 0) {
        m_tabs[m_currentIndex].isActive = true;
    }
    
    DrawTabs();
}

void TabBar::SwitchTab(int index) {
    if (index < 0 || index >= (int)m_tabs.size()) return;
    
    if (m_currentIndex >= 0 && m_currentIndex < (int)m_tabs.size()) {
        m_tabs[m_currentIndex].isActive = false;
    }
    
    m_currentIndex = index;
    m_tabs[index].isActive = true;
    
    DrawTabs();
}

void TabBar::CloseTab(int index) {
    RemoveTab(index);
}

void TabBar::UpdateTabTitle(int index, const std::wstring& title) {
    if (index >= 0 && index < (int)m_tabs.size()) {
        m_tabs[index].title = title;
        DrawTabs();
    }
}

HWND TabBar::GetTabHwnd(int index) const {
    if (index >= 0 && index < (int)m_tabs.size()) {
        return m_tabs[index].targetHwnd;
    }
    return NULL;
}

LRESULT CALLBACK TabBar::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TabBar* pThis = reinterpret_cast<TabBar*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_LBUTTONDOWN:
            if (pThis) pThis->OnMouseDown(wParam, lParam);
            return 0;
        case WM_MOUSEWHEEL:
            if (pThis) pThis->OnMouseWheel(wParam, lParam);
            return 0;
        case WM_PAINT:
            if (pThis) pThis->DrawTabs();
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void TabBar::DrawTabs() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);
    
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    
    HBRUSH activeBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH inactiveBrush = CreateSolidBrush(RGB(200, 200, 200));
    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    
    int x = 0;
    for (size_t i = 0; i < m_tabs.size(); i++) {
        SIZE size;
        GetTextExtentPoint32(hdc, m_tabs[i].title.c_str(), m_tabs[i].title.length(), &size);
        
        int tabWidth = size.cx + 40;
        int tabHeight = rect.bottom - rect.top;
        
        RECT tabRect = {x, 0, x + tabWidth, tabHeight};
        
        HBRUSH brush = m_tabs[i].isActive ? activeBrush : inactiveBrush;
        FillRect(hdc, &tabRect, brush);
        
        FrameRect(hdc, &tabRect, borderPen);
        
        if (!m_tabs[i].isActive) {
            RECT bottomLine = {x, tabHeight - 1, x + tabWidth, tabHeight};
            FillRect(hdc, &bottomLine, borderPen);
        }
        
        SetTextColor(hdc, m_tabs[i].isActive ? RGB(0, 0, 0) : RGB(50, 50, 50));
        SetBkMode(hdc, TRANSPARENT);
        
        RECT textRect = {x + 10, 5, x + tabWidth - 20, tabHeight - 5};
        DrawText(hdc, m_tabs[i].title.c_str(), m_tabs[i].title.length(), &textRect, DT_CENTER | DT_VCENTER);
        
        x += tabWidth;
    }
    
    DeleteObject(activeBrush);
    DeleteObject(inactiveBrush);
    DeleteObject(borderPen);
    
    EndPaint(m_hwnd, &ps);
}

void TabBar::OnMouseDown(WPARAM wParam, LPARAM lParam) {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    
    HDC hdc = GetDC(m_hwnd);
    int currentX = 0;
    
    for (size_t i = 0; i < m_tabs.size(); i++) {
        SIZE size;
        GetTextExtentPoint32(hdc, m_tabs[i].title.c_str(), m_tabs[i].title.length(), &size);
        int tabWidth = size.cx + 40;
        
        if (x >= currentX && x < currentX + tabWidth) {
            int closeX = currentX + tabWidth - 20;
            if (x >= closeX && x < closeX + 15) {
                CloseTab(i);
            } else {
                SwitchTab(i);
            }
            break;
        }
        currentX += tabWidth;
    }
    
    ReleaseDC(m_hwnd, hdc);
}

void TabBar::OnMouseWheel(WPARAM wParam, LPARAM lParam) {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    
    if (delta > 0 && m_currentIndex > 0) {
        SwitchTab(m_currentIndex - 1);
    } else if (delta < 0 && m_currentIndex < (int)m_tabs.size() - 1) {
        SwitchTab(m_currentIndex + 1);
    }
}
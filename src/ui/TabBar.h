#ifndef TABBAR_H
#define TABBAR_H

#include <windows.h>
#include <vector>
#include <string>

struct TabItem {
    int id;
    HWND targetHwnd;
    std::wstring title;
    HICON icon;
    bool isActive;
};

class TabBar {
public:
    TabBar(HWND parent);
    ~TabBar();
    
    HWND GetHWND() const { return m_hwnd; }
    void AddTab(HWND targetHwnd, const std::wstring& title);
    void RemoveTab(int index);
    void SwitchTab(int index);
    void CloseTab(int index);
    void UpdateTabTitle(int index, const std::wstring& title);
    int GetCurrentTabIndex() const { return m_currentIndex; }
    int GetTabCount() const { return m_tabs.size(); }
    HWND GetTabHwnd(int index) const;
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void DrawTabs();
    void OnMouseDown(WPARAM wParam, LPARAM lParam);
    void OnMouseWheel(WPARAM wParam, LPARAM lParam);
    
    HWND m_hwnd;
    std::vector<TabItem> m_tabs;
    int m_currentIndex;
};

#endif
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <vector>

class TabBar;
class WindowListPanel;

class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    
    HWND GetHWND() const { return m_hwnd; }
    void Show();
    void Hide();
    void Toggle();
    void AddWindow(HWND hwnd);
    void RemoveWindow(HWND hwnd);
    void SwitchToTab(int index);
    void NextTab();
    void PrevTab();
    TabBar* GetTabBar() const { return m_tabBar; }
    WindowListPanel* GetWindowListPanel() const { return m_windowListPanel; }
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void OnSize(UINT width, UINT height);
    void AdjustSizeToContent();
    
    HWND m_hwnd;
    HWND m_contentArea;
    HWND m_activeWindow;
    TabBar* m_tabBar;
    WindowListPanel* m_windowListPanel;
};

#endif
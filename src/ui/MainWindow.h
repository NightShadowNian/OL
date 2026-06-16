#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>

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
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void OnSize(UINT width, UINT height);
    
    HWND m_hwnd;
    HWND m_contentArea;
    HWND m_activeWindow;
};

#endif
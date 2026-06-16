#ifndef WINDOWLISTPANEL_H
#define WINDOWLISTPANEL_H

#include <windows.h>
#include <string>
#include <vector>

struct WindowItem {
    HWND hwnd;
    std::wstring title;
    std::wstring processName;
};

class WindowListPanel {
public:
    WindowListPanel(HWND parent);
    ~WindowListPanel();
    
    HWND GetHWND() const { return m_hwnd; }
    void Refresh();
    void OnMergeClicked(int index);
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void Draw();
    
    HWND m_hwnd;
    std::vector<WindowItem> m_windows;
};

#endif
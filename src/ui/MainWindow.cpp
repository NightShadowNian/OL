#include "MainWindow.h"
#include "TabBar.h"
#include "WindowListPanel.h"
#include "../core/WindowManager.h"

static LRESULT CALLBACK ContentWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void MergeWindow(HWND hwnd);

MainWindow::MainWindow() : m_contentArea(nullptr), m_activeWindow(nullptr), 
    m_tabBar(nullptr), m_windowListPanel(nullptr) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = MainWindow::WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"WindowMergerMain";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClassW(&wc);
    
    WNDCLASSW contentwc = {0};
    contentwc.lpfnWndProc = ContentWndProc;
    contentwc.hInstance = hInst;
    contentwc.lpszClassName = L"WindowMergerContent";
    contentwc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClassW(&contentwc);
    
    m_hwnd = CreateWindowExW(0, L"WindowMergerMain", L"WindowMerger",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInst, this);
}

MainWindow::~MainWindow() {
    if (m_tabBar) delete m_tabBar;
    if (m_windowListPanel) delete m_windowListPanel;
}

void MainWindow::Show() {
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
}

void MainWindow::Hide() {
    ShowWindow(m_hwnd, SW_HIDE);
}

void MainWindow::Toggle() {
    if (IsWindowVisible(m_hwnd)) {
        Hide();
    } else {
        Show();
    }
}

void MainWindow::AddWindow(HWND hwnd) {
    if (!m_tabBar) return;
    
    std::wstring title = WindowManager::GetWindowTitle(hwnd);
    m_tabBar->AddTab(hwnd, title);
    
    WindowManager::HideFromTaskbar(hwnd);
    WindowManager::SetParentWindow(hwnd, m_contentArea);
    
    RECT rect;
    GetClientRect(m_contentArea, &rect);
    SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    ShowWindow(hwnd, SW_SHOW);
    
    m_activeWindow = hwnd;
    AdjustSizeToContent();
}

void MainWindow::RemoveWindow(HWND hwnd) {
    WindowManager::ShowInTaskbar(hwnd);
    WindowManager::SetParentWindow(hwnd, NULL);
    m_activeWindow = NULL;
    AdjustSizeToContent();
}

void MainWindow::SwitchToTab(int index) {
    if (!m_tabBar) return;
    m_tabBar->SwitchTab(index);
    m_activeWindow = m_tabBar->GetTabHwnd(index);
    
    if (m_activeWindow) {
        RECT rect;
        GetClientRect(m_contentArea, &rect);
        SetWindowPos(m_activeWindow, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
        ShowWindow(m_activeWindow, SW_SHOW);
    }
}

void MainWindow::NextTab() {
    if (!m_tabBar) return;
    int count = m_tabBar->GetTabCount();
    if (count <= 1) return;
    int current = m_tabBar->GetCurrentTabIndex();
    int next = (current + 1) % count;
    SwitchToTab(next);
}

void MainWindow::PrevTab() {
    if (!m_tabBar) return;
    int count = m_tabBar->GetTabCount();
    if (count <= 1) return;
    int current = m_tabBar->GetCurrentTabIndex();
    int prev = (current - 1 + count) % count;
    SwitchToTab(prev);
}

void MainWindow::AdjustSizeToContent() {
    if (m_activeWindow) {
        RECT rect;
        GetWindowRect(m_activeWindow, &rect);
        int width = rect.right - rect.left + 50;
        int height = rect.bottom - rect.top + 100;
        
        RECT workArea;
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
        
        if (width > workArea.right - workArea.left) width = workArea.right - workArea.left;
        if (height > workArea.bottom - workArea.top) height = workArea.bottom - workArea.top;
        
        SetWindowPos(m_hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    }
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;
    
    if (msg == WM_CREATE) {
        CREATESTRUCT* pcs = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pcs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    
    if (!pThis) return DefWindowProcW(hwnd, msg, wParam, lParam);
    
    switch (msg) {
        case WM_CREATE: {
            pThis->m_tabBar = new TabBar(hwnd);
            
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            pThis->m_contentArea = CreateWindowExW(0, L"WindowMergerContent", L"",
                WS_CHILD | WS_VISIBLE,
                0, 35, rc.right - rc.left, rc.bottom - rc.top - 35,
                hwnd, NULL, GetModuleHandle(NULL), NULL);
            
            pThis->m_windowListPanel = new WindowListPanel(pThis->m_contentArea);
            
            SetWindowPos(pThis->m_windowListPanel->GetHWND(), NULL, 0, 0, 
                rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
            
            ShowWindow(pThis->m_windowListPanel->GetHWND(), SW_SHOW);
            return 0;
        }
        
        case WM_SIZE:
            pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void MainWindow::OnSize(UINT width, UINT height) {
    if (m_tabBar) {
        SetWindowPos(m_tabBar->GetHWND(), NULL, 0, 0, width, 35, SWP_NOZORDER);
    }
    if (m_contentArea) {
        SetWindowPos(m_contentArea, NULL, 0, 35, width, height - 35, SWP_NOZORDER);
    }
    if (m_windowListPanel) {
        SetWindowPos(m_windowListPanel->GetHWND(), NULL, 0, 0, width, height - 35, SWP_NOZORDER);
    }
    if (m_activeWindow) {
        RECT rect;
        GetClientRect(m_contentArea, &rect);
        SetWindowPos(m_activeWindow, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    }
}

void MergeWindow(HWND hwnd) {
    extern MainWindow* g_mainWindow;
    if (g_mainWindow) {
        g_mainWindow->AddWindow(hwnd);
    }
}
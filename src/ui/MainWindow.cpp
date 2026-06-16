#include "MainWindow.h"
#include "../core/WindowManager.h"

MainWindow::MainWindow() : m_contentArea(nullptr), m_activeWindow(nullptr) {
    m_hwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"WindowMergerMain",
        L"WindowMerger",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, NULL, NULL
    );
    
    m_contentArea = CreateWindowEx(0, L"STATIC", L"", 
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        0, 30, 0, 0, m_hwnd, NULL, NULL, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

MainWindow::~MainWindow() {
}

void MainWindow::Show() {
    ShowWindow(m_hwnd, SW_SHOW);
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
    std::wstring title = WindowManager::GetWindowTitle(hwnd);
    
    WindowManager::HideFromTaskbar(hwnd);
    WindowManager::SetParentWindow(hwnd, m_contentArea);
    
    RECT rect;
    GetClientRect(m_contentArea, &rect);
    SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    
    ShowWindow(hwnd, SW_SHOW);
    m_activeWindow = hwnd;
}

void MainWindow::RemoveWindow(HWND hwnd) {
    WindowManager::ShowInTaskbar(hwnd);
    WindowManager::SetParentWindow(hwnd, NULL);
    m_activeWindow = NULL;
}

void MainWindow::SwitchToTab(int index) {
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_SIZE:
            if (pThis) pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void MainWindow::OnSize(UINT width, UINT height) {
    if (m_contentArea) {
        SetWindowPos(m_contentArea, NULL, 0, 30, width, height - 30, SWP_NOZORDER);
    }
    if (m_activeWindow) {
        RECT rect;
        GetClientRect(m_contentArea, &rect);
        SetWindowPos(m_activeWindow, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    }
}
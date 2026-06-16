# 窗口合并工具实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 开发一款 Windows 平台的窗口合并工具，支持多窗口标签页管理、自动合并、快捷键控制等功能

**Architecture:** 采用代理窗口模式，使用原生 Win32 API 实现窗口管理，模块化设计确保低资源占用

**Tech Stack:** C++17, Win32 API, JSON for Modern C++, Regex

---

## 项目结构

```
src/
├── core/
│   ├── WindowManager.h/cpp    # 窗口检测与合并逻辑
│   ├── AutoMerge.h/cpp        # 自动合并规则引擎
│   ├── ShortcutManager.h/cpp  # 快捷键管理与冲突检测
│   ├── Settings.h/cpp         # 设置管理
│   └── ConfigStorage.h/cpp    # 配置持久化存储
├── ui/
│   ├── MainWindow.h/cpp       # 主容器窗口
│   ├── TabBar.h/cpp           # 标签栏管理
│   ├── ProxyWindow.h/cpp      # 代理窗口
│   ├── TrayIcon.h/cpp         # 系统托盘
│   └── SettingsDialog.h/cpp   # 设置对话框
├── utils/
│   ├── StringUtils.h/cpp      # 字符串工具
│   └── Logger.h/cpp           # 日志工具
└── main.cpp                   # 入口函数
```

---

## 任务分解

### 任务 1: 项目初始化

**Files:**
- Create: `src/main.cpp`
- Create: `src/core/Settings.h`
- Create: `src/core/Settings.cpp`

- [ ] **Step 1: 创建项目结构**

```bash
mkdir -p src/core src/ui src/utils
```

- [ ] **Step 2: 创建 Settings 头文件**

```cpp
#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <map>

struct Hotkey {
    std::string id;
    std::string key;
    bool enabled;
};

struct AutoMergeRule {
    std::string name;
    std::string pattern;
    bool enabled;
    std::string workspace;
};

struct Workspace {
    std::string name;
    std::vector<std::string> windows;
};

struct AppSettings {
    std::map<std::string, Hotkey> hotkeys;
    std::vector<AutoMergeRule> autoMergeRules;
    std::vector<Workspace> workspaces;
    bool startOnBoot;
    bool showNotifications;
    bool hideTaskbarIcons;
};

class Settings {
public:
    static AppSettings load();
    static void save(const AppSettings& settings);
    static AppSettings getDefault();
};

#endif
```

- [ ] **Step 3: 创建 Settings 实现文件**

```cpp
#include "Settings.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    std::string getConfigPath() {
        char appData[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appData);
        return std::string(appData) + "\\WindowMerger\\config.json";
    }
}

AppSettings Settings::load() {
    std::string path = getConfigPath();
    std::ifstream file(path);
    if (!file.is_open()) {
        return getDefault();
    }
    
    json data;
    file >> data;
    
    AppSettings settings;
    settings.startOnBoot = data.value("startOnBoot", true);
    settings.showNotifications = data.value("showNotifications", true);
    settings.hideTaskbarIcons = data.value("hideTaskbarIcons", true);
    
    if (data.contains("hotkeys")) {
        for (const auto& [id, hotkey] : data["hotkeys"].items()) {
            settings.hotkeys[id] = {
                id,
                hotkey.value("key", ""),
                hotkey.value("enabled", true)
            };
        }
    }
    
    if (data.contains("autoMergeRules")) {
        for (const auto& rule : data["autoMergeRules"]) {
            settings.autoMergeRules.push_back({
                rule.value("name", ""),
                rule.value("pattern", ""),
                rule.value("enabled", true),
                rule.value("workspace", "")
            });
        }
    }
    
    return settings;
}

void Settings::save(const AppSettings& settings) {
    json data;
    data["startOnBoot"] = settings.startOnBoot;
    data["showNotifications"] = settings.showNotifications;
    data["hideTaskbarIcons"] = settings.hideTaskbarIcons;
    
    json hotkeysJson;
    for (const auto& [id, hotkey] : settings.hotkeys) {
        hotkeysJson[id]["key"] = hotkey.key;
        hotkeysJson[id]["enabled"] = hotkey.enabled;
    }
    data["hotkeys"] = hotkeysJson;
    
    json rulesJson;
    for (const auto& rule : settings.autoMergeRules) {
        rulesJson.push_back({
            {"name", rule.name},
            {"pattern", rule.pattern},
            {"enabled", rule.enabled},
            {"workspace", rule.workspace}
        });
    }
    data["autoMergeRules"] = rulesJson;
    
    std::string path = getConfigPath();
    std::ofstream file(path);
    file << data.dump(4);
}

AppSettings Settings::getDefault() {
    AppSettings settings;
    settings.startOnBoot = true;
    settings.showNotifications = true;
    settings.hideTaskbarIcons = true;
    
    settings.hotkeys["toggle"] = {"toggle", "Ctrl+Shift+O", true};
    settings.hotkeys["next_tab"] = {"next_tab", "Ctrl+Tab", true};
    settings.hotkeys["prev_tab"] = {"prev_tab", "Ctrl+Shift+Tab", true};
    settings.hotkeys["close_tab"] = {"close_tab", "Ctrl+W", true};
    
    return settings;
}
```

- [ ] **Step 4: 创建 main.cpp 入口文件**

```cpp
#include <windows.h>
#include "core/Settings.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AppSettings settings = Settings::load();
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}
```

- [ ] **Step 5: 创建 CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.16)
project(WindowMerger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WindowMerger
    src/main.cpp
    src/core/Settings.cpp
)

target_link_libraries(WindowMerger
    user32.lib
    shell32.lib
)

set_target_properties(WindowMerger PROPERTIES
    WIN32_EXECUTABLE TRUE
)
```

- [ ] **Step 6: 构建测试**

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

---

### 任务 2: 窗口管理器实现

**Files:**
- Create: `src/core/WindowManager.h`
- Create: `src/core/WindowManager.cpp`

- [ ] **Step 1: 创建 WindowManager 头文件**

```cpp
#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <windows.h>
#include <vector>
#include <string>

struct WindowInfo {
    HWND hwnd;
    DWORD processId;
    std::wstring title;
    std::wstring processName;
    bool isMainWindow;
    bool isMerged;
};

class WindowManager {
public:
    static std::vector<WindowInfo> GetAllMainWindows();
    static bool IsMainWindow(HWND hwnd);
    static std::wstring GetWindowTitle(HWND hwnd);
    static std::wstring GetProcessName(DWORD processId);
    static void HideFromTaskbar(HWND hwnd);
    static void ShowInTaskbar(HWND hwnd);
    static void SetParentWindow(HWND child, HWND parent);
};

#endif
```

- [ ] **Step 2: 创建 WindowManager 实现文件**

```cpp
#include "WindowManager.h"
#include <psapi.h>
#include <tlhelp32.h>

std::vector<WindowInfo> WindowManager::GetAllMainWindows() {
    std::vector<WindowInfo> windows;
    
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        std::vector<WindowInfo>* pWindows = reinterpret_cast<std::vector<WindowInfo>*>(lParam);
        
        if (!IsWindowVisible(hwnd)) return TRUE;
        if (GetParent(hwnd) != NULL) return TRUE;
        
        LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
        
        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);
        
        WindowInfo info;
        info.hwnd = hwnd;
        info.processId = processId;
        info.title = GetWindowTitle(hwnd);
        info.processName = GetProcessName(processId);
        info.isMainWindow = IsMainWindow(hwnd);
        info.isMerged = false;
        
        pWindows->push_back(info);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    
    return windows;
}

bool WindowManager::IsMainWindow(HWND hwnd) {
    LONG style = GetWindowLongPtr(hwnd, GWL_STYLE);
    return (style & WS_OVERLAPPEDWINDOW) != 0;
}

std::wstring WindowManager::GetWindowTitle(HWND hwnd) {
    int length = GetWindowTextLengthW(hwnd);
    if (length == 0) return L"";
    
    std::wstring title(length + 1, L'\0');
    GetWindowTextW(hwnd, &title[0], length + 1);
    return title;
}

std::wstring WindowManager::GetProcessName(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) return L"";
    
    wchar_t buffer[MAX_PATH];
    GetModuleBaseNameW(hProcess, NULL, buffer, MAX_PATH);
    
    CloseHandle(hProcess);
    return std::wstring(buffer);
}

void WindowManager::HideFromTaskbar(HWND hwnd) {
    ITaskbarList3* pTaskbarList;
    if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, reinterpret_cast<void**>(&pTaskbarList)))) {
        pTaskbarList->DeleteTab(hwnd);
        pTaskbarList->Release();
    }
    
    LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
}

void WindowManager::ShowInTaskbar(HWND hwnd) {
    LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
    
    ITaskbarList3* pTaskbarList;
    if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, reinterpret_cast<void**>(&pTaskbarList)))) {
        pTaskbarList->AddTab(hwnd);
        pTaskbarList->Release();
    }
}

void WindowManager::SetParentWindow(HWND child, HWND parent) {
    SetParent(child, parent);
}
```

- [ ] **Step 3: 更新 CMakeLists.txt 添加 psapi 库**

```cmake
target_link_libraries(WindowMerger
    user32.lib
    shell32.lib
    psapi.lib
    ole32.lib
)
```

---

### 任务 3: 自动合并规则引擎

**Files:**
- Create: `src/core/AutoMerge.h`
- Create: `src/core/AutoMerge.cpp`

- [ ] **Step 1: 创建 AutoMerge 头文件**

```cpp
#ifndef AUTOMERGE_H
#define AUTOMERGE_H

#include <vector>
#include <string>
#include <functional>
#include "Settings.h"

class AutoMerge {
public:
    static void LoadRules(const std::vector<AutoMergeRule>& rules);
    static bool MatchRule(const std::wstring& windowTitle);
    static void SetOnMatchCallback(std::function<void(HWND)> callback);
    static void StartMonitoring();
    static void StopMonitoring();
private:
    static std::vector<AutoMergeRule> m_rules;
    static std::function<void(HWND)> m_callback;
    static HHOOK m_hook;
    static LRESULT CALLBACK WindowProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif
```

- [ ] **Step 2: 创建 AutoMerge 实现文件**

```cpp
#include "AutoMerge.h"
#include <regex>

std::vector<AutoMergeRule> AutoMerge::m_rules;
std::function<void(HWND)> AutoMerge::m_callback;
HHOOK AutoMerge::m_hook = NULL;

void AutoMerge::LoadRules(const std::vector<AutoMergeRule>& rules) {
    m_rules.clear();
    for (const auto& rule : rules) {
        if (rule.enabled) {
            m_rules.push_back(rule);
        }
    }
}

bool AutoMerge::MatchRule(const std::wstring& windowTitle) {
    for (const auto& rule : m_rules) {
        if (!rule.enabled) continue;
        
        try {
            std::wregex pattern(rule.pattern);
            if (std::regex_search(windowTitle, pattern)) {
                return true;
            }
        } catch (...) {
            continue;
        }
    }
    return false;
}

void AutoMerge::SetOnMatchCallback(std::function<void(HWND)> callback) {
    m_callback = callback;
}

LRESULT CALLBACK AutoMerge::WindowProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        CWPSTRUCT* pCWP = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (pCWP->message == WM_CREATE) {
            std::wstring title = WindowManager::GetWindowTitle(pCWP->hwnd);
            if (MatchRule(title)) {
                if (m_callback) {
                    m_callback(pCWP->hwnd);
                }
            }
        }
    }
    return CallNextHookEx(m_hook, nCode, wParam, lParam);
}

void AutoMerge::StartMonitoring() {
    m_hook = SetWindowsHookEx(WH_CALLWNDPROC, WindowProc, NULL, GetCurrentThreadId());
}

void AutoMerge::StopMonitoring() {
    if (m_hook) {
        UnhookWindowsHookEx(m_hook);
        m_hook = NULL;
    }
}
```

---

### 任务 4: 快捷键管理器

**Files:**
- Create: `src/core/ShortcutManager.h`
- Create: `src/core/ShortcutManager.cpp`

- [ ] **Step 1: 创建 ShortcutManager 头文件**

```cpp
#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <map>
#include <string>
#include <functional>

class ShortcutManager {
public:
    static bool RegisterHotkey(const std::string& id, const std::string& key, std::function<void()> callback);
    static bool UnregisterHotkey(const std::string& id);
    static void UnregisterAll();
    static bool IsKeyConflict(const std::string& key);
    static void ProcessMessage(MSG& msg);
private:
    static std::map<std::string, int> m_hotkeyIds;
    static std::map<int, std::function<void()>> m_callbacks;
    static int m_nextId;
    static int ParseKey(const std::string& key);
};

#endif
```

- [ ] **Step 2: 创建 ShortcutManager 实现文件**

```cpp
#include "ShortcutManager.h"
#include <Windows.h>

std::map<std::string, int> ShortcutManager::m_hotkeyIds;
std::map<int, std::function<void()>> ShortcutManager::m_callbacks;
int ShortcutManager::m_nextId = 100;

int ShortcutManager::ParseKey(const std::string& key) {
    int modifiers = 0;
    std::string keyPart = key;
    
    if (keyPart.find("Ctrl+") != std::string::npos) {
        modifiers |= MOD_CONTROL;
        keyPart = keyPart.substr(5);
    }
    if (keyPart.find("Shift+") != std::string::npos) {
        modifiers |= MOD_SHIFT;
        keyPart = keyPart.substr(6);
    }
    if (keyPart.find("Alt+") != std::string::npos) {
        modifiers |= MOD_ALT;
        keyPart = keyPart.substr(4);
    }
    
    if (keyPart.length() == 1) {
        return modifiers | toupper(keyPart[0]);
    }
    
    if (keyPart == "Tab") return modifiers | VK_TAB;
    if (keyPart == "W") return modifiers | 'W';
    if (keyPart == "O") return modifiers | 'O';
    
    return 0;
}

bool ShortcutManager::RegisterHotkey(const std::string& id, const std::string& key, std::function<void()> callback) {
    int keyCode = ParseKey(key);
    if (keyCode == 0) return false;
    
    int modifiers = keyCode & 0xFF00;
    int vkCode = keyCode & 0x00FF;
    
    if (!RegisterHotKey(NULL, m_nextId, modifiers, vkCode)) {
        return false;
    }
    
    m_hotkeyIds[id] = m_nextId;
    m_callbacks[m_nextId] = callback;
    m_nextId++;
    
    return true;
}

bool ShortcutManager::UnregisterHotkey(const std::string& id) {
    auto it = m_hotkeyIds.find(id);
    if (it == m_hotkeyIds.end()) return false;
    
    UnregisterHotKey(NULL, it->second);
    m_callbacks.erase(it->second);
    m_hotkeyIds.erase(it);
    
    return true;
}

void ShortcutManager::UnregisterAll() {
    for (const auto& pair : m_hotkeyIds) {
        UnregisterHotKey(NULL, pair.second);
    }
    m_hotkeyIds.clear();
    m_callbacks.clear();
}

bool ShortcutManager::IsKeyConflict(const std::string& key) {
    int keyCode = ParseKey(key);
    if (keyCode == 0) return true;
    
    int modifiers = keyCode & 0xFF00;
    int vkCode = keyCode & 0x00FF;
    
    bool result = !RegisterHotKey(NULL, 0xFFFF, modifiers, vkCode);
    if (!result) {
        UnregisterHotKey(NULL, 0xFFFF);
    }
    return result;
}

void ShortcutManager::ProcessMessage(MSG& msg) {
    if (msg.message == WM_HOTKEY) {
        int id = msg.wParam;
        auto it = m_callbacks.find(id);
        if (it != m_callbacks.end()) {
            it->second();
        }
    }
}
```

---

### 任务 5: 主窗口与标签栏

**Files:**
- Create: `src/ui/MainWindow.h`
- Create: `src/ui/MainWindow.cpp`
- Create: `src/ui/TabBar.h`
- Create: `src/ui/TabBar.cpp`

- [ ] **Step 1: 创建 TabBar 头文件**

```cpp
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
    int m_selectedTab;
};

#endif
```

- [ ] **Step 2: 创建 TabBar 实现文件**

```cpp
#include "TabBar.h"
#include <commctrl.h>

TabBar::TabBar(HWND parent) : m_currentIndex(-1), m_selectedTab(-1) {
    m_hwnd = CreateWindowEx(0, L"STATIC", L"", 
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        0, 0, 0, 30, parent, NULL, NULL, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetWindowSubclass(m_hwnd, [](HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR d, DWORD_PTR) -> LRESULT {
        return reinterpret_cast<TabBar*>(d)->WindowProc(h, m, w, l);
    }, 0, reinterpret_cast<DWORD_PTR>(this));
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
    switch (msg) {
        case WM_LBUTTONDOWN:
            OnMouseDown(wParam, lParam);
            return 0;
        case WM_MOUSEWHEEL:
            OnMouseWheel(wParam, lParam);
            return 0;
        case WM_PAINT:
            DrawTabs();
            return 0;
        default:
            return DefSubclassProc(hwnd, msg, wParam, lParam);
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
            if (y > rect.bottom - 20) {
                int closeX = currentX + tabWidth - 20;
                if (x >= closeX && x < closeX + 15) {
                    CloseTab(i);
                } else {
                    SwitchTab(i);
                }
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
```

- [ ] **Step 3: 创建 MainWindow 头文件**

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include "TabBar.h"
#include "core/WindowManager.h"

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
    void OnCommand(WPARAM wParam, LPARAM lParam);
    
    HWND m_hwnd;
    TabBar* m_tabBar;
    HWND m_contentArea;
    HWND m_activeWindow;
};

#endif
```

- [ ] **Step 4: 创建 MainWindow 实现文件**

```cpp
#include "MainWindow.h"

MainWindow::MainWindow() : m_tabBar(nullptr), m_contentArea(nullptr), m_activeWindow(nullptr) {
    m_hwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"WindowMergerMain",
        L"WindowMerger",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, NULL, NULL
    );
    
    m_tabBar = new TabBar(m_hwnd);
    m_contentArea = CreateWindowEx(0, L"STATIC", L"", 
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        0, 30, 0, 0, m_hwnd, NULL, NULL, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

MainWindow::~MainWindow() {
    if (m_tabBar) delete m_tabBar;
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
    m_tabBar->AddTab(hwnd, title);
    
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
    
    for (int i = 0; i < m_tabBar->GetTabCount(); i++) {
        if (m_tabBar->GetTabHwnd(i) == hwnd) {
            m_tabBar->RemoveTab(i);
            break;
        }
    }
    
    m_activeWindow = m_tabBar->GetTabHwnd(m_tabBar->GetCurrentTabIndex());
}

void MainWindow::SwitchToTab(int index) {
    m_tabBar->SwitchTab(index);
    m_activeWindow = m_tabBar->GetTabHwnd(index);
    
    if (m_activeWindow) {
        ShowWindow(m_activeWindow, SW_SHOW);
        SetFocus(m_activeWindow);
    }
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_SIZE:
            if (pThis) pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_COMMAND:
            if (pThis) pThis->OnCommand(wParam, lParam);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void MainWindow::OnSize(UINT width, UINT height) {
    if (m_tabBar) {
        SetWindowPos(m_tabBar->GetHWND(), NULL, 0, 0, width, 30, SWP_NOZORDER);
    }
    if (m_contentArea) {
        SetWindowPos(m_contentArea, NULL, 0, 30, width, height - 30, SWP_NOZORDER);
    }
    if (m_activeWindow) {
        RECT rect;
        GetClientRect(m_contentArea, &rect);
        SetWindowPos(m_activeWindow, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    }
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam) {
    // Handle menu commands
}
```

---

### 任务 6: 系统托盘

**Files:**
- Create: `src/ui/TrayIcon.h`
- Create: `src/ui/TrayIcon.cpp`

- [ ] **Step 1: 创建 TrayIcon 头文件**

```cpp
#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>
#include <string>

class TrayIcon {
public:
    TrayIcon(HWND parent);
    ~TrayIcon();
    
    void Show();
    void Hide();
    void SetTooltip(const std::wstring& tooltip);
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void ShowMenu();
    
    HWND m_hwnd;
    NOTIFYICONDATA m_nid;
};

#endif
```

- [ ] **Step 2: 创建 TrayIcon 实现文件**

```cpp
#include "TrayIcon.h"

TrayIcon::TrayIcon(HWND parent) : m_hwnd(NULL) {
    m_hwnd = CreateWindowEx(0, L"STATIC", L"TrayIconWindow",
        WS_POPUP, 0, 0, 0, 0, parent, NULL, NULL, NULL);
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetWindowSubclass(m_hwnd, [](HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR d, DWORD_PTR) -> LRESULT {
        return reinterpret_cast<TrayIcon*>(d)->WindowProc(h, m, w, l);
    }, 0, reinterpret_cast<DWORD_PTR>(this));
    
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
    if (msg == WM_USER + 1 && lParam == WM_RBUTTONUP) {
        TrayIcon* pThis = reinterpret_cast<TrayIcon*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (pThis) pThis->ShowMenu();
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
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
```

---

### 任务 7: 设置对话框

**Files:**
- Create: `src/ui/SettingsDialog.h`
- Create: `src/ui/SettingsDialog.cpp`

- [ ] **Step 1: 创建 SettingsDialog 头文件**

```cpp
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <windows.h>
#include "core/Settings.h"

class SettingsDialog {
public:
    static INT_PTR Show(HWND parent, AppSettings& settings);
    
private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void InitControls(HWND hwnd, const AppSettings& settings);
    static void SaveSettings(HWND hwnd, AppSettings& settings);
};

#endif
```

- [ ] **Step 2: 创建 SettingsDialog 实现文件**

```cpp
#include "SettingsDialog.h"

INT_PTR SettingsDialog::Show(HWND parent, AppSettings& settings) {
    return DialogBoxParam(
        NULL,
        MAKEINTRESOURCE(101),
        parent,
        DialogProc,
        reinterpret_cast<LPARAM>(&settings)
    );
}

INT_PTR CALLBACK SettingsDialog::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    AppSettings* pSettings = nullptr;
    
    if (msg == WM_INITDIALOG) {
        pSettings = reinterpret_cast<AppSettings*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pSettings));
        InitControls(hwnd, *pSettings);
        return TRUE;
    }
    
    pSettings = reinterpret_cast<AppSettings*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    SaveSettings(hwnd, *pSettings);
                    EndDialog(hwnd, IDOK);
                    return TRUE;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
            }
            break;
        default:
            return FALSE;
    }
    return FALSE;
}

void SettingsDialog::InitControls(HWND hwnd, const AppSettings& settings) {
    CheckDlgButton(hwnd, 1001, settings.startOnBoot ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, 1002, settings.showNotifications ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, 1003, settings.hideTaskbarIcons ? BST_CHECKED : BST_UNCHECKED);
    
    SetDlgItemText(hwnd, 2001, settings.hotkeys.at("toggle").key.c_str());
    SetDlgItemText(hwnd, 2002, settings.hotkeys.at("next_tab").key.c_str());
    SetDlgItemText(hwnd, 2003, settings.hotkeys.at("prev_tab").key.c_str());
    SetDlgItemText(hwnd, 2004, settings.hotkeys.at("close_tab").key.c_str());
}

void SettingsDialog::SaveSettings(HWND hwnd, AppSettings& settings) {
    settings.startOnBoot = (IsDlgButtonChecked(hwnd, 1001) == BST_CHECKED);
    settings.showNotifications = (IsDlgButtonChecked(hwnd, 1002) == BST_CHECKED);
    settings.hideTaskbarIcons = (IsDlgButtonChecked(hwnd, 1003) == BST_CHECKED);
    
    wchar_t buffer[256];
    
    GetDlgItemText(hwnd, 2001, buffer, 256);
    settings.hotkeys["toggle"].key = std::string(buffer, buffer + wcslen(buffer));
    
    GetDlgItemText(hwnd, 2002, buffer, 256);
    settings.hotkeys["next_tab"].key = std::string(buffer, buffer + wcslen(buffer));
    
    GetDlgItemText(hwnd, 2003, buffer, 256);
    settings.hotkeys["prev_tab"].key = std::string(buffer, buffer + wcslen(buffer));
    
    GetDlgItemText(hwnd, 2004, buffer, 256);
    settings.hotkeys["close_tab"].key = std::string(buffer, buffer + wcslen(buffer));
    
    Settings::save(settings);
}
```

---

### 任务 8: 更新主入口文件

**Files:**
- Modify: `src/main.cpp`

- [ ] **Step 1: 更新 main.cpp 整合所有模块**

```cpp
#include <windows.h>
#include "core/Settings.h"
#include "core/WindowManager.h"
#include "core/AutoMerge.h"
#include "core/ShortcutManager.h"
#include "ui/MainWindow.h"
#include "ui/TrayIcon.h"

MainWindow* g_mainWindow = nullptr;
TrayIcon* g_trayIcon = nullptr;

LRESULT CALLBACK MessageHandler(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSG* pMsg = reinterpret_cast<MSG*>(lParam);
        ShortcutManager::ProcessMessage(*pMsg);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CoInitialize(NULL);
    
    AppSettings settings = Settings::load();
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WindowMergerMain";
    RegisterClass(&wc);
    
    g_mainWindow = new MainWindow();
    g_trayIcon = new TrayIcon(g_mainWindow->GetHWND());
    
    ShortcutManager::RegisterHotkey("toggle", settings.hotkeys["toggle"].key, []() {
        if (g_mainWindow) g_mainWindow->Toggle();
    });
    
    ShortcutManager::RegisterHotkey("next_tab", settings.hotkeys["next_tab"].key, []() {
        if (g_mainWindow) g_mainWindow->SwitchToTab(0);
    });
    
    AutoMerge::LoadRules(settings.autoMergeRules);
    AutoMerge::SetOnMatchCallback([](HWND hwnd) {
        if (g_mainWindow) {
            g_mainWindow->AddWindow(hwnd);
        }
    });
    AutoMerge::StartMonitoring();
    
    HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, MessageHandler, NULL, GetCurrentThreadId());
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(hook);
    AutoMerge::StopMonitoring();
    ShortcutManager::UnregisterAll();
    
    delete g_trayIcon;
    delete g_mainWindow;
    
    CoUninitialize();
    
    return (int)msg.wParam;
}
```

---

## 测试与验证

### 功能测试清单

| 测试项 | 测试内容 | 预期结果 |
|-------|---------|---------|
| 窗口合并 | 手动选择窗口合并到容器 | 窗口成功合并，任务栏图标消失 |
| 标签切换 | 点击标签、滚轮、快捷键切换 | 切换流畅，窗口内容正确显示 |
| 任务栏隐藏 | 合并后检查任务栏 | 原窗口图标不在任务栏显示 |
| 快捷键 | 测试所有快捷键 | 快捷键响应正常，冲突检测有效 |
| 自动合并 | 添加规则后启动匹配程序 | 程序启动后自动合并 |
| 托盘菜单 | 右键点击托盘图标 | 菜单显示正常，功能可用 |

### 性能测试指标

| 指标 | 目标值 |
|-----|-------|
| 启动时间 | < 1秒 |
| 内存占用 | < 50MB |
| CPU占用 | < 5% |

---

## 编译与构建

```bash
# 创建构建目录
mkdir build && cd build

# 生成项目文件
cmake .. -G "Visual Studio 17 2022" -A x64

# 构建项目
cmake --build . --config Release

# 输出位置
# build/Release/WindowMerger.exe
```
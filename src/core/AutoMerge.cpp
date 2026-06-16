#include "AutoMerge.h"
#include <regex>
#include <codecvt>
#include "WindowManager.h"

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
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring patternW = converter.from_bytes(rule.pattern);
            std::wregex pattern(patternW);
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
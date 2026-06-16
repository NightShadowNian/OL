#ifndef AUTOMERGE_H
#define AUTOMERGE_H

#include <windows.h>
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
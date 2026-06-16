#include "ShortcutManager.h"

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
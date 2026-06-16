#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <windows.h>
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
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
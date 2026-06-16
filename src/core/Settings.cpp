#include "Settings.h"
#include <fstream>
#include <shlobj.h>

namespace {
    std::string getConfigPath() {
        wchar_t appData[MAX_PATH];
        SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appData);
        
        std::wstring path = std::wstring(appData) + L"\\WindowMerger";
        CreateDirectoryW(path.c_str(), NULL);
        
        std::string result;
        for (wchar_t c : path) result += (char)c;
        result += "\\config.json";
        return result;
    }
}

AppSettings Settings::load() {
    std::string path = getConfigPath();
    std::ifstream file(path);
    if (!file.is_open()) {
        return getDefault();
    }
    
    file.close();
    return getDefault();
}

void Settings::save(const AppSettings& settings) {
    std::string path = getConfigPath();
    std::ofstream file(path);
    file << "{}";
    file.close();
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
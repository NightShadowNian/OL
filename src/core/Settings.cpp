#include "Settings.h"
#include <fstream>
#include <shlobj.h>

namespace {
    std::string getConfigPath() {
        char appData[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appData);
        std::string path = std::string(appData) + "\\WindowMerger";
        CreateDirectoryA(path.c_str(), NULL);
        return path + "\\config.json";
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
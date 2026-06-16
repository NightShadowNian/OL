cmake_minimum_required(VERSION 3.16)
project(WindowMerger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WindowMerger
    src/main.cpp
    src/core/Settings.cpp
    src/core/WindowManager.cpp
    src/core/AutoMerge.cpp
    src/core/ShortcutManager.cpp
    src/ui/MainWindow.cpp
    src/ui/TabBar.cpp
    src/ui/TrayIcon.cpp
)

target_link_libraries(WindowMerger
    user32.lib
    shell32.lib
    ole32.lib
    psapi.lib
)

set_target_properties(WindowMerger PROPERTIES
    WIN32_EXECUTABLE TRUE
)
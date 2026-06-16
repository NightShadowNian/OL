cmake_minimum_required(VERSION 3.16)
project(WindowMerger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查是否为交叉编译
if(CMAKE_CXX_COMPILER MATCHES "mingw")
    message(STATUS "Cross-compiling for Windows with MinGW")
    set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
else()
    message(STATUS "Building for host platform")
endif()

add_executable(WindowMerger WIN32
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
    user32
    gdi32
    shell32
    ole32
    psapi
)

if(CMAKE_CXX_COMPILER MATCHES "mingw")
    set_target_properties(WindowMerger PROPERTIES
        SUFFIX ".exe"
        LINK_FLAGS "-static"
    )
endif()

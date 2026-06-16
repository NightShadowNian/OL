#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
SRC_DIR="$SCRIPT_DIR/src"

echo "=== WindowMerger 编译脚本 ==="

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 查找 mingw 编译器
if [ -f "/usr/bin/x86_64-w64-mingw32-g++-win32" ]; then
    CXX="/usr/bin/x86_64-w64-mingw32-g++-win32"
elif [ -f "/usr/bin/x86_64-w64-mingw32-g++-posix" ]; then
    CXX="/usr/bin/x86_64-w64-mingw32-g++-posix"
else
    echo "错误: 未找到 MinGW C++ 编译器"
    exit 1
fi

CC="/usr/bin/x86_64-w64-mingw32-gcc-posix"
RC="/usr/bin/x86_64-w64-mingw32-windres"

echo "编译器: $CXX"

# 收集源文件
SOURCES=$(find "$SRC_DIR" -name "*.cpp")

echo "编译 C++ 文件..."
$CXX -c $SOURCES -I"$SRC_DIR" -std=c++17 -static -mwindows -DWIN32 -D_UNICODE -DUNICODE -DWINVER=0x0A00 2>&1 | head -50

if [ $? -ne 0 ]; then
    echo "编译失败"
    exit 1
fi

echo "链接..."
$CXX *.o -o WindowMerger.exe -static -mwindows -luser32 -lshell32 -lole32 -lpsapi -lgdi32 2>&1

if [ -f "WindowMerger.exe" ]; then
    echo ""
    echo "=== 编译成功 ==="
    ls -lh WindowMerger.exe
    cp WindowMerger.exe "$SCRIPT_DIR/"
else
    echo ""
    echo "=== 链接失败 ==="
    exit 1
fi

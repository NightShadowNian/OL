#!/bin/bash

# WindowMerger 跨平台编译脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

echo "=== WindowMerger 编译脚本 ==="
echo "编译目录: $BUILD_DIR"

# 检测编译器
if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    COMPILER=x86_64-w64-mingw32-g++
    COMPILER_NAME="MinGW-w64 (x86_64)"
elif command -v i686-w64-mingw32-g++ &> /dev/null; then
    COMPILER=i686-w64-mingw32-g++
    COMPILER_NAME="MinGW-w64 (i686)"
elif command -v g++ &> /dev/null; then
    COMPILER=g++
    COMPILER_NAME="本地 GCC (仅限测试)"
    echo "警告: 未找到 MinGW，将编译为主机平台"
else
    echo "错误: 未找到任何 C++ 编译器"
    exit 1
fi

echo "编译器: $COMPILER_NAME"
echo ""

# 创建构建目录
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置 CMake
if [[ "$COMPILER" == *"mingw"* ]]; then
    echo "配置交叉编译..."
    cmake .. \
        -DCMAKE_CXX_COMPILER="$COMPILER" \
        -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
        -DCMAKE_BUILD_TYPE=Release
else
    echo "配置本地编译..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

# 编译
echo ""
echo "开始编译..."
cmake --build . --config Release -- -j$(nproc)

# 查找生成的 exe 文件
if [[ "$COMPILER" == *"mingw"* ]]; then
    EXE_FILE="$BUILD_DIR/WindowMerger.exe"
else
    EXE_FILE="$BUILD_DIR/WindowMerger"
fi

if [ -f "$EXE_FILE" ]; then
    echo ""
    echo "=== 编译成功 ==="
    echo "输出文件: $EXE_FILE"
    ls -lh "$EXE_FILE"
else
    echo ""
    echo "=== 编译失败 ==="
    exit 1
fi

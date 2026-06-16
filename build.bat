@echo off
echo 正在构建 MPEV 项目...
echo.

REM 检查Qt是否安装
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 未找到 qmake，请确保已安装 Qt 并添加到 PATH 环境变量
    pause
    exit /b 1
)

REM 创建构建目录
if not exist build mkdir build
cd build

REM 运行 qmake
echo 正在运行 qmake...
qmake ../MPEV.pro
if %errorlevel% neq 0 (
    echo 错误: qmake 执行失败
    pause
    exit /b 1
)

REM 编译项目
echo 正在编译项目...
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    nmake
) else (
    nmake
)

if %errorlevel% neq 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo.
echo 构建完成！
echo 可执行文件位于: build/release/MPEV.exe
echo.
pause

@echo off
:: 设置控制台代码页为 UTF-8
chcp 65001 >nul

:: 设置窗口标题（测试中文）
title 依赖清理工具

:: 设置目标目录变量
set "TARGET_DIR=..\build-vs2022\_deps"

:: 检查 Python 是否可用
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 未找到 Python。请确保 Python 已安装并添加到系统 PATH 环境变量中。
    pause
    exit /b 1
)

:: 检查目标目录是否存在
if not exist "%TARGET_DIR%" (
    echo 错误: 目录 "%TARGET_DIR%" 不存在。
    echo 请先创建该目录或检查路径是否正确。
    pause
    exit /b 1
)

:: 执行 Python 脚本
echo 正在执行清理任务...
python .\rm-deps-build.py "%TARGET_DIR%"

:: 检查脚本执行结果
if %errorlevel% equ 0 (
    echo 清理任务完成。
) else (
    echo 清理任务失败。
)

pause

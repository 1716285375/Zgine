@echo off
REM ============================================================================
REM Zgine Build Script - Main Entry Point
REM ============================================================================
REM This script provides a convenient way to build different components
REM Usage:
REM   build.bat [component] [build_type] [pch]
REM   component: runtime, editor, sandbox, tests, all (default: sandbox)
REM   build_type: debug, release, debug-vs, release-vs (default: release)
REM   pch: ON, OFF (default: ON)
REM
REM Or use specific scripts:
REM   build-runtime.bat [build_type] [pch]
REM   build-editor.bat [build_type] [pch]
REM   build-sandbox.bat [build_type] [pch]
REM   build-tests.bat [build_type] [pch]
REM   build-all.bat [build_type] [pch]
REM ============================================================================
setlocal enabledelayedexpansion

echo.
echo ================================================
echo    Zgine Windows Builder
echo ================================================
echo.

REM Parse component (first argument)
set COMPONENT=sandbox
if /i "%~1"=="runtime" set COMPONENT=runtime
if /i "%~1"=="editor" set COMPONENT=editor
if /i "%~1"=="sandbox" set COMPONENT=sandbox
if /i "%~1"=="tests" set COMPONENT=tests
if /i "%~1"=="test" set COMPONENT=tests
if /i "%~1"=="all" set COMPONENT=all

REM If first arg is a build type, use default component
if /i "%~1"=="debug" set COMPONENT=sandbox
if /i "%~1"=="release" set COMPONENT=sandbox
if /i "%~1"=="debug-vs" set COMPONENT=sandbox
if /i "%~1"=="release-vs" set COMPONENT=sandbox

REM Call appropriate build script
if "%COMPONENT%"=="runtime" (
    call build-runtime.bat %~2 %~3
    exit /b %ERRORLEVEL%
)
if "%COMPONENT%"=="editor" (
    call build-editor.bat %~2 %~3
    exit /b %ERRORLEVEL%
)
if "%COMPONENT%"=="sandbox" (
    call build-sandbox.bat %~1 %~2
    exit /b %ERRORLEVEL%
)
if "%COMPONENT%"=="tests" (
    call build-tests.bat %~2 %~3
    exit /b %ERRORLEVEL%
)
if "%COMPONENT%"=="all" (
    call build-all.bat %~2 %~3
    exit /b %ERRORLEVEL%
)

REM Default: build sandbox
call build-sandbox.bat %~1 %~2
exit /b %ERRORLEVEL%

REM ============================================
REM Step 1: Locate Visual Studio 2022
REM ============================================
echo [1/6] Detecting Visual Studio 2022...

set "VSWHERE_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
set "VS_PATH="

if exist "%VSWHERE_PATH%\vswhere.exe" (
    REM 切换目录执行以避开路径空格转义坑
    pushd "%VSWHERE_PATH%"
    for /f "usebackq tokens=*" %%i in (`vswhere.exe -latest -version "[17.0,18.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
    popd
)

if not defined VS_PATH (
    echo    ERROR: Visual Studio 2022 not found. 
    echo    Make sure "Desktop development with C++" is installed.
    pause
    exit /b 1
)

echo    Found: "%VS_PATH%"

REM ============================================
REM Step 2: Enter Developer Environment
REM ============================================
echo [2/6] Entering Developer Environment...

set "VSDEVCMD=%VS_PATH%\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
    echo    ERROR: VsDevCmd.bat not found at: "%VSDEVCMD%"
    pause
    exit /b 1
)

REM 注意：直接调用脚本，外层加引号
call "%VSDEVCMD%" -arch=amd64 -host_arch=amd64 >nul 2>&1
echo    OK - Environment ready

REM ============================================
REM Step 3: Environment Patches
REM ============================================
echo [3/6] Applying environment patches...
set VSLANG=1033
set "CMAKE_CXX_COMPILER_LAUNCHER="
set "CMAKE_C_COMPILER_LAUNCHER="

REM 暂时屏蔽可能冲突的编译工具路径 (如 Strawberry Perl)
set "TEMP_PATH=%PATH%"
set "PATH=%TEMP_PATH:C:\Strawberry=C:\DISABLED_STB%"

REM ============================================
REM Step 4 & 5: Configure and Build
REM ============================================
echo [4/6] Checking tools...
where cmake >nul 2>&1 || (echo ERROR: CMake not found & pause & exit /b 1)

set "CMAKE_PRESET=windows-%BUILD_TYPE%"
if "%FORCE_VS%"=="1" (
    set "CMAKE_PRESET=windows-vs-%BUILD_TYPE%"
) else (
    where ninja >nul 2>&1 || (set "CMAKE_PRESET=windows-vs-%BUILD_TYPE%")
)

echo [5/6] Building Zgine with preset: %CMAKE_PRESET% (PCH=%ZGINE_USE_PCH%)...

REM 清理损坏的缓存
if exist "build\%CMAKE_PRESET%\CMakeCache.txt" del /f /q "build\%CMAKE_PRESET%\CMakeCache.txt"

cmake --preset=%CMAKE_PRESET% -DZGINE_USE_PCH=%ZGINE_USE_PCH%
if errorlevel 1 (echo Configuration FAILED! & pause & exit /b 1)

cmake --build --preset=%CMAKE_PRESET% --parallel
if errorlevel 1 (echo Build FAILED! & pause & exit /b 1)

echo [6/6] Build SUCCESS!
set "EXE_PATH=build\%CMAKE_PRESET%\bin\ZgineSandbox.exe"
if exist "build\%CMAKE_PRESET%\bin\Release\ZgineSandbox.exe" set "EXE_PATH=build\%CMAKE_PRESET%\bin\Release\ZgineSandbox.exe"
if not exist "%EXE_PATH%" set "EXE_PATH=build\%CMAKE_PRESET%\bin\Zgine.exe"
if not exist "%EXE_PATH%" if exist "build\%CMAKE_PRESET%\bin\Release\Zgine.exe" set "EXE_PATH=build\%CMAKE_PRESET%\bin\Release\Zgine.exe"

if exist "%EXE_PATH%" (
    echo Found: "%EXE_PATH%"
    set /p RUN="Run now? (y/n): "
    if /i "!RUN!"=="y" "%EXE_PATH%"
)
pause

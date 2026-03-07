@echo off
REM ============================================================================
REM Build ZgineRuntime Only (Core Library)  ->  build-vs2022
REM Usage: build-runtime.bat [debug|release]
REM ============================================================================
setlocal enabledelayedexpansion

echo.
echo ================================================
echo    Zgine Runtime Builder  (build-vs2022)
echo ================================================
echo.

REM --- Build type ---
set BUILD_TYPE=Release
if /i "%~1"=="debug"   set BUILD_TYPE=Debug
if /i "%~1"=="release" set BUILD_TYPE=Release
echo Build type: %BUILD_TYPE%

REM --- Locate Visual Studio 2022 ---
echo [1/5] Detecting Visual Studio 2022...
set "VSWHERE_DIR=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
set "VS_PATH="
if exist "%VSWHERE_DIR%\vswhere.exe" (
    pushd "%VSWHERE_DIR%"
    for /f "usebackq tokens=*" %%i in (
        `vswhere.exe -latest -version "[17.0,18.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`
    ) do set "VS_PATH=%%i"
    popd
)
if not defined VS_PATH (
    echo ERROR: Visual Studio 2022 not found.
    pause & exit /b 1
)
echo    Found: "%VS_PATH%"

REM --- Developer environment ---
echo [2/5] Entering Developer Environment...
call "%VS_PATH%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64 >nul 2>&1
echo    OK

REM --- Environment patches ---
set VSLANG=1033
set "CMAKE_CXX_COMPILER_LAUNCHER="
set "CMAKE_C_COMPILER_LAUNCHER="
set "PATH=%PATH:C:\Strawberry=C:\DISABLED_STB%"

REM --- Check CMake ---
echo [3/5] Checking tools...
where cmake >nul 2>&1 || (echo ERROR: CMake not found & pause & exit /b 1)

REM --- Configure ---
echo [4/5] Configuring (Runtime only)...
cmake -S . -B build-vs2022 -G "Visual Studio 17 2022" -A x64 ^
    -DZGINE_BUILD_SANDBOX=OFF ^
    -DZGINE_BUILD_EDITOR=OFF ^
    -DZGINE_BUILD_TESTS=OFF ^
    -DZGINE_USE_PCH=ON
if errorlevel 1 (echo Configuration FAILED! & pause & exit /b 1)

REM --- Build ---
echo [5/5] Building ZgineRuntime...
cmake --build build-vs2022 --config %BUILD_TYPE% --target ZgineRuntime --parallel 8
if errorlevel 1 (echo Build FAILED! & pause & exit /b 1)

echo.
echo ================================================
echo    Runtime Build Complete  [%BUILD_TYPE%]
echo ================================================
echo    Library: build-vs2022\lib\%BUILD_TYPE%\ZgineRuntime.lib
echo.
pause

@echo off
REM ============================================================================
REM Build Zgine Sandbox with Nuklear Backend
REM ============================================================================
setlocal enabledelayedexpansion

echo.
echo ================================================
echo    Zgine Nuklear Builder
echo ================================================
echo.

REM Parse arguments
set BUILD_TYPE=debug
set FORCE_VS=0
set ZGINE_USE_PCH=ON

REM ============================================
REM Step 1: Locate Visual Studio 2022
REM ============================================
echo [1/6] Detecting Visual Studio 2022...

set "VSWHERE_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
set "VS_PATH="

if exist "%VSWHERE_PATH%\vswhere.exe" (
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

call "%VSDEVCMD%" -arch=amd64 -host_arch=amd64 >nul 2>&1
echo    OK - Environment ready

REM ============================================
REM Step 3: Environment Patches
REM ============================================
echo [3/6] Applying environment patches...
set VSLANG=1033
set "CMAKE_CXX_COMPILER_LAUNCHER="
set "CMAKE_C_COMPILER_LAUNCHER="
set "TEMP_PATH=%PATH%"
set "PATH=%TEMP_PATH:C:\Strawberry=C:\DISABLED_STB%"

REM ============================================
REM Step 4: Check Tools
REM ============================================
echo [4/6] Checking tools...
where cmake >nul 2>&1 || (echo ERROR: CMake not found & pause & exit /b 1)

set "CMAKE_PRESET=windows-%BUILD_TYPE%"
where ninja >nul 2>&1 || (set "CMAKE_PRESET=windows-vs-%BUILD_TYPE%")

REM ============================================
REM Step 5: Configure Build (FORCE RECONFIGURE FOR NUKLEAR)
REM ============================================
echo [5/6] Configuring Zgine with Nuklear Backend (Preset: %CMAKE_PRESET%)...

REM Ensure we clean cache to force reconfiguration of the option
if exist "build\%CMAKE_PRESET%\CMakeCache.txt" del /f /q "build\%CMAKE_PRESET%\CMakeCache.txt"

cmake --preset=%CMAKE_PRESET% -DZGINE_USE_PCH=%ZGINE_USE_PCH% -DZGINE_BUILD_SANDBOX=ON -DZGINE_BUILD_EDITOR=OFF -DZGINE_BUILD_TESTS=OFF -DZGINE_GUI_NUKLEAR=ON
if errorlevel 1 (
    echo Configuration FAILED!
    pause
    exit /b 1
)

REM ============================================
REM Step 6: Build
REM ============================================
echo [6/6] Building ZgineRuntime and ZgineSandbox...
cmake --build --preset=%CMAKE_PRESET% --parallel 8
if errorlevel 1 (
    echo Build FAILED!
    pause
    exit /b 1
)

echo.
echo ================================================
echo    Nuklear Build Complete
echo ================================================
set "EXE_PATH=build\%CMAKE_PRESET%\bin\ZgineSandbox.exe"
if exist "build\%CMAKE_PRESET%\bin\Debug\ZgineSandbox.exe" set "EXE_PATH=build\%CMAKE_PRESET%\bin\Debug\ZgineSandbox.exe"

if exist "%EXE_PATH%" (
    echo    Executable: "%EXE_PATH%"
    set /p RUN="Run Nuklear Sandbox now? (y/n): "
    if /i "!RUN!"=="y" "%EXE_PATH%"
) else (
    echo    WARNING: Executable not found at expected location
)
echo.
pause

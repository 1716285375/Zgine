@echo off
REM ============================================================================
REM Build and Run Editor Tests
REM ============================================================================
setlocal enabledelayedexpansion

echo.
echo ================================================
echo    Zgine Editor Tests Builder
echo ================================================
echo.

REM Parse arguments
set BUILD_TYPE=Debug
set RUN_TESTS=1
if /i "%~1"=="Release" set BUILD_TYPE=Release
if /i "%~1"=="debug" set BUILD_TYPE=Debug
if /i "%~1"=="release" set BUILD_TYPE=Release
if /i "%~2"=="--no-run" set RUN_TESTS=0

REM ============================================
REM Step 1: Locate Visual Studio 2022
REM ============================================
echo [1/5] Detecting Visual Studio 2022...

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
echo [2/5] Entering Developer Environment...

set "VSDEVCMD=%VS_PATH%\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
    echo    ERROR: VsDevCmd.bat not found at: "%VSDEVCMD%"
    pause
    exit /b 1
)

call "%VSDEVCMD%" -arch=amd64 -host_arch=amd64 >nul 2>&1
echo    OK - Environment ready

REM ============================================
REM Step 3: Check Tools
REM ============================================
echo [3/5] Checking tools...
where cmake >nul 2>&1 || (echo ERROR: CMake not found & pause & exit /b 1)

REM ============================================
REM Step 4: Configure and Build
REM ============================================
echo [4/5] Configuring and building Editor tests (%BUILD_TYPE%)...

REM Create build directory if not exists
if not exist "build" mkdir build
cd build

REM Configure
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DZGINE_BUILD_TESTS=ON ^
    -DZGINE_BUILD_EDITOR=ON ^
    -DZGINE_BUILD_SANDBOX=OFF

if errorlevel 1 (
    echo Configuration FAILED!
    cd ..
    pause
    exit /b 1
)

REM Build Editor Tests target
cmake --build . --config %BUILD_TYPE% --target ZgineEditorTests

if errorlevel 1 (
    echo Build FAILED!
    cd ..
    pause
    exit /b 1
)

cd ..

REM ============================================
REM Step 5: Run Tests
REM ============================================
echo [5/5] Locating test executable...

set "TEST_EXE=build\tests\%BUILD_TYPE%\ZgineEditorTests.exe"

if not exist "%TEST_EXE%" (
    REM Try alternative paths
    if exist "build\bin\%BUILD_TYPE%\ZgineEditorTests.exe" (
        set "TEST_EXE=build\bin\%BUILD_TYPE%\ZgineEditorTests.exe"
    ) else if exist "build\%BUILD_TYPE%\ZgineEditorTests.exe" (
        set "TEST_EXE=build\%BUILD_TYPE%\ZgineEditorTests.exe"
    )
)

if not exist "%TEST_EXE%" (
    echo.
    echo WARNING: Test executable not found!
    echo Expected locations:
    echo   - build\tests\%BUILD_TYPE%\ZgineEditorTests.exe
    echo   - build\bin\%BUILD_TYPE%\ZgineEditorTests.exe
    echo   - build\%BUILD_TYPE%\ZgineEditorTests.exe
    echo.
    pause
    exit /b 1
)

echo    Found: "%TEST_EXE%"
echo.
echo ================================================
echo    Editor Tests Build Complete
echo ================================================
echo.

if "%RUN_TESTS%"=="1" (
    echo Running Editor tests...
    echo.
    "%TEST_EXE%" --gtest_color=yes

    if errorlevel 1 (
        echo.
        echo ================================================
        echo    TESTS FAILED
        echo ================================================
    ) else (
        echo.
        echo ================================================
        echo    ALL TESTS PASSED
        echo ================================================
    )
) else (
    echo Tests built but not run (use --no-run to skip running)
    echo To run tests manually: "%TEST_EXE%"
)

echo.
pause

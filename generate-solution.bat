@echo off
REM ============================================================================
REM Generate Visual Studio 2022 Solution for Zgine
REM ============================================================================
REM This script generates the .sln file using CMake with --fresh parameter.
REM After running this, manually open the solution in Visual Studio 2022.
REM ============================================================================

echo.
echo ========================================
echo Generating Visual Studio 2022 Solution
echo ========================================
echo.

REM Set build directory
set BUILD_DIR=build-vs2022

REM Remove old build directory if --clean flag is provided
if "%1"=="--clean" (
    echo Cleaning old build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    echo.
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Navigate to build directory
cd "%BUILD_DIR%"

echo Running CMake to generate Visual Studio 2022 solution...
echo.

REM Generate solution with --fresh to reconfigure from scratch
REM Enable Editor build
cmake --fresh -G "Visual Studio 17 2022" -A x64 -DZGINE_BUILD_EDITOR=ON ..

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo ERROR: CMake generation failed!
    echo ========================================
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS!
echo ========================================
echo.
echo Solution file location:
echo   %CD%\Zgine.sln
echo.
echo To build the project:
echo   1. Open %CD%\Zgine.sln in Visual Studio 2022
echo   2. Select your desired configuration (Debug/Release)
echo   3. Build the solution (Ctrl+Shift+B)
echo.
echo Or run: start Zgine.sln
echo.

REM Return to root directory
cd ..

pause

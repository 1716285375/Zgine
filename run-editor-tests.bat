@echo off
REM Quick script to run Editor tests after building
setlocal

set BUILD_TYPE=Debug
if /i "%~1"=="Release" set BUILD_TYPE=Release
if /i "%~1"=="release" set BUILD_TYPE=Release

set "TEST_EXE=build\tests\%BUILD_TYPE%\ZgineEditorTests.exe"

if not exist "%TEST_EXE%" (
    if exist "build\bin\%BUILD_TYPE%\ZgineEditorTests.exe" (
        set "TEST_EXE=build\bin\%BUILD_TYPE%\ZgineEditorTests.exe"
    ) else if exist "build\%BUILD_TYPE%\ZgineEditorTests.exe" (
        set "TEST_EXE=build\%BUILD_TYPE%\ZgineEditorTests.exe"
    )
)

if not exist "%TEST_EXE%" (
    echo ERROR: Test executable not found!
    echo Please build tests first using build-editor-tests.bat
    pause
    exit /b 1
)

echo Running Editor tests from: %TEST_EXE%
echo.

REM Run with all Google Test features
"%TEST_EXE%" --gtest_color=yes %*

echo.
pause

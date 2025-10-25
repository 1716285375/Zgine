@echo off
echo ========================================
echo Zgine Engine - Fast Build Script
echo ========================================
echo.

REM 设置环境变量
set MSBUILD_EXE_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe
if not exist "%MSBUILD_EXE_PATH%" (
    set MSBUILD_EXE_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe
)

REM 检查MSBuild是否存在
if not exist "%MSBUILD_EXE_PATH%" (
    echo Error: MSBuild not found!
    echo Please install Visual Studio 2019 or 2022
    pause
    exit /b 1
)

echo Using MSBuild: %MSBUILD_EXE_PATH%
echo.

REM 重新生成项目文件（如果需要）
if "%1"=="clean" (
    echo Cleaning project files...
    del /q /s bin-int\*.* 2>nul
    del /q /s bin\*.* 2>nul
    echo Project files cleaned.
    echo.
)

REM 生成项目文件
echo Generating project files...
call premake5.exe vs2022
if errorlevel 1 (
    echo Error: Failed to generate project files!
    pause
    exit /b 1
)
echo Project files generated successfully.
echo.

REM 编译配置
set CONFIG=Debug
if "%1"=="release" set CONFIG=Release
if "%1"=="dist" set CONFIG=Dist

echo Building %CONFIG% configuration...
echo.

REM 使用MSBuild进行快速编译
"%MSBUILD_EXE_PATH%" Zgine.sln ^
    /p:Configuration=%CONFIG% ^
    /p:Platform=x64 ^
    /m:8 ^
    /p:BuildInParallel=true ^
    /p:MaxCpuCount=8 ^
    /p:UseMultiToolTask=true ^
    /p:EnforceProcessCountAcrossBuilds=true ^
    /p:CL_MPCount=8 ^
    /p:CL_MPMaxCount=8 ^
    /verbosity:minimal ^
    /nologo

if errorlevel 1 (
    echo.
    echo ========================================
    echo Build failed! Check errors above.
    echo ========================================
    pause
    exit /b 1
) else (
    echo.
    echo ========================================
    echo Build completed successfully!
    echo ========================================
    echo.
    echo Running Sandbox...
    start "" "bin\%CONFIG%-windows-x86_64\Sandbox\Sandbox.exe"
)

pause
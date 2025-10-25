@echo off
echo ========================================
echo Zgine Engine - Incremental Build Script
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

REM 编译配置
set CONFIG=Debug
if "%1"=="release" set CONFIG=Release
if "%1"=="dist" set CONFIG=Dist

echo Building %CONFIG% configuration (Incremental)...
echo.

REM 使用MSBuild进行增量编译
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
    /p:UseSharedCompilation=true ^
    /p:ClCompile_UseSharedCompilation=true ^
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
    echo Incremental build completed successfully!
    echo ========================================
    echo.
    echo Running Sandbox...
    start "" "bin\%CONFIG%-windows-x86_64\Sandbox\Sandbox.exe"
)

pause

@echo off
echo ========================================
echo Zgine Engine - Build Speed Test
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

REM 清理项目
echo Cleaning project...
del /q /s bin-int\*.* 2>nul
del /q /s bin\*.* 2>nul
echo Project cleaned.
echo.

REM 重新生成项目文件
echo Generating project files...
call premake5.exe vs2022
if errorlevel 1 (
    echo Error: Failed to generate project files!
    pause
    exit /b 1
)
echo Project files generated successfully.
echo.

REM 记录开始时间
echo Starting build speed test...
set start_time=%time%
echo Start time: %start_time%
echo.

REM 使用MSBuild进行编译
"%MSBUILD_EXE_PATH%" Zgine.sln ^
    /p:Configuration=Debug ^
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

REM 记录结束时间
set end_time=%time%
echo.
echo End time: %end_time%
echo.

if errorlevel 1 (
    echo ========================================
    echo Build failed! Check errors above.
    echo ========================================
    pause
    exit /b 1
) else (
    echo ========================================
    echo Build completed successfully!
    echo ========================================
    echo.
    echo Build time: %start_time% to %end_time%
    echo.
    echo Running Sandbox...
    start "" "bin\Debug-windows-x86_64\Sandbox\Sandbox.exe"
)

pause

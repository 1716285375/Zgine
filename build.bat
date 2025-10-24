@echo off
echo Building Zgine project...

REM Generate project files
vendor\bin\premake\premake5.exe vs2022

REM Build the project
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Zgine.sln /p:Configuration=Debug /p:Platform=x64 /p:UseMultiToolTask=false /p:AdditionalOptions="/FS" /verbosity:minimal

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running application...
    cd bin\Debug-windows-x86_64\Sandbox
    Sandbox.exe
) else (
    echo Build failed!
    pause
)

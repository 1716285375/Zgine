@echo off
setlocal enabledelayedexpansion

echo Starting project file cleanup...

:: Delete bin and bin-int directories in current folder
if exist "bin" (
    echo Deleting bin directory...
    rmdir /s /q "bin"
)

if exist "bin-int" (
    echo Deleting bin-int directory...
    rmdir /s /q "bin-int"
)

:: Delete .sln solution file in current directory
if exist "*.sln" (
    echo Deleting solution files in current directory...
    del /f /q "*.sln"
)

:: Define project directories to process
set "dirs=Sandbox Zgine"

:: Define VS2022 related file extensions to delete
set "extensions=.vcxproj .vcxproj.filters .vcxproj.user .suo .sdf .opensdf .ipch"

:: Iterate through each project directory and delete specified files
for %%d in (%dirs%) do (
    if exist "%%d" (
        echo Processing %%d directory...
        for %%e in (%extensions%) do (
            if exist "%%d\*%%e" (
                echo Deleting *%%e files in %%d directory...
                del /f /q "%%d\*%%e"
            )
        )
    ) else (
        echo %%d directory does not exist, skipping...
    )
)

echo ========== Cleanup Done ============
    
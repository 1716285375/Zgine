@echo off
echo Performing cleanup...
call CleanProject.bat  :: First call Clean.bat to perform cleanup

echo Cleanup completed, starting project generation...
call vendor\bin\premake\premake5.exe vs2022  :: Then call Premake to generate solution

echo ========== Generation Done ============
PAUSE
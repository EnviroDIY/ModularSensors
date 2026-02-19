@echo off
REM Setup script for Git filter drivers used in ModularSensors
REM This configures the smudgePasswords and disableDebug filters referenced in .gitattributes

echo Setting up Git filter drivers for ModularSensors...

REM Configure the smudgePasswords filter for .ino files
echo Configuring smudgePasswords filter...
git config --local filter.smudgePasswords.clean "powershell -ExecutionPolicy Bypass -File filters/cleanPasswords.ps1"
git config --local filter.smudgePasswords.smudge "powershell -ExecutionPolicy Bypass -File filters/smudgePasswords.ps1"

REM Configure the disableDebug filter for ModSensorDebugConfig.h
echo Configuring disableDebug filter...
git config --local filter.disableDebug.clean "powershell -ExecutionPolicy Bypass -File filters/cleanDebugConfig.ps1"
git config --local filter.disableDebug.smudge "powershell -ExecutionPolicy Bypass -File filters/smudgeDebugConfig.ps1"

echo.
echo Git filter drivers have been successfully configured!
echo.
echo Filters configured:
echo   smudgePasswords - Manages credentials in .ino files
echo   disableDebug    - Manages debug defines in ModSensorDebugConfig.h
echo.
echo You may need to run "git checkout HEAD -- ." to apply filters to existing files.

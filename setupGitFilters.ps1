#!/usr/bin/env powershell
# Setup script for Git filter drivers used in ModularSensors
# This configures the smudgePasswords and disableDebug filters referenced in .gitattributes

Write-Host "Setting up Git filter drivers for ModularSensors..." -ForegroundColor Green

# Configure the smudgePasswords filter for .ino files
Write-Host "Configuring smudgePasswords filter..." -ForegroundColor Yellow
git config --local filter.smudgePasswords.clean "powershell -ExecutionPolicy RemoteSigned -File filters/cleanPasswords.ps1"
git config --local filter.smudgePasswords.smudge "powershell -ExecutionPolicy RemoteSigned -File filters/smudgePasswords.ps1"

# Configure the disableDebug filter for ModSensorDebugConfig.h
Write-Host "Configuring disableDebug filter..." -ForegroundColor Yellow
git config --local filter.disableDebug.clean "powershell -ExecutionPolicy RemoteSigned -File filters/cleanDebugConfig.ps1"
git config --local filter.disableDebug.smudge "powershell -ExecutionPolicy RemoteSigned -File filters/smudgeDebugConfig.ps1"

Write-Host "`nGit filter drivers have been successfully configured!" -ForegroundColor Green
Write-Host "`nFilters configured:" -ForegroundColor Cyan
Write-Host "  smudgePasswords - Manages credentials in .ino files" -ForegroundColor White
Write-Host "  disableDebug    - Manages debug defines in ModSensorDebugConfig.h" -ForegroundColor White
Write-Host "`nYou may need to run 'git checkout HEAD -- .' to apply filters to existing files." -ForegroundColor Magenta

#  .\buildAlias - doesn't work
#Function doMe.ps1 {Set-Location -Path ..\..\tools\build\}
#Set-Alias -Name dobuild -Value doMe.ps1
$ps_script_dir = "..\..\tools\build"
New-Alias -Name dobuild2  doMe.ps1


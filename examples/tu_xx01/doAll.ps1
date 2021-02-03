#  .\dodAll.ps1 0_xx_y
$parm1 = $($args)
$parm2 = Get-Date -Format "yyMMdd_HHmm"
$log_file = -join("build_","$parm2",".txt" )
$dest_dir = "..\..\..\releases"
$libdeps_dir = ".pio\libdeps"


# if ext remove all libdeps
if (Test-Path -Path $libdeps_dir) {
    Write-Output  "Removing $libdeps_dir "
    Remove-item $libdeps_dir -Recurse -Force
}
.\doBuild.ps1 $parm1  | Out-File -FilePath $log_file -NoClobber

Copy-Item $log_file  $dest_dir

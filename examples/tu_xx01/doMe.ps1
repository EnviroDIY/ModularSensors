#  .\buildAll.ps1 0_xx_y
$parm1 = $($args)
$parm2 = Get-Date -Format "yyMMdd_HHmm"
$log_file = -join("maylfy_",$parm1,"_$parm2","_log.txt" )
$dest_dir = "..\..\..\releases"

.\doBuild.ps1 $parm1  | Out-File -FilePath $log_file -NoClobber

Move-Item $log_file  $dest_dir
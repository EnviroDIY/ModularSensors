#  .\do.ps1 -rel 0_xx_y
#$param1=$args[0]
#Get-Executionpolicy
#Set-ExecutionPolicy -Scope CurrentUser unrestricted
#param ($rel = $(throw "rel required"))
#write-host "doing $($args[0])  $($args.count) something" 
#for ( $i = 0; $i -lt $args.count; $i++ ) {
#    write-host "Argument  $i is $($args[$i])"
#} 


$config1 = $($args)
$config2 = Get-Date -Format "yyMMdd_HHmm"
$dest_dir = "..\..\..\releases"

function Do-Build {
    $dest_file = -join($dest_dir,"\mayfly",$config1, "_",$config2,$hext,".hex")
    $src_file  = -join("src\ms_cfg.h","$hext" )

    if (-not (Test-Path -Path $src_file)) {
        #Write-Host  "The file does not exist $src_file"
        throw  "The file does not exist $src_file"
    } else {
        Write-Host "Output is $dest_file"
        copy  src\ms_cfg.h$hext .\src\ms_cfg.h
        C:\Users\neilh77a\.platformio\penv\Scripts\pio run
        copy .\.pio\build\mayfly\firmware.hex  $dest_file
    }
}

if (-not (Test-Path -Path $dest_dir)) {
    #Write-Host  "The file does not exist $src_file"
    throw  "The file does not exist $dest_dir"
}

$hext = "_EC"
Do-Build

$hext = "_LT5_lte"
Do-Build

$hext = "_LT5_wifi"
Do-Build

$hext = "_nano"
Do-Build

$hext = "_test"
Do-Build
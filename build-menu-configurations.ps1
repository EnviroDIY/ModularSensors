$ErrorActionPreference = "Stop"

mkdir temp -Force
mkdir temp/menu_a_la_carte -Force

$pioCommand = "pio lib install"
$pioCommand += ';$?'

$pioResult = Invoke-Expression $pioCommand
if (("$pioResult".EndsWith('False')) -or  (-not $pioResult)){
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
    Write-Host "PlatformIO Library Installation Failed" -ForegroundColor Red
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
    Exit 1
}

$pioCommand = "pio lib update"
$pioCommand += ';$?'

$pioResult = Invoke-Expression $pioCommand
if (("$pioResult".EndsWith('False')) -or  (-not $pioResult)){
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
    Write-Host "PlatformIO Library Update Failed" -ForegroundColor Red
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
    Exit 1
}

$pioCommand = "pio run --project-conf=""continuous_integration/platformio.ini"" --verbose"
$pioCommand += ';$?'

$modemFlags = @(`
    'BUILD_MODEM_DIGI_XBEE_CELLULAR_TRANSPARENT', `
    'BUILD_MODEM_DIGI_XBEE_LTE_BYPASS', `
    'BUILD_MODEM_DIGI_XBEE_3G_BYPASS', `
    'BUILD_MODEM_DIGI_XBEE_WIFI', `
    'BUILD_MODEM_ESPRESSIF_ESP8266', `
    'BUILD_MODEM_QUECTEL_BG96', `
    'BUILD_MODEM_SEQUANS_MONARCH', `
    'BUILD_MODEM_SIM_COM_SIM800', `
    'BUILD_MODEM_SIM_COM_SIM7000', `
    'BUILD_MODEM_SODAQ2_G_BEE_R6', `
    'BUILD_MODEM_SODAQ_UBEE_R410M', `
    'BUILD_MODEM_SODAQ_UBEE_U201')

Foreach ($modemFlag in $modemFlags)
{
    $tempFile = "temp/menu_a_la_carte/main.cpp"
    if (Test-Path $tempFile) {
        Remove-Item $tempFile
    }
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host "Modifying source for $modemFlag" -ForegroundColor Green
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    $originalMenu = Get-Content -Path "examples/menu_a_la_carte/menu_a_la_carte.ino" -Encoding UTF8 -Raw
    $newHeading =  "#define $modemFlag`n#define BUILD_TEST_PRE_NAMED_VARS`n"
    $newHeading += $originalMenu
    $newHeading | Add-Content -Path $tempFile -Encoding UTF8

    # Write-Output "First few lines of source"
    # Get-Content $tempFile | select -Skip 10

    $pioResult = Invoke-Expression $pioCommand
    if (("$pioResult".EndsWith('False')) -or  (-not $pioResult)){
        Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
        Write-Host "PlatformIO Build Failed" -ForegroundColor Red
        Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
        Remove-Item –path temp –recurse
        Exit 1
    }
}

$sensorFlags = @(`
    'BUILD_SENSOR_AO_SONG_AM2315', `
    'BUILD_SENSOR_AO_SONG_DHT', `
    'BUILD_SENSOR_APOGEE_SQ212', `
    'BUILD_SENSOR_ATLAS_SCIENTIFIC_CO2', `
    'BUILD_SENSOR_ATLAS_SCIENTIFIC_DO', `
    'BUILD_SENSOR_ATLAS_SCIENTIFIC_ORP', `
    'BUILD_SENSOR_ATLAS_SCIENTIFIC_PH', `
    'BUILD_SENSOR_ATLAS_SCIENTIFIC_RTD', `
    'BUILD_SENSOR_ATLAS_SCIENTIFIC_EC', `
    'BUILD_SENSOR_BOSCH_BME280', `
    'BUILD_SENSOR_CAMPBELL_OBS3', `
    'BUILD_SENSOR_DECAGON_ES2', `
    'BUILD_SENSOR_TIADS1X15', `
    'BUILD_SENSOR_FREESCALE_MPL115A2', `
    'BUILD_SENSOR_IN_SITU_RDO', `
    'BUILD_SENSOR_IN_SITU_TROLL_SDI12A', `
    'BUILD_SENSOR_KELLER_ACCULEVEL', `
    'BUILD_SENSOR_KELLER_NANOLEVEL', `
    'BUILD_SENSOR_MAX_BOTIX_SONAR', `
    'BUILD_SENSOR_MAXIM_DS18', `
    'BUILD_SENSOR_MEA_SPEC_MS5803', `
    'BUILD_SENSOR_DECAGON_5TM', `
    'BUILD_SENSOR_DECAGON_CTD', `
    'BUILD_SENSOR_METER_TEROS11', `
    'BUILD_SENSOR_PALEO_TERRA_REDOX', `
    'BUILD_SENSOR_RAIN_COUNTER_I2C', `
    'BUILD_SENSOR_TALLY_COUNTER_I2C', `
    'BUILD_SENSOR_SENSIRION_SHT4X', `
    'BUILD_SENSOR_TI_INA219', `
    'BUILD_SENSOR_TURNER_CYCLOPS', `
    'BUILD_SENSOR_ANALOG_ELEC_CONDUCTIVITY', `
    'BUILD_SENSOR_YOSEMITECH_Y504', `
    'BUILD_SENSOR_YOSEMITECH_Y510', `
    'BUILD_SENSOR_YOSEMITECH_Y511', `
    'BUILD_SENSOR_YOSEMITECH_Y514', `
    'BUILD_SENSOR_YOSEMITECH_Y520', `
    'BUILD_SENSOR_YOSEMITECH_Y532', `
    'BUILD_SENSOR_YOSEMITECH_Y533', `
    'BUILD_SENSOR_YOSEMITECH_Y551', `
    'BUILD_SENSOR_YOSEMITECH_Y560', `
    'BUILD_SENSOR_YOSEMITECH_Y4000', `
    'BUILD_SENSOR_ZEBRA_TECH_D_OPTO')

Foreach ($sensorFlag in $sensorFlags)
{
    $tempFile = "temp/menu_a_la_carte/main.cpp"
    if (Test-Path $tempFile) {
        Remove-Item $tempFile
    }
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host "Modifying source for $sensorFlag" -ForegroundColor Green
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    $originalMenu = Get-Content -Path "examples/menu_a_la_carte/menu_a_la_carte.ino" -Encoding UTF8 -Raw
    $newHeading =  "#define BUILD_MODEM_DIGI_XBEE_CELLULAR_TRANSPARENT`n#define BUILD_TEST_PRE_NAMED_VARS`n#define $sensorFlag`n"
    $newHeading += $originalMenu
    $newHeading | Add-Content -Path $tempFile -Encoding UTF8

    # # Write-Output "First few lines of source"
    # Get-Content $tempFile | select -Skip 10

    $pioResult = Invoke-Expression $pioCommand
    if (("$pioResult".EndsWith('False')) -or  (-not $pioResult)){
        Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
        Write-Host "PlatformIO Build Failed" -ForegroundColor Red
        Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
        Remove-Item –path temp –recurse
        Exit 1
    }
}

$publisherFlag = @(`
    'BUILD_PUB_ENVIRO_DIY_PUBLISHER', `
    'BUILD_PUB_DREAM_HOST_PUBLISHER', `
    'BUILD_PUB_THING_SPEAK_PUBLISHER')

Foreach ($publisherFlag in $publisherFlags)
{
    $tempFile = "temp/menu_a_la_carte/main.cpp"
    if (Test-Path $tempFile) {
        Remove-Item $tempFile
    }
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host "Modifying source for $publisherFlag" -ForegroundColor Green
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Cyan
    $originalMenu = Get-Content -Path "examples/menu_a_la_carte/menu_a_la_carte.ino" -Encoding UTF8 -Raw
    $newHeading =  "#define BUILD_MODEM_DIGI_XBEE_CELLULAR_TRANSPARENT`n#define BUILD_TEST_PRE_NAMED_VARS`n#define $publisherFlag`n"
    $newHeading += $originalMenu
    $newHeading | Add-Content -Path $tempFile -Encoding UTF8

    # Write-Output "First few lines of source"
    # Get-Content $tempFile | select -Skip 10

    $pioResult = Invoke-Expression $pioCommand
    if (("$pioResult".EndsWith('False')) -or  (-not $pioResult)){
        Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
        Write-Host "PlatformIO Build Failed" -ForegroundColor Red
        Write-Host  "----------------------------------------------------------------------------" -ForegroundColor Red
        Remove-Item –path temp –recurse
        Exit 1
    }
}

Remove-Item –path temp –recurse
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
    'MS_BUILD_MODEM_XBEE_CELLULAR', `
    'MS_BUILD_MODEM_XBEE_LTE_B', `
    'MS_BUILD_MODEM_XBEE_3G_B', `
    'MS_BUILD_MODEM_XBEE_WIFI', `
    'MS_BUILD_MODEM_ESP8266', `
    'MS_BUILD_MODEM_BG96', `
    'MS_BUILD_MODEM_MONARCH', `
    'MS_BUILD_MODEM_SIM800', `
    'MS_BUILD_MODEM_SIM7000', `
    'MS_BUILD_MODEM_S2GB', `
    'MS_BUILD_MODEM_UBEE_R410M', `
    'MS_BUILD_MODEM_UBEE_U201')

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
    $newHeading =  "#define $modemFlag`n#define MS_BUILD_TEST_PRE_NAMED_VARS`n"
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
    'MS_BUILD_SENSOR_AM2315', `
    # 'MS_BUILD_SENSOR_DHT', `
    'MS_BUILD_SENSOR_SQ212', `
    'MS_BUILD_SENSOR_ATLASCO2', `
    'MS_BUILD_SENSOR_ATLASDO', `
    'MS_BUILD_SENSOR_ATLASORP', `
    'MS_BUILD_SENSOR_ATLASPH', `
    'MS_BUILD_SENSOR_ATLASRTD', `
    'MS_BUILD_SENSOR_ATLASEC', `
    'MS_BUILD_SENSOR_BME280', `
    'MS_BUILD_SENSOR_OBS3', `
    'MS_BUILD_SENSOR_ES2', `
    'MS_BUILD_SENSOR_VOLTAGE', `
    'MS_BUILD_SENSOR_MPL115A2', `
    'MS_BUILD_SENSOR_INSITURDO', `
    'MS_BUILD_SENSOR_ACCULEVEL', `
    'MS_BUILD_SENSOR_NANOLEVEL', `
    'MS_BUILD_SENSOR_MAXBOTIX', `
    'MS_BUILD_SENSOR_DS18', `
    'MS_BUILD_SENSOR_MS5803', `
    'MS_BUILD_SENSOR_5TM', `
    'MS_BUILD_SENSOR_CTD', `
    'MS_BUILD_SENSOR_TEROS11', `
    'MS_BUILD_SENSOR_PALEOTERRA', `
    'MS_BUILD_SENSOR_RAINI2C', `
    'MS_BUILD_SENSOR_TALLY', `
    'MS_BUILD_SENSOR_INA219', `
    'MS_BUILD_SENSOR_CYCLOPS', `
    'MS_BUILD_SENSOR_ANALOGEC', `
    'MS_BUILD_SENSOR_Y504', `
    'MS_BUILD_SENSOR_Y510', `
    'MS_BUILD_SENSOR_Y511', `
    'MS_BUILD_SENSOR_Y514', `
    'MS_BUILD_SENSOR_Y520', `
    'MS_BUILD_SENSOR_Y532', `
    'MS_BUILD_SENSOR_Y533', `
    'MS_BUILD_SENSOR_Y550', `
    'MS_BUILD_SENSOR_Y4000', `
    'MS_BUILD_SENSOR_DOPTO')

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
    $newHeading =  "#define MS_BUILD_MODEM_XBEE_CELLULAR`n#define MS_BUILD_TEST_PRE_NAMED_VARS`n#define $sensorFlag`n"
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
    'MS_BUILD_PUB_MMW', `
    'MS_BUILD_PUB_DREAMHOST', `
    'MS_BUILD_PUB_THINGSPEAK')

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
    $newHeading =  "#define MS_BUILD_MODEM_XBEE_CELLULAR`n#define MS_BUILD_TEST_PRE_NAMED_VARS`n#define $publisherFlag`n"
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
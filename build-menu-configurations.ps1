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
    'BUILD_MODEM_XBEE_CELLULAR', `
    'BUILD_MODEM_XBEE_LTE_B', `
    'BUILD_MODEM_XBEE_3G_B', `
    'BUILD_MODEM_XBEE_WIFI', `
    'BUILD_MODEM_ESP8266', `
    'BUILD_MODEM_BG96', `
    'BUILD_MODEM_MONARCH', `
    'BUILD_MODEM_SIM800', `
    'BUILD_MODEM_SIM7000', `
    'BUILD_MODEM_S2GB', `
    'BUILD_MODEM_UBEE_R410M', `
    'BUILD_MODEM_UBEE_U201')

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
    'BUILD_SENSOR_AM2315', `
    # 'BUILD_SENSOR_DHT', `
    'BUILD_SENSOR_SQ212', `
    'BUILD_SENSOR_ATLASCO2', `
    'BUILD_SENSOR_ATLASDO', `
    'BUILD_SENSOR_ATLASORP', `
    'BUILD_SENSOR_ATLASPH', `
    'BUILD_SENSOR_ATLASRTD', `
    'BUILD_SENSOR_ATLASEC', `
    'BUILD_SENSOR_BME280', `
    'BUILD_SENSOR_OBS3', `
    'BUILD_SENSOR_ES2', `
    'BUILD_SENSOR_VOLTAGE', `
    'BUILD_SENSOR_MPL115A2', `
    'BUILD_SENSOR_INSITURDO', `
    'BUILD_SENSOR_ACCULEVEL', `
    'BUILD_SENSOR_NANOLEVEL', `
    'BUILD_SENSOR_MAXBOTIX', `
    'BUILD_SENSOR_DS18', `
    'BUILD_SENSOR_MS5803', `
    'BUILD_SENSOR_5TM', `
    'BUILD_SENSOR_CTD', `
    'BUILD_SENSOR_TEROS11', `
    'BUILD_SENSOR_PALEOTERRA', `
    'BUILD_SENSOR_RAINI2C', `
    'BUILD_SENSOR_TALLY', `
    'BUILD_SENSOR_INA219', `
    'BUILD_SENSOR_CYCLOPS', `
    'BUILD_SENSOR_ANALOGEC', `
    'BUILD_SENSOR_Y504', `
    'BUILD_SENSOR_Y510', `
    'BUILD_SENSOR_Y511', `
    'BUILD_SENSOR_Y514', `
    'BUILD_SENSOR_Y520', `
    'BUILD_SENSOR_Y532', `
    'BUILD_SENSOR_Y533', `
    'BUILD_SENSOR_Y550', `
    'BUILD_SENSOR_Y4000', `
    'BUILD_SENSOR_DOPTO')

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
    $newHeading =  "#define BUILD_MODEM_XBEE_CELLULAR`n#define BUILD_TEST_PRE_NAMED_VARS`n#define $sensorFlag`n"
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
    'BUILD_PUB_MMW', `
    'BUILD_PUB_DREAMHOST', `
    'BUILD_PUB_THINGSPEAK')

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
    $newHeading =  "#define BUILD_MODEM_XBEE_CELLULAR`n#define BUILD_TEST_PRE_NAMED_VARS`n#define $publisherFlag`n"
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
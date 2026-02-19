# Developer Setup<!--! {#page_developer_setup} -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [Developer Setup](#developer-setup)
  - [Git Filter Setup](#git-filter-setup)
  - [PlatformIO Setup](#platformio-setup)
  - [Debugging](#debugging)

<!--! @endif -->

If you want to fork this repository and work with it, you'll need to set PlatformIO up a bit differently than you would to merely use this library.

First, fork this repository into your own GitHub space.
Clone it to your local computer.

## Git Filter Setup

This repository uses Git filters to manage sensitive credentials and debug configurations. After cloning the repository, run one of the following setup scripts to configure the necessary Git filter drivers:

**Windows Command Prompt:**
```batch
setupGitFilters.bat
```

**PowerShell:**
```powershell
.\setupGitFilters.ps1
```

**Manual Setup:**
If you prefer to configure the filters manually, run these Git commands from the repository root:
```bash
git config --local filter.smudgePasswords.clean "powershell -ExecutionPolicy Bypass -File filters/cleanPasswords.ps1"
git config --local filter.smudgePasswords.smudge "powershell -ExecutionPolicy Bypass -File filters/smudgePasswords.ps1"
git config --local filter.disableDebug.clean "powershell -ExecutionPolicy Bypass -File filters/cleanDebugConfig.ps1"
git config --local filter.disableDebug.smudge "powershell -ExecutionPolicy Bypass -File filters/smudgeDebugConfig.ps1"
```

These filters provide the following functionality:
- **smudgePasswords** - Manages placeholder credentials in `.ino` example files during development and commits
- **disableDebug** - Automatically manages debug defines in `ModSensorDebugConfig.h` to keep them disabled in commits but enabled locally


## PlatformIO Setup

The Arduino IDE is not a good tool for library development.
Use PlatformIO in VSCode instead.

Open the folder you've cloned this repo into with VSCode.
Have PlatformIO create a new project for you, but instead of allowing it to create a new folder, select the folder you've already cloned this repo into.

Create a new source program to work with in a new directory (ie, a tests directory).
This is the directory you should reference in the `src_dir` line of your platformio.ini.
_Also add this directory to your .gitignore file_ so you can test and play with your code without publishing personal passwords or other messiness to the web.
I recommend you start with the menu a la carte example for development since it already contains all features and is tested for proper compilation with continuous integration tools.

Set your platformio.ini configuration file up like this:

```ini
; PlatformIO Project Configuration File
;

[platformio]
description = ModularSensors Library
; Pick your default environment, if you have more than one and don't want to build all of them every time
default_envs = mayfly
; Set the director for the code you want to test from
src_dir = your_directory/your_source_code

[env]
; Default baud for all examples
monitor_speed = 115200
framework = arduino
; To run code checks; clangtidy must be installed
check_tool = clangtidy
check_src_filters =
    src
    extras
    examples
check_flags =
    clangtidy: --checks=-*
check_skip_packages = yes
; deep search for dependencies, evaluating preprocessor conditionals
lib_ldf_mode = deep+
lib_compat_mode = soft
; look for the library directory
lib_extra_dirs = .
; All these library dependencies must be listed out since we're in the library
; source code and won't read the dependencies from the library.json like a
; typical user would
lib_deps =
    Adafruit BusIO=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit BusIO
    Adafruit GFX Library=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit GFX Library
    Adafruit MPL115A2=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit MPL115A2
    Adafruit NeoPixel=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit NeoPixel
    Adafruit SH110X=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit SH110X
    Adafruit SSD1306=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit SSD1306
    Adafruit Unified Sensor=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit Unified Sensor
    AltSoftSerial=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\AltSoftSerial
    BMP388_DEV=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\BMP388_DEV
    DHT sensor library=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\DHT sensor library
    EnableInterrupt=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\EnableInterrupt
    EnviroDIY_DS3231=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\EnviroDIY_DS3231
    GeoluxCamera=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\GeoluxCamera
    MS5803=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\MS5803
    NeoSWSerial=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\NeoSWSerial
    OneWire=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\OneWire
    PubSubClient=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\PubSubClient
    RTCZero=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\RTCZero
    SDI-12=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SDI-12
    SDI-12_ExtInts=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SDI-12_ExtInts
    SdFat=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SdFat
    SensorModbusMaster=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SensorModbusMaster
    SoftwareSerial_ExternalInts=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SoftwareSerial_ExternalInts
    SoftwareWire=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SoftwareWire
    SparkFun Qwiic RTC RV8803 Arduino Library=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\SparkFun Qwiic RTC RV8803 Arduino Library
    StreamDebugger=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\StreamDebugger
    Tally_Library_I2C=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Tally_Library_I2C
    TinyGSM=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\TinyGSM
    YosemitechModbus=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\YosemitechModbus
    fast_math=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\fast_math
    ANBSensorsModbus=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\ANBSensorsModbus
    Adafruit ADS1X15=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit ADS1X15
    Adafruit AM2315=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit AM2315
    Adafruit BME280 Library=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit BME280 Library
    Adafruit INA219=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit INA219
    Adafruit SHT4x Library=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\Adafruit SHT4x Library
    DallasTemperature=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\DallasTemperature
    GroPointModbus=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\GroPointModbus
    KellerModbus=symlink://C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\lib\KellerModbus
; We have to ignore these folders or PlatformIO will double count all the dependencies
lib_ignore =
    .git
    .github
    .history
    .pio
    .vscode
    archive
    continuous_integration
    docs
    examples
    extras
    ex_one_offs
    filters
    sensor_tests
    tests
    Adafruit GFX Library
    Adafruit SH110X
    Adafruit SSD1306
    Adafruit ADXL343
    Adafruit STMPE610
    Adafruit TouchScreen
    Adafruit ILI9341
; The directories for the ModularSensors library source code
src_filter =
    +<*>
    +<../../src>
    +<../../src/sensors>
    +<../../src/publishers>
    +<../../src/modems>
    +<../../src/WatchDogs>
    +<../../src/clocks>
    +<../../src/utils>
; Some common build flags
build_flags =
    -Wall
    -Wextra
    -D SDI12_EXTERNAL_PCINT
    -D NEOSWSERIAL_EXTERNAL_PCINT

[env:mayfly]
; Find your COM port, enter it here, and remove the semicolon at the start of the line
; upload_port = COM##
; monitor_port = COM##
board = mayfly
platform = atmelavr
framework = arduino
; You probably need some software serial libraries
lib_deps =
    ${env.lib_deps}
    https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git
    https://github.com/PaulStoffregen/AltSoftSerial.git
    https://github.com/SRGDamia1/NeoSWSerial.git
; AVR boards need to ignore RTCZero, it's for SAMD only and will not compile for AVR
lib_ignore =
    ${env.lib_ignore}
    RTCZero
    Adafruit Zero DMA Library
    SDI-12_ExtInts
    Adafruit TinyUSB Library
    ESP8266SdFat
; Any extra build flags you want
build_flags =
    ${env.build_flags}
```

## Debugging

While you're working on development, there is _extensive_ debugging text built into this library.
More on that is in the [Code Debugging](https://github.com/EnviroDIY/ModularSensors/wiki/Code-Debugging) page.
In fact, there is _so much_ debugging that turning it on universally through a build flag will cause the program to be too big to fit on a Mayfly and will likely crash a SAMD board's on-board USB drivers.

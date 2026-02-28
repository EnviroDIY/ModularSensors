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
; All these library dependencies must be listed out since we're in the library
; source code and won't read the dependencies from the library.json like a
; typical user would
lib_deps =
   envirodiy/EnviroDIY_DS3231@^1.3.6
   arduino-libraries/RTCZero@^1.6.0
    sparkfun/SparkFun Qwiic RTC RV8803 Arduino Library@^1.2.10
    greygnome/EnableInterrupt@^1.1.0
    greiman/SdFat@=2.3.0
    TinyGSM=https://github.com/EnviroDIY/TinyGSM
    knolleary/PubSubClient@^2.8
    adafruit/Adafruit BusIO@^1.17.4
    adafruit/Adafruit Unified Sensor@^1.1.15
    adafruit/Adafruit ADS1X15@^2.6.2
    adafruit/Adafruit AM2315@^2.2.3
    adafruit/Adafruit BME280 Library@^2.3.0
    MartinL1/BMP388_DEV@^1.0.11
    adafruit/DHT sensor library@^1.4.6
    adafruit/Adafruit INA219@^1.2.3
    adafruit/Adafruit MPL115A2@^2.0.2
    adafruit/Adafruit SHT4x Library@^1.0.5
    paulstoffregen/OneWire@^2.3.8
    milesburton/DallasTemperature@^4.0.6
    envirodiy/SDI-12@^2.3.2
    SDI-12_ExtInts=https://github.com/EnviroDIY/Arduino-SDI-12#ExtInts
    northernwidget/MS5803@^0.1.2
    Tally_Library_I2C=https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C
    envirodiy/SensorModbusMaster@^1.7.0
    envirodiy/KellerModbus@^0.2.7
    envirodiy/YosemitechModbus@^0.5.4
    envirodiy/GroPointModbus@^0.1.5
    envirodiy/GeoluxCamera@^0.1.3
    robtillaart/fast_math@^0.2.4
    envirodiy/ANBSensorsModbus@^0.4.2
    robtillaart/MS5837@^0.3.2
    StreamDebugger=https://github.com/EnviroDIY/StreamDebugger.git
    NeoSWSerial=https://github.com/SRGDamia1/NeoSWSerial.git
    AltSoftSerial=https://github.com/PaulStoffregen/AltSoftSerial.git
    SoftwareWire=https://github.com/Testato/SoftwareWire.git#v1.5.1
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

[//]: # ( @page page_for_developers Developer Setup )
## Developer Setup

If you want to fork this repository and work with it, you'll need to set PlatformIO up a bit differently than you would to merely use this library.

First, fork this repository into your own GitHub space.
Clone it to your local computer.

Open the folder you've cloned this repo into with either VSCode or Atom.
Have PlatformIO create a new project for you, but instead of allowing it to create a new folder, select the folder you've already cloned this repo into.

Create a new source program to work with in a new directory.
This is the directory you should reference in the `src_dir` line of your platformio.ini.
_Also add this directory to your .gitignore file_ so you can test and play with your code without publishing personal passwords or other messiness to the web.
I recommend you start with one of the programs in the compile_tests folder rather than one of the examples because the compiler test programs are _much_ more extensive and include all sensors and many modems in them.

Set your platformio.ini configuration file up like this:

```ini
; PlatformIO Project Configuration File
;

[platformio]
description = ModularSensors Library
; We need to tell PlatformIO that the main directory is a library
lib_extra_dirs = ., src
; Whatever code you personally want to test from
src_dir = your_directory/your_source_code

[common]
; We have to ignore these folders or PlatformIO will double count all the dependencies
lib_ignore = .git, .pioenvs, .piolibdeps, .vscode, include, doc, examples, sensor_tests, compile_tests, pioScripts
; All these library dependencies must be listed out since we're in the library
; source code and won't read the dependencies from the library.json like a
; typical user would
lib_deps =
.
 EnviroDIY_DS3231
    EnableInterrupt
    SdFat
    ADS1X15
    Adafruit Unified Sensor
    Adafruit AM2315
    Adafruit BME280 Library
    DHT sensor library
    Adafruit MPL115A2
    OneWire
    DallasTemperature
    Arduino-SDI-12
    KellerModbus
    MS5803
    SensorModbusMaster
    YosemitechModbus
    https://github.com/EnviroDIY/TinyGSM.git
    StreamDebugger
    RTCZero
    Adafruit INA219

[env:mayfly]
upload_port = COM##
monitor_speed = 115200
board = mayfly
platform = atmelavr
framework = arduino
lib_ldf_mode = deep+
; You probably need some software serial libraries
lib_deps = ${common.lib_deps}
           https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git
           https://github.com/PaulStoffregen/AltSoftSerial.git
           https://github.com/SRGDamia1/NeoSWSerial.git
; AVR boards need to ignore RTCZero, it's for SAMD only and will not compile for AVR
lib_ignore = ${common.lib_ignore}, RTCZero
;  Use the src filter to ensure subfolders are built
src_filter =
    +<*> +<../../src> +<../../src/sensors> +<../../src/publishers>
; extra_scripts = pioScripts/pio_set_global_flags.py
build_flags =
    -DSDI12_EXTERNAL_PCINT
    -DNEOSWSERIAL_EXTERNAL_PCINT
    -DMQTT_MAX_PACKET_SIZE=240
    -DTINY_GSM_RX_BUFFER=64
    -DTINY_GSM_YIELD_MS=2


[env:adafruit_feather_m0]
upload_port = COM##
platform = atmelsam
board = adafruit_feather_m0
framework = arduino
lib_ldf_mode = deep++
             NeoSWSerial
; SAMD boards need RTCZero for the real time clock and sleeping
lib_deps = ${common.lib_deps}
           RTCZero
; Most of the software serial libraries won't compile.
; Use the SERCOM's; they're better anyway
lib_ignore = ${common.lib_ignore}
            SoftwareSerial_ExtInts
            AltSoftSerial
;  Use the src filter to ensure subfolders are built
src_filter =
    +<*> +<../../src> +<../../src/sensors> +<../../src/publishers>
build_flags =
    -Isrc/sensors
    -DSDI12_EXTERNAL_PCINT
    -DMQTT_MAX_PACKET_SIZE=240
```

While you're working on development, there is *extensive* debugging text built into this library.
More on that is in the [Code Debugging](https://github.com/EnviroDIY/ModularSensors/wiki/Code-Debugging) page.
In fact, there is _so much_ debugging that turning it on universally through a build flag will cause the program to be too big to fit on a Mayfly and will likely crash a SAMD board's on-board USB drivers.

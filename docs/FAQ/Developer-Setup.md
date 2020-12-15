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
; Pick your default environment, if you don't want to build all of them every time
default_envs = mayfly
; Whatever code you personally want to test from
src_dir = your_directory/your_source_code

[env]
; Default baud for all examples
monitor_speed = 115200
framework = arduino
; To run code checks; cppcheck and clangtidy must be installed
check_tool = cppcheck, clangtidy
check_patterns =
	src
	tools
	examples
check_flags =
	cppcheck: --enable=all, --inline-suppr
	clangtidy: --checks=-*
; deep search for dependencies, evalulating preprocessor conditionals
lib_ldf_mode = deep+
; look for the library director
lib_extra_dirs = .
; We have to ignore these folders or PlatformIO will double count all the dependencies
lib_ignore = .git, .pio, .vscode, include, doc, examples, tools
; All these library dependencies must be listed out since we're in the library
; source code and won't read the dependencies from the library.json like a
; typical user would
lib_deps =
	envirodiy/EnviroDIY_DS3231
	arduino-libraries/RTCZero
	greygnome/EnableInterrupt
	greiman/SdFat
	vshymanskyy/TinyGSM
	knolleary/PubSubClient
	adafruit/Adafruit BusIO
	adafruit/Adafruit Unified Sensor
	https://github.com/soligen2010/Adafruit_ADS1X15.git
	adafruit/Adafruit AM2315
	adafruit/Adafruit BME280 Library
	adafruit/DHT sensor library
	adafruit/Adafruit INA219
	adafruit/Adafruit MPL115A2
	paulstoffregen/OneWire
	milesburton/DallasTemperature
	envirodiy/SDI-12
	northernwidget/MS5803
	https://github.com/NorthernWidget-Skunkworks/Tally_Library.git#Dev_I2C
	envirodiy/SensorModbusMaster
	envirodiy/KellerModbus
	envirodiy/YosemitechModbus
	vshymanskyy/StreamDebugger
; The directories for the source code
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
	-D SDI12_EXTERNAL_PCINT
	-D NEOSWSERIAL_EXTERNAL_PCINT
	-D MQTT_MAX_PACKET_SIZE=240
	-D TINY_GSM_RX_BUFFER=64
	-D TINY_GSM_YIELD_MS=2

[env:mayfly]
upload_port = COM##
monitor_port = COM##
board = mayfly
platform = atmelavr
framework = arduino
; You probably need some software serial libraries
lib_deps = ${env.lib_deps}
           https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git
           https://github.com/PaulStoffregen/AltSoftSerial.git
           https://github.com/SRGDamia1/NeoSWSerial.git
; AVR boards need to ignore RTCZero, it's for SAMD only and will not compile for AVR
lib_ignore = ${env.lib_ignore}, RTCZero


[env:adafruit_feather_m0]
upload_port = COM##
monitor_port = COM##
platform = atmelsam
board = adafruit_feather_m0
framework = arduino
; SAMD boards need RTCZero for the real time clock and sleeping
lib_deps = ${env.lib_deps}
           RTCZero
; Most of the software serial libraries won't compile.
; Use the SERCOM's; they're better anyway
lib_ignore = ${env.lib_ignore}
            SoftwareSerial_ExtInts
            AltSoftSerial
            NeoSWSerial
```

While you're working on development, there is *extensive* debugging text built into this library.
More on that is in the [Code Debugging](https://github.com/EnviroDIY/ModularSensors/wiki/Code-Debugging) page.
In fact, there is _so much_ debugging that turning it on universally through a build flag will cause the program to be too big to fit on a Mayfly and will likely crash a SAMD board's on-board USB drivers.

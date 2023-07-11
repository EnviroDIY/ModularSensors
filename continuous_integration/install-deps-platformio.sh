#!/bin/bash

# Makes the bash script print out every command before it is executed, except echo
trap '[[ $BASH_COMMAND != echo* ]] && echo $BASH_COMMAND' DEBUG

# Exit with nonzero exit code if anything fails
set -e

echo "::group::Installing Platforms and Frameworks"
echo "\e[32mInstalling Atmel AVR platforms \e[0m"
pio pkg install -g --platform atmelavr
pio pkg install -g --tool framework-arduino-avr
pio pkg install -g --tool tool-avrdude
pio pkg install -g --tool toolchain-atmelavr

echo "\e[32mInstalling Atmel AVR framework \e[0m"
pio pkg install -g --platform atmelmegaavr
pio pkg install -g --tool framework-arduino-megaavr

echo "\e[32mInstalling Atmel SAM platform \e[0m"
pio pkg install -g --platform atmelsam

echo "\e[32mInstalling Atmel SAM framework \e[0m"
pio pkg install -g --tool framework-arduino-samd
pio pkg install -g --tool framework-arduino-samd-adafruit
pio pkg install -g --tool framework-arduino-samd-sodaq
pio pkg install -g --tool framework-cmsis
pio pkg install -g --tool framework-cmsis-atmel
pio pkg install -g --tool tool-bossac
pio pkg install -g --tool toolchain-gccarmnoneeabi
echo "::endgroup::"


echo "::group::Installing Libraries"
echo "\e[32m\nCurrently installed packages:\e[0m"
pio pkg list -g -v

echo "\e[32mInstalling envirodiy/EnviroDIY_DS3231\e[0m"
pio pkg install -g --library envirodiy/EnviroDIY_DS3231

echo "\e[32mInstalling arduino-libraries/RTCZero\e[0m"
pio pkg install -g --library arduino-libraries/RTCZero

echo "\e[32mInstalling greygnome/EnableInterrupt\e[0m"
pio pkg install -g --library greygnome/EnableInterrupt

echo "\e[32mInstalling greiman/SdFat\e[0m"
pio pkg install -g --library greiman/SdFat

echo "\e[32mInstalling vshymanskyy/TinyGSM\e[0m"
pio pkg install -g --library vshymanskyy/TinyGSM

echo "\e[32mInstalling knolleary/PubSubClient\e[0m"
pio pkg install -g --library knolleary/PubSubClient

echo "\e[32mInstalling adafruit/'Adafruit BusIO'\e[0m"
pio pkg install -g --library adafruit/'Adafruit BusIO'

echo "\e[32mInstalling adafruit/'Adafruit Unified Sensor'\e[0m"
pio pkg install -g --library adafruit/'Adafruit Unified Sensor'

echo "\e[32mInstalling https://github.com/soligen2010/Adafruit_ADS1X15.git\e[0m"
pio pkg install -g --library https://github.com/soligen2010/Adafruit_ADS1X15.git

echo "\e[32mInstalling adafruit/'Adafruit AM2315'\e[0m"
pio pkg install -g --library adafruit/'Adafruit AM2315'

echo "\e[32mInstalling adafruit/'Adafruit BME280 Library'\e[0m"
pio pkg install -g --library adafruit/'Adafruit BME280 Library'

echo "\e[32mInstalling adafruit/'DHT sensor library'\e[0m"
pio pkg install -g --library adafruit/'DHT sensor library'

echo "\e[32mInstalling adafruit/'Adafruit INA219'\e[0m"
pio pkg install -g --library adafruit/'Adafruit INA219'

echo "\e[32mInstalling adafruit/'Adafruit MPL115A2'\e[0m"
pio pkg install -g --library adafruit/'Adafruit MPL115A2'

echo "\e[32mInstalling adafruit/'Adafruit SHT'\e[0m"
pio pkg install -g --library adafruit/'Adafruit SHT4x Library'

echo "\e[32mInstalling Martin Lindupp's BMP388 Library\e[0m"
pio pkg install -g --library https://github.com/MartinL1/BMP388_DEV.git

echo "\e[32mInstalling paulstoffregen/OneWire\e[0m"
pio pkg install -g --library paulstoffregen/OneWire

echo "\e[32mInstalling milesburton/DallasTemperature\e[0m"
pio pkg install -g --library milesburton/DallasTemperature

echo "\e[32mInstalling envirodiy/SDI-12\e[0m"
pio pkg install -g --library envirodiy/SDI-12

echo "\e[32mInstalling northernwidget/MS5803\e[0m"
pio pkg install -g --library northernwidget/MS5803

echo "\e[32mInstalling https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C\e[0m"
pio pkg install -g --library https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C

echo "\e[32mInstalling envirodiy/SensorModbusMaster\e[0m"
pio pkg install -g --library envirodiy/SensorModbusMaster

echo "\e[32mInstalling envirodiy/KellerModbus\e[0m"
pio pkg install -g --library envirodiy/KellerModbus

echo "\e[32mInstalling envirodiy/YosemitechModbus\e[0m"
pio pkg install -g --library envirodiy/YosemitechModbus

echo "\e[32mInstalling envirodiy/GroPointModbus\e[0m"
pio pkg install -g --library envirodiy/GroPointModbus

echo "\e[32mInstalling vshymanskyy/StreamDebugger\e[0m"
pio pkg install -g --library vshymanskyy/StreamDebugger

echo "\e[32mInstalling https://github.com/PaulStoffregen/AltSoftSerial.git\e[0m"
pio pkg install -g --library https://github.com/PaulStoffregen/AltSoftSerial.git

echo "\e[32mInstalling https://github.com/Testato/SoftwareWire.git#v1.5.1\e[0m"
pio pkg install -g --library https://github.com/Testato/SoftwareWire.git#v1.5.1

echo "\e[32mInstalling https://github.com/SRGDamia1/NeoSWSerial.git\e[0m"
pio pkg install -g --library https://github.com/SRGDamia1/NeoSWSerial.git

echo "\e[32mInstalling https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git\e[0m"
pio pkg install -g --library https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git
echo "::endgroup::"


echo "::group::Current globally installed packages"
echo "\e[32m\nCurrently installed packages:\e[0m"
pio pkg list -g -v
echo "::endgroup::"

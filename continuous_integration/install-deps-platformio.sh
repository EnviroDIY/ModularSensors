#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

echo "\e[32mInstalling envirodiy/EnviroDIY_DS3231\e[0m"
pio lib -g install envirodiy/EnviroDIY_DS3231

echo "\e[32mInstalling arduino-libraries/RTCZero\e[0m"
pio lib -g install arduino-libraries/RTCZero

echo "\e[32mInstalling greygnome/EnableInterrupt\e[0m"
pio lib -g install greygnome/EnableInterrupt

echo "\e[32mInstalling greiman/SdFat\e[0m"
pio lib -g install greiman/SdFat

echo "\e[32mInstalling vshymanskyy/TinyGSM\e[0m"
pio lib -g install vshymanskyy/TinyGSM

echo "\e[32mInstalling knolleary/PubSubClient\e[0m"
pio lib -g install knolleary/PubSubClient

echo "\e[32mInstalling adafruit/'Adafruit BusIO'\e[0m"
pio lib -g install adafruit/'Adafruit BusIO'

echo "\e[32mInstalling adafruit/'Adafruit Unified Sensor'\e[0m"
pio lib -g install adafruit/'Adafruit Unified Sensor'

echo "\e[32mInstalling https://github.com/soligen2010/Adafruit_ADS1X15.git\e[0m"
pio lib -g install https://github.com/soligen2010/Adafruit_ADS1X15.git

echo "\e[32mInstalling adafruit/'Adafruit AM2315'\e[0m"
pio lib -g install adafruit/'Adafruit AM2315'

echo "\e[32mInstalling adafruit/'Adafruit BME280 Library'\e[0m"
pio lib -g install adafruit/'Adafruit BME280 Library'

echo "\e[32mInstalling adafruit/'DHT sensor library'\e[0m"
pio lib -g install adafruit/'DHT sensor library'

echo "\e[32mInstalling adafruit/'Adafruit INA219'\e[0m"
pio lib -g install adafruit/'Adafruit INA219'

echo "\e[32mInstalling adafruit/'Adafruit MPL115A2'\e[0m"
pio lib -g install adafruit/'Adafruit MPL115A2'

echo "\e[32mInstalling paulstoffregen/OneWire\e[0m"
pio lib -g install paulstoffregen/OneWire

echo "\e[32mInstalling milesburton/DallasTemperature\e[0m"
pio lib -g install milesburton/DallasTemperature

echo "\e[32mInstalling envirodiy/SDI-12\e[0m"
pio lib -g install envirodiy/SDI-12

echo "\e[32mInstalling northernwidget/MS5803\e[0m"
pio lib -g install northernwidget/MS5803

echo "\e[32mInstalling https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C\e[0m"
pio lib -g install https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C

echo "\e[32mInstalling envirodiy/SensorModbusMaster\e[0m"
pio lib -g install envirodiy/SensorModbusMaster

echo "\e[32mInstalling envirodiy/KellerModbus\e[0m"
pio lib -g install envirodiy/KellerModbus

echo "\e[32mInstalling envirodiy/YosemitechModbus\e[0m"
pio lib -g install envirodiy/YosemitechModbus

echo "\e[32mInstalling vshymanskyy/StreamDebugger\e[0m"
pio lib -g install vshymanskyy/StreamDebugger

echo "\e[32mInstalling https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git\e[0m"
pio lib -g install https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git

echo "\e[32mInstalling https://github.com/PaulStoffregen/AltSoftSerial.git\e[0m"
pio lib -g install https://github.com/PaulStoffregen/AltSoftSerial.git

echo "\e[32mInstalling https://github.com/SRGDamia1/NeoSWSerial.git\e[0m"
pio lib -g install https://github.com/SRGDamia1/NeoSWSerial.git

echo "\e[32mInstalling https://github.com/Testato/SoftwareWire.git#v1.5.1\e[0m"
pio lib -g install https://github.com/Testato/SoftwareWire.git#v1.5.1

echo "\e[32m\nCurrently installed libraries:\e[0m"
pio lib -g list
pio lib list
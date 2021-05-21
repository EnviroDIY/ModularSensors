#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

echo "::debug::Installing envirodiy/EnviroDIY_DS3231"
pio lib -g install envirodiy/EnviroDIY_DS3231

echo "::debug::Installing arduino-libraries/RTCZero"
pio lib -g install arduino-libraries/RTCZero

echo "::debug::Installing greygnome/EnableInterrupt"
pio lib -g install greygnome/EnableInterrupt

echo "::debug::Installing greiman/SdFat"
pio lib -g install greiman/SdFat

echo "::debug::Installing vshymanskyy/TinyGSM"
pio lib -g install vshymanskyy/TinyGSM

echo "::debug::Installing knolleary/PubSubClient"
pio lib -g install knolleary/PubSubClient

echo "::debug::Installing adafruit/'Adafruit BusIO'"
pio lib -g install adafruit/'Adafruit BusIO'

echo "::debug::Installing adafruit/'Adafruit Unified Sensor'"
pio lib -g install adafruit/'Adafruit Unified Sensor'

echo "::debug::Installing https://github.com/soligen2010/Adafruit_ADS1X15.git"
pio lib -g install https://github.com/soligen2010/Adafruit_ADS1X15.git

echo "::debug::Installing adafruit/'Adafruit AM2315'"
pio lib -g install adafruit/'Adafruit AM2315'

echo "::debug::Installing adafruit/'Adafruit BME280 Library'"
pio lib -g install adafruit/'Adafruit BME280 Library'

echo "::debug::Installing adafruit/'DHT sensor library'"
pio lib -g install adafruit/'DHT sensor library'

echo "::debug::Installing adafruit/'Adafruit INA219'"
pio lib -g install adafruit/'Adafruit INA219'

echo "::debug::Installing adafruit/'Adafruit MPL115A2'"
pio lib -g install adafruit/'Adafruit MPL115A2'

echo "::debug::Installing paulstoffregen/OneWire"
pio lib -g install paulstoffregen/OneWire

echo "::debug::Installing milesburton/DallasTemperature"
pio lib -g install milesburton/DallasTemperature

echo "::debug::Installing envirodiy/SDI-12"
pio lib -g install envirodiy/SDI-12

echo "::debug::Installing northernwidget/MS5803"
pio lib -g install northernwidget/MS5803

echo "::debug::Installing https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C"
pio lib -g install https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C

echo "::debug::Installing envirodiy/SensorModbusMaster"
pio lib -g install envirodiy/SensorModbusMaster

echo "::debug::Installing envirodiy/KellerModbus"
pio lib -g install envirodiy/KellerModbus

echo "::debug::Installing envirodiy/YosemitechModbus"
pio lib -g install envirodiy/YosemitechModbus

echo "::debug::Installing vshymanskyy/StreamDebugger"
pio lib -g install vshymanskyy/StreamDebugger

echo "::debug::Installing https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git"
pio lib -g install https://github.com/EnviroDIY/SoftwareSerial_ExternalInts.git

echo "::debug::Installing https://github.com/PaulStoffregen/AltSoftSerial.git"
pio lib -g install https://github.com/PaulStoffregen/AltSoftSerial.git

echo "::debug::Installing https://github.com/SRGDamia1/NeoSWSerial.git"
pio lib -g install https://github.com/SRGDamia1/NeoSWSerial.git

echo "::debug::Installing https://github.com/Testato/SoftwareWire.git"
pio lib -g install https://github.com/Testato/SoftwareWire.git

echo "\nCurrently installed libraries:"
lib -g list
lib list
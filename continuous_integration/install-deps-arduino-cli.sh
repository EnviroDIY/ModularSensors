#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

echo "\n\e[32mCurrent Arduino CLI version:\e[0m"
arduino-cli version

echo "\n\e[32mUpdating the core index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core update-index

echo "\n\e[32mInstalling the Arduino AVR Core\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install arduino:avr

echo "\n\e[32mInstalling the EnviroDIY AVR Core\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install EnviroDIY:avr

echo "\n\e[32mInstalling the Arduino SAMD Core\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install arduino:samd

echo "\n\e[32mInstalling the Adafruit SAMD Core\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install adafruit:samd

echo "\n\e[32mInstalling the Sodaq SAMD Core\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install SODAQ:samd

echo "\n\e[32mUpdating the core index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core update-index

echo "\n\e[32mUpgrading all cores\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core upgrade

echo "\n\e[32mCurrently installed cores:\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core list

echo "\n\e[32mUpdating the library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index

echo "\n\e[32mInstalling EnviroDIY DS3231 library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnviroDIY_DS3231

echo "\n\e[32mInstalling RTCZero library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install RTCZero

echo "\n\e[32mInstalling EnableInterrupt library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnableInterrupt

echo "\n\e[32mInstalling SdFat library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SdFat

echo "\n\e[32mInstalling TinyGSM library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install TinyGSM

echo "\n\e[32mInstalling PubSubClient library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install PubSubClient

echo "\n\e[32mInstalling Adafruit BusIO library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BusIO"

echo "\n\e[32mInstalling Adafruit Unified Sensor library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit Unified Sensor"

echo "\n\e[32mDownloading Soligen fork of Adafruit_ADS1X15 as a zip"
# Soligen fork needs to be manually unzipped and moved because the CLI chokes on the library name not matching the h file
curl -L  --retry 15 --retry-delay 0 https://github.com/soligen2010/Adafruit_ADS1X15/archive/master.zip --create-dirs -o home/arduino/downloads/Adafruit_ADS1X15.zip
echo "\e[32mDecompressing Adafruit_ADS1X15\e[0m"
unzip -q -o home/arduino/downloads/Adafruit_ADS1X15.zip -d home/arduino/downloads/
echo "\e[32mMoving Adafruit_ADS1X15 to the libraries folder\e[0m"
mkdir -p home/arduino/user/libraries/Adafruit_ADS1X15
mv home/arduino/downloads/Adafruit_ADS1X15-master/* home/arduino/user/libraries/Adafruit_ADS1X15

echo "\n\e[32mInstalling Adafruit AM2315 library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit AM2315"

echo "\n\e[32mInstalling Adafruit BME280 Library library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BME280 Library"

echo "\n\e[32mInstalling DHT sensor library library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "DHT sensor library"

echo "\n\e[32mInstalling Adafruit INA219 library from Arduino library index - excluding dependencies\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit INA219" --no-deps

echo "\n\e[32mInstalling Adafruit MPL115A2 library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit MPL115A2"

echo "\n\e[32mInstalling Adafruit SHT4x Library library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit SHT4x Library"

echo "\n\e[32mInstalling OneWire library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install OneWire

echo "\n\e[32mInstalling DallasTemperature library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install DallasTemperature

echo "\n\e[32mInstalling SDI-12 library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SDI-12

echo "\n\e[32mDownloading External Interrupt version of the SDI-12 library as a zip\e[0m"
# The "external interrupt" version needs to be installed from a zip because the Arduino CLI cannot pull from a branch
curl -L  --retry 15 --retry-delay 0 https://github.com/EnviroDIY/Arduino-SDI-12/archive/refs/heads/ExtInts.zip --create-dirs -o home/arduino/downloads/EnviroDIY_SDI12_ExtInts.zip
echo "\e[32mDecompressing EnviroDIY_SDI12_ExtInts\e[0m"
unzip -q -o home/arduino/downloads/EnviroDIY_SDI12_ExtInts.zip -d home/arduino/downloads/
echo "\e[32mMoving EnviroDIY_SDI12_ExtInts to the libraries folder\e[0m"
mkdir -p home/arduino/user/libraries/EnviroDIY_SDI12_ExtInts
mv home/arduino/downloads/Arduino-SDI-12-ExtInts/* home/arduino/user/libraries/EnviroDIY_SDI12_ExtInts

echo "\n\e[32mInstalling NorthernWidget MS5803 library from GitHub\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/NorthernWidget/MS5803.git

echo "\n\e[32mInstalling EnviroDIY Tally_Library library from GitHub"
# NOTE:  This only works because the DEV_I2C branch is the main branch of the EnviroDIY fork
# The Arduino CLI can only install from whatever is assigned as the default branch on GitHub
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/EnviroDIY/Tally_Library.git

echo "\n\e[32mInstalling EnviroDIY SensorModbusMaster library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SensorModbusMaster

echo "\n\e[32mInstalling EnviroDIY KellerModbus library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install KellerModbus

echo "\n\e[32mInstalling EnviroDIY YosemitechModbus library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install YosemitechModbus

echo "\n\e[32mInstalling StreamDebugger library from Arduino library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install StreamDebugger

echo "\n\e[32mInstalling AltSoftSerial library from GitHub\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/PaulStoffregen/AltSoftSerial.git

echo "\n\e[32mInstalling SoftwareWire library from GitHub\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/Testato/SoftwareWire.git#v1.5.1

echo "\n\e[32mInstalling NeoSWSerial library from GitHub\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/SRGDamia1/NeoSWSerial.git

echo "\n\e[32mDownloading SoftwareSerial with External Interrupts as a zip\e[0m"
# SoftwareSerial with External Interrupts needs to be manually unzipped and moved because the CLI chokes on the library name not matching the h file
curl -L  --retry 15 --retry-delay 0 https://github.com/EnviroDIY/SoftwareSerial_ExternalInts/archive/master.zip --create-dirs -o home/arduino/downloads/SoftwareSerial_ExternalInts.zip
echo "\e[32mDecompressing SoftwareSerial_ExternalInts\e[0m"
unzip -q -o home/arduino/downloads/SoftwareSerial_ExternalInts.zip -d home/arduino/downloads/
echo "\e[32mMoving SoftwareSerial_ExternalInts to the libraries folder\e[0m"
mkdir -p home/arduino/user/libraries/SoftwareSerial_ExternalInts
mv home/arduino/downloads/SoftwareSerial_ExtInts-master/* home/arduino/user/libraries/SoftwareSerial_ExternalInts

echo "\n\e[32mCurrently installed libraries:\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib list
#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

echo "\nCurrent Arduino CLI version:"
arduino-cli version

echo "\nUpdating the core index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core update-index

echo "\nInstalling the Arduino AVR Core"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install arduino:avr

echo "\nInstalling the EnviroDIY AVR Core"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install EnviroDIY:avr

echo "\nInstalling the Arduino SAMD Core"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install arduino:samd

echo "\nInstalling the Adafruit SAMD Core"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install adafruit:samd

echo "\nInstalling the Sodaq SAMD Core"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core install SODAQ:samd

echo "\nUpdating the core index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core update-index

echo "\nUpgrading all cores"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core upgrade

echo "\nCurrently installed cores:"
arduino-cli --config-file continuous_integration/arduino_cli.yaml core list

echo "\nUpdating the library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index

echo "\nInstalling EnviroDIY DS3231 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnviroDIY_DS3231

echo "\nInstalling RTCZero library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install RTCZero

echo "\nInstalling EnableInterrupt library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnableInterrupt

echo "\nInstalling SdFat library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SdFat

echo "\nInstalling TinyGSM library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install TinyGSM

echo "\nInstalling PubSubClient library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install PubSubClient

echo "\nInstalling Adafruit BusIO library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BusIO"

echo "\nInstalling Adafruit Unified Sensor library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit Unified Sensor"

echo "\nDownloading Soligen fork of Adafruit_ADS1X15 as a zip"
# Soligen fork needs to be manually unzipped and moved because the CLI chokes on the library name not matching the h file
curl -L  --retry 5 --retry-delay 0 https://github.com/soligen2010/Adafruit_ADS1X15/archive/master.zip --create-dirs -o home/arduino/downloads/Adafruit_ADS1X15.zip
echo "Decompressing Adafruit_ADS1X15"
unzip -q -o home/arduino/downloads/Adafruit_ADS1X15.zip -d home/arduino/downloads/
echo "Moving Adafruit_ADS1X15 to the libraries folder"
mkdir -p home/arduino/user/libraries/Adafruit_ADS1X15
mv home/arduino/downloads/Adafruit_ADS1X15-master/* home/arduino/user/libraries/Adafruit_ADS1X15

echo "\nInstalling Adafruit AM2315 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit AM2315"

echo "\nInstalling Adafruit BME280 Library library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BME280 Library"

echo "\nInstalling DHT sensor library library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "DHT sensor library"

echo "\nInstalling Adafruit INA219 library from Arduino library index - excluding dependencies"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit INA219" --no-deps

echo "\nInstalling Adafruit MPL115A2 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit MPL115A2"

echo "\nInstalling OneWire library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install OneWire

echo "\nInstalling DallasTemperature library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install DallasTemperature

echo "\nInstalling SDI-12 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SDI-12

echo "\nDownloading External Interrupt version of the SDI-12 library as a zip"
# The "external interrupt" version needs to be installed from a zip because the Arduino CLI cannot pull from a branch
curl -L  --retry 5 --retry-delay 0 https://github.com/EnviroDIY/Arduino-SDI-12/archive/refs/heads/ExtInts.zip --create-dirs -o home/arduino/downloads/EnviroDIY_SDI12_ExtInts.zip
echo "Decompressing EnviroDIY_SDI12_ExtInts"
unzip -q -o home/arduino/downloads/EnviroDIY_SDI12_ExtInts.zip -d home/arduino/downloads/
echo "Moving EnviroDIY_SDI12_ExtInts to the libraries folder"
mkdir -p home/arduino/user/libraries/EnviroDIY_SDI12_ExtInts
mv home/arduino/downloads/Arduino-SDI-12-ExtInts/* home/arduino/user/libraries/EnviroDIY_SDI12_ExtInts

echo "\nInstalling NorthernWidget MS5803 library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/NorthernWidget/MS5803.git

echo "\nInstalling EnviroDIY Tally_Library library from GitHub"
# NOTE:  This only works because the DEV_I2C branch is the main branch of the EnviroDIY fork
# The Arduino CLI can only install from whatever is assigned as the default branch on GitHub
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/EnviroDIY/Tally_Library.git

echo "\nInstalling EnviroDIY SensorModbusMaster library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SensorModbusMaster

echo "\nInstalling EnviroDIY KellerModbus library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install KellerModbus

echo "\nInstalling EnviroDIY YosemitechModbus library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install YosemitechModbus

echo "\nInstalling StreamDebugger library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install StreamDebugger

echo "\nInstalling AltSoftSerial library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/PaulStoffregen/AltSoftSerial.git

echo "\nInstalling SoftwareWire library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/Testato/SoftwareWire.git

echo "\nInstalling NeoSWSerial library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/SRGDamia1/NeoSWSerial.git

echo "\nDownloading SoftwareSerial with External Interrupts as a zip"
# SoftwareSerial with External Interrupts needs to be manually unzipped and moved because the CLI chokes on the library name not matching the h file
curl -L  --retry 5 --retry-delay 0 https://github.com/EnviroDIY/SoftwareSerial_ExternalInts/archive/master.zip --create-dirs -o home/arduino/downloads/SoftwareSerial_ExternalInts.zip
echo "Decompressing SoftwareSerial_ExternalInts"
unzip -q -o home/arduino/downloads/SoftwareSerial_ExternalInts.zip -d home/arduino/downloads/
echo "Moving SoftwareSerial_ExternalInts to the libraries folder"
mkdir -p home/arduino/user/libraries/SoftwareSerial_ExternalInts
mv home/arduino/downloads/SoftwareSerial_ExtInts-master/* home/arduino/user/libraries/SoftwareSerial_ExternalInts

echo "\nCurrently installed libraries:"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib list
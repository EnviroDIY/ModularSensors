#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

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

echo "\nDownloading Soligen fork of Adafruit_ADS1X15 as a tarball"
# Soligen fork needs to be manually unzipped and moved because the CLI chokes on the library name not matching the h file
curl -L https://github.com/soligen2010/Adafruit_ADS1X15/archive/master.zip --create-dirs -o ~/arduino/downloads/Adafruit_ADS1X15.zip
echo "Decompressing Adafruit_ADS1X15"
unzip -o ~/arduino/downloads/Adafruit_ADS1X15.zip -d ~/arduino/downloads/
echo "Moving Adafruit_ADS1X15 to the libraries folder"
mkdir -p ~/arduino/user/libraries/Adafruit_ADS1X15
mv ~/arduino/downloads/Adafruit_ADS1X15-master/* ~/arduino/user/libraries/Adafruit_ADS1X15

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

echo "\nInstalling NorthernWidget MS5803 library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/NorthernWidget/MS5803.git

echo "\nInstalling EnviroDIY Tally_Library library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C

echo "\nInstalling EnviroDIY SensorModbusMaster library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SensorModbusMaster

echo "\nInstalling EnviroDIY KellerModbus library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install KellerModbus

echo "\nInstalling EnviroDIY YosemitechModbus library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install YosemitechModbus

echo "\nInstalling StreamDebugger library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install StreamDebugger
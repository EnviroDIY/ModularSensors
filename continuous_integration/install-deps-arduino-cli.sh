#!/bin/sh

echo "Updating the library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index

echo "Installing EnviroDIY DS3231 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnviroDIY_DS3231

echo "Installing RTCZero library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install RTCZero

echo "Installing EnableInterrupt library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnableInterrupt

echo "Installing SdFat library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SdFat

echo "Installing TinyGSM library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install TinyGSM

echo "Installing PubSubClient library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install PubSubClient

echo "Installing Adafruit BusIO library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BusIO"

echo "Installing Adafruit Unified Sensor library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit Unified Sensor"

echo "Downloading Soligen fork of Adafruit_ADS1X15 as a zip"
# Soligen fork needs to be manually unzipped because the CLI chokes on the library name not matching the h file
curl -L https://github.com/soligen2010/Adafruit_ADS1X15/archive/master.zip --create-dirs -o ~/arduino/downloads/Adafruit_ADS1X15.zip
echo "Decompressing Adafruit_ADS1X15"
upzip ~/arduino/downloads/Adafruit_ADS1X15.zip
echo "Moving Adafruit_ADS1X15 to the libraries folder"
mv ~/arduino/downloads/Adafruit_ADS1X15/Adafruit_ADS1X15-master/* ~/arduino/user/libraries/Adafruit_ADS1X15

echo "Installing Adafruit AM2315 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit AM2315"

echo "Installing Adafruit BME280 Library library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BME280 Library"

echo "Installing DHT sensor library library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "DHT sensor library"

echo "Installing Adafruit INA219 library from Arduino library index - excluding dependencies"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit INA219" --no-deps

echo "Installing Adafruit MPL115A2 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit MPL115A2"

echo "Installing OneWire library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install OneWire

echo "Installing DallasTemperature library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install DallasTemperature

echo "Installing SDI-12 library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SDI-12

echo "Installing NorthernWidget MS5803 library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/NorthernWidget/MS5803.git

echo "Installing EnviroDIY Tally_Library library from GitHub"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C

echo "Installing EnviroDIY SensorModbusMaster library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SensorModbusMaster

echo "Installing EnviroDIY KellerModbus library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install KellerModbus

echo "Installing EnviroDIY YosemitechModbus library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install YosemitechModbus

echo "Installing StreamDebugger library from Arduino library index"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install StreamDebugger
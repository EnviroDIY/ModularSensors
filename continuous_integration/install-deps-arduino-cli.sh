#!/bin/sh

arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnviroDIY_DS3231
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install RTCZero
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install EnableInterrupt
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SdFat
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install TinyGSM
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install PubSubClient
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BusIO"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit Unified Sensor"
# Soligen fork needs to be manually unzipped because the CLI chokes on the library name not matching the h file
curl -L https://github.com/soligen2010/Adafruit_ADS1X15/archive/master.zip -o ~/arduino/downloads/Adafruit_ADS1X15.zip
upzip ~/arduino/downloads/Adafruit_ADS1X15.zip
mv /~/arduino/downloads/Adafruit_ADS1X15/Adafruit_ADS1X15-master/* ~/arduino/user/libraries/Adafruit_ADS1X15
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --zip-file ~/arduino/downloads/Adafruit_ADS1X15.zip
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit AM2315"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit BME280 Library"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "DHT sensor library"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit INA219" --no-deps
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install "Adafruit MPL115A2"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install OneWire
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install DallasTemperature
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SDI-12
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/NorthernWidget/MS5803.git
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install --git-url https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install SensorModbusMaster
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install KellerModbus
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install YosemitechModbus
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib install StreamDebugger
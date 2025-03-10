# Library Dependencies<!--! {#page_library_dependencies} -->

> [!WARNING]
> This page is frequently out of date. Please see the library.json or dependencies.json and example_dependencies.json for the most up-to-date library references!

In order to support multiple functions and sensors, there are quite a lot of sub-libraries that this library depends on.
_Even if you do not use the modules, you must have all of the dependencies installed for the library itself to properly compile._
Please check the [library.json](https://github.com/EnviroDIY/ModularSensors/blob/master/library.json) file for more details on the versions required of each library.
If you are using [PlatformIO](https://platformio.org), you can list "EnviroDIY_ModularSensors" in the `lib_deps` section of your platformio.ini file and all of these libraries will be installed automatically.
If using the "standard" Arduino IDE, you must install each of these libraries individually, or in a bundle from the [EnviroDIY Libraries](https://github.com/EnviroDIY/Libraries) meta-repository.

- [EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt) - Administrates and handles pin change interrupts, allowing the logger to sleep and save battery.
This also controls the interrupts for the versions of SoftwareSerial and SDI-12 linked below that have been stripped of interrupt control.
Because we use this library, _you must always add the line `#include <EnableInterrupt.h>` to the top of your sketch._
- AVR sleep library - This is for low power sleeping for AVR processors.
(This library is built in to the Arduino and PlatformIO IDEs.)
- [EnviroDIY DS-3231](https://github.com/EnviroDIY/Sodaq_DS3231) - For real time clock control
- [RTCZero library](https://github.com/arduino-libraries/RTCZero) - This real time clock control and low power sleeping on SAMD processors.
(This library may be built in to the Arduino IDE.)
NOTE:  If using an AVR board, you must explicitly _ignore_ this library when compiling with PlatformIO or you will have compiler errors.
- [SdFat library](https://github.com/greiman/SdFat) - This enables communication with the SD card.
- [TinyGSM library](https://github.com/vshymanskyy/TinyGSM) - This provides internet (TCP/IP) connectivity.
- [PubSubClient](https://github.com/knolleary/pubsubclient) - For MQTT connectivity
- [Adafruit ADS1X15 library](https://github.com/adafruit/Adafruit_ADS1X15) - For high-resolution analog to digital conversion.
  - NOTE: As of version 0.36.0 the standard Adafruit library should be used, *NOT* the Soligen2010 fork.
- [EnviroDIY Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12/tree/ExtInts) - For control of SDI-12 based sensors.
This modified version is needed so there are no pin change interrupt conflicts with the SoftwareSerial library or the software pin change interrupt library used to wake the processor.
- [SensorModbusMaster](https://github.com/EnviroDIY/SensorModbusMaster) - for easy communication with Modbus devices.
- [OneWire](https://github.com/PaulStoffregen/OneWire) - This enables communication with Maxim/Dallas OneWire devices.
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) - for communication with the DS18 line of Maxim/Dallas OneWire temperature probes.
- [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO) - a dependency of several other Adafruit libraries, used to unify commands for fetching data via SPI and I2C.
- [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor) - a dependency of several other Adafruit libraries, used to unify sensor data return types.
- [Adafruit AM2315 library](https://github.com/adafruit/Adafruit_AM2315) - for the AOSong AM2315 temperature and humidity sensor.
- [Adafruit DHT library](https://github.com/adafruit/DHT-sensor-library) - for other AOSong temperature and humidity sensors.
- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library) - for the Bosch BME280 environmental sensor.
- [Adafruit INA219 library](https://github.com/adafruit/Adafruit_INA219) - for the INA219 current/voltage sensor.
- [Adafruit MPL115A2 library](https://github.com/adafruit/Adafruit_MPL115A2) - for the Freescale Semiconductor MPL115A2 barometer.
- [Adafruit SHT4x library](https://github.com/adafruit/Adafruit_SHT4X) - for the Senserion SHT40 temperature and humidity sensor. This sensor is built into the EnviroDIY Mayfly and Stonefly.
- [YosemitechModbus](https://github.com/EnviroDIY/YosemitechModbus) - for all Yosemitech modbus environmental sensors.
- [Northern Widget MS5803 Library](https://github.com/NorthernWidget/MS5803) - for the TE Connectivity MEAS MS5803 pressure sensor
- [EnviroDIY KellerModbus Library](https://github.com/EnviroDIY/KellerModbus) - for all Keller modbus pressure and water level sensors.
- [EnviroDIY GroPointModbus Library](https://github.com/EnviroDIY/GroPointModbus.git) - For GroPoint soil moisture probes.
- [BMP388_DEV](https://registry.platformio.org/libraries/martinl1/BMP388_DEV) - for communication with the Bosch BMP388 barometer
    - WARNING: The repository for this library has been removed from GitHub. The library itself is still available in the PlatformIO and Arduino library registries.
- [Tally Library](https://github.com/EnviroDIY/Tally_Library.git#Dev_I2C) - For the Project Tally Event sensor
    - NOTE: Use the `Dev_I2C` feature branch

[//]: # ( @mainpage ModularSensors )
# ModularSensors
___
[//]: # ( @section mainpage_intro An Introduction )
## An Introduction

Check out the [Just Getting Started](https://envirodiy.github.io/ModularSensors/getting_started.html) section of the documentation!

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly.
The ModularSensors library is specifically designed to support wireless, solar-powered environmental data logging applications, that is, to:
* Retrieve data from many physical sensors;
* Save that data to a SD memory card;
* Transmit that data wirelessly to a web server; and
* Put the processor, sensors and all other peripherals to sleep between readings to conserve power.

The ModularSensors library coordinates these tasks by "wrapping" native sensor libraries into a common interface of functions and returns.
These [wrapper functions](https://en.wikipedia.org/wiki/Wrapper_function) serve to harmonize and simplify the process of iterating through and logging data from a diverse set of sensors and variables.
Using the common sensor and variable interface, the library attempts to optimize measurement timing as much as possible to reduce logger "on-time" and power consumption.

Although this library was written primarily for the [EnviroDIY Mayfly data logger board](https://envirodiy.org/mayfly/), it is also designed to be [compatible with a variety of other Arduino-based boards](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Compatibility) as well.

There is extensive documentation available in the [ModularSensors github pages](https://envirodiy.github.io/ModularSensors/index.html) including details of the class structures and example code.

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [ModularSensors](#modularsensors)
  - [An Introduction](#an-introduction)
  - [Supported Sensors](#supported-sensors)
  - [Data Endpoints](#data-endpoints)
  - [Supported Cellular/Wifi Modules:](#supported-cellularwifi-modules)
  - [Contributing](#contributing)
  - [License](#license)
  - [Acknowledgments](#acknowledgments)
[//]: # ( End GitHub Only )

[//]: # ( @section mainpage_supported_sensors Supported Sensors )
## Supported Sensors

For some generalized information about attaching sensors to an Arduino style board, see the [Sensor Notes page](https://envirodiy.github.io/ModularSensors/sensor_notes_page.html)

- [Processor Metrics: battery voltage, free RAM, sample count](https://envirodiy.github.io/ModularSensors/processor_sensor_page.html)
- [Maxim DS3231: real time clock](https://envirodiy.github.io/ModularSensors/ds3231_page.html)
- [AOSong AM2315: humidity & temperature](https://envirodiy.github.io/ModularSensors/am2315_page.html)
- [AOSong DHT: humidity & temperature](https://envirodiy.github.io/ModularSensors/dht_page.html)
- [Apogee SQ-212: quantum light sensor, via TI ADS1115](https://envirodiy.github.io/ModularSensors/sq212_page.html)
- [Atlas Scientific EZO Sensors](https://envirodiy.github.io/ModularSensors/atlas_page.html)
    - EZO-CO2: Carbon Dioxide and Temperature
    - EZO-DO: Dissolved Oxygen
    - EZO-EC: Conductivity, Total Dissolved Solids, Salinity, and Specific Gravity
    - EZO-ORP: Oxidation/Reduction Potential
    - EZO-pH: pH
    - EZO-RTD: Temperature
- [Bosch BME280: barometric pressure, humidity & temperature](https://envirodiy.github.io/ModularSensors/bme280_page.html)
- [Campbell Scientific OBS-3+: turbidity, via TI ADS1115](https://envirodiy.github.io/ModularSensors/obs3_page.html)
- [Decagon Devices ES-2: conductivity ](https://envirodiy.github.io/ModularSensors/es2_page.html)
- [Freescale Semiconductor MPL115A2: barometric pressure and temperature](https://envirodiy.github.io/ModularSensors/mpl115a2_page.html)
- [External Arduino I2C Rain Tipping Bucket Counter: rainfall totals](https://envirodiy.github.io/ModularSensors/i2c_rain_page.html)
- [Keller Submersible Level Transmitters: pressure and temperature](https://envirodiy.github.io/ModularSensors/keller_page.html)
    - Acculevel
    - Nanolevel
- [MaxBotix MaxSonar: water level](https://envirodiy.github.io/ModularSensors/maxbotics_page.html)
- [Maxim DS18: temperature](https://envirodiy.github.io/ModularSensors/ds18_page.html)
- [Measurement Specialties MS5803: pressure and temperature](https://envirodiy.github.io/ModularSensors/ms5803_page.html)
- [Meter Environmental Soil Moisture Probes: soil Ea and volumetric water content](https://envirodiy.github.io/ModularSensors/meter_soil_page.html)
    - Meter ECH2O 5TM
    - Meter Teros 11
- [Meter Environmental Hydros 21 (formerly Decagon Devices CTD-10): conductivity, temperature & depth](https://envirodiy.github.io/ModularSensors/hydros21_page.html)
- [TI ADS1115: external voltage with support for divided current ](https://envirodiy.github.io/ModularSensors/ads1115_page.html)
- [TI INA219: current, voltage, and power draw](https://envirodiy.github.io/ModularSensors/ina219_page.html)
- [Yosemitech: water quality sensors](https://envirodiy.github.io/ModularSensors/yosemitech_page.html)
    - Y502-A or Y504-A: Optical DO and Temperature
    - Y510-B: Optical Turbidity and Temperature
    - Y511-A: Optical Turbidity and Temperature
    - Y514-A: Optical Chlorophyll and Temperature
    - Y520-A: Conductivity and Temperature
    - Y532-A: Digital pH and Temperature
    - Y533: ORP, pH, and Temperature
    - Y550-B: UV254/COD, Turbidity, and Temperature
    - Y4000 Multiparameter Sonde
- [Zebra-Tech D-Opto: dissolved oxygen](https://envirodiy.github.io/ModularSensors/dopto_page.html)


[//]: # ( @section mainpage_data_receivers Data Endpoints )
## Data Endpoints

Within ModularSensors, the "dataPublisher" objects add the functionality to send data to remote web services.
The currently supported services are the [Monitor My Watershed data portal](http://data.envirodiy.org/) and [ThingSpeak](https://thingspeak.com/).

- [Monitor My Watershed/EnviroDIY Data Portal](https://github.com/EnviroDIY/ModularSensors/wiki/EnviroDIY-Portal-Functions)
- [ThingSpeak](https://github.com/EnviroDIY/ModularSensors/wiki/ThingSpeak-Functions)
[//]: # ( @todo Page on Data Endpoints )


[//]: # ( @section mainpage_modems Supported Cellular/Wifi Modules )
## Supported Cellular/Wifi Modules:

For information common to all modems and for tables of the proper class, baud rate, and pins to uses, see the [Modem Notes page](https://envirodiy.github.io/ModularSensors/modem_notes_page.html).

- [Digi XBee](https://envirodiy.github.io/ModularSensors/digi_xbees.html)
    - Digi XBee® 3 Cellular LTE-M/NB-IoT
    - Digi XBee® 3 Cellular LTE Cat 1 (AT&T or Verizon)
    - Digi XBee® Cellular 3G
    - Digi XBee® Cellular LTE Cat 1 (Verizon)
    - Digi XBee® Wi-Fi (S6B)
- [ESP8266](https://envirodiy.github.io/ModularSensors/esp8266_page.html)
- [QuectelBG96](https://envirodiy.github.io/ModularSensors/bg96_page.html)
- [Sequans Monarch](https://envirodiy.github.io/ModularSensors/monarch_page.html)
- [SIM7000](https://envirodiy.github.io/ModularSensors/sim7000_page.html)
- [SIM800](https://envirodiy.github.io/ModularSensors/sim800_page.html), including the [Sodaq GPRSBee](https://envirodiy.github.io/ModularSensors/gprsbee_page.html)
- u-blox LTE-M R4 and N4 series, including the [Sodaq uBee](https://envirodiy.github.io/ModularSensors/ubee_page.html)
- u-blox 2G, 3G, and 4G, including the [Sodaq 3GBee](https://envirodiy.github.io/ModularSensors/ubee_page.html)



[//]: # ( @section mainpage_contributing Contributing )
## Contributing
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions.
Feel free to open issues about any bugs you find or any sensors you would like to have added.

If you would like to directly help with the coding development of the library, there are some [tips here](https://envirodiy.github.io/ModularSensors/for_developers.html) on how to set up PlatformIO so you can fork the library and test programs while in the library repo.
Please _take time to familiarize yourself with the [terminology, classes and data structures](https://envirodiy.github.io/ModularSensors/library_terminology.html) this library uses_.
This library is built to fully take advantage of Objecting Oriented Programing (OOP) approaches and is larger and more complicated than many Arduino libraries.
There is _extensive_ documentation on our [github pages](https://envirodiy.github.io/ModularSensors/index.html) and an _enormous_ number of comments and debugging printouts in the code itself to help you get going.



[//]: # ( @section mainpage_license License )
## License
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

Hardware designs shared are released, unless otherwise indicated, under the [CERN Open Hardware License 1.2](http://www.ohwr.org/licenses/cern-ohl/v1.2) (CERN_OHL).

[//]: # ( @section mainpage_acknowledgments Acknowledgments )
## Acknowledgments
[EnviroDIY](http://envirodiy.org/)™ is presented by the Stroud Water Research Center, with contributions from a community of enthusiasts sharing do-it-yourself ideas for environmental science and monitoring.

[Sara Damiano](https://github.com/SRGDamia1) is the primary developer of the EnviroDIY ModularSensors library, with input from many [other contributors](https://github.com/EnviroDIY/ModularSensors/graphs/contributors).

This project has benefited from the support from the following funders:

* William Penn Foundation
* US Environmental Protection Agency (EPA)
* National Science Foundation, awards [EAR-0724971](http://www.nsf.gov/awardsearch/showAward?AWD_ID=0724971), [EAR-1331856](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1331856), [ACI-1339834](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1339834)
* Stroud Water Research Center endowment

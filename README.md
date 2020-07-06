[//]: # ( @mainpage ModularSensors )
# ModularSensors
___
[//]: # ( @section mainpage_intro An Introduction )
## An Introduction

Check out the [Just Getting Started](https://envirodiy.github.io/ModularSensors/getting_started.html) section of the documentation!

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly. 
This version of the ModularSensors library is a rugged solar powered wireless data logger, that:
* Based in the riparian corridor
* Polls local physical sensors readings
* Supports 12V/Modbus interface through Wingboard 
* Stores the readings  to a SD memory card;
* Transmit that data wirelessly to a web server; and
* powers sensors when used, and powers the board down to a quiescent of about 3mA between useage.
## New for this fork   
* ONLY applies to ModularSensors\a\.. directories (Mayfly and other ARM SAMDx boards)   
* Scales the rollout of multiple loggers with ms_cfg.ini configuration on the SD memory card
* Manages the available LiIon battery power, with best management practices for power demand management,
* Tested for ruggedness/reliability with the Xbee WiFi S6 module (and future the Xbee LTE family and Xbee ISM band modules)

To use, from platformio.ini open folder ModularSensors\a\<select a folder> - and press icon "Build" (the tick mark)   
tbd - download a prebuilt image   
This is an open source fork of https://github.com/EnviroDIY/ModularSensors   
EnviroDIY/ModularSensors is a comprehensive package that covers a number of boards and example usages. Its the recommended starting point for anybody new to ModularSensors, its assumed you read the excellent https://github.com/EnviroDIY/ModularSensors/blob/master/README.md
There is an comprehensive manual on riparian monitoring using the Mayfly  https://www.envirodiy.org/mayfly-sensor-station-manual/

This fork focuses on a rugged reliable scalable use of ModularSensors using the Mayfly 0.5b based on the mature AVR Mega1284 processor with 16Kb ram. 
Embedded processor functionality is largely dictated by size of the ram and flash memory.
Boards for riparian monitoring need solar & LiIon battery, wireless modules such as what the Mafly 0.5b + 12V Wingboard offer. 
Other more extensible boards based on the Arm Cortex M family will probably be supported over time -
 eg Adafruit Feather Alogger

To use this fork, and underestand the New features, its best to have code famalirity with ModularSensor. 
I hope to make this simpler in the future.   
The standard educational route is use a Mayfly 0.5b, open the following - and change the .ino file to a src directory and then build 
EnviroDIY/ModularSensors/examples/menu_a_la_carte

Verify builds and dowload to your Mayfly.

To build from this fork, in platformio opend the folder  ModularSensors/a/<directory>/ 
Note the changes in  platformio.ini
<blockquote><pre><code>
    https://github.com/neilh10/ModularSensors#release1
;  ^^ Use this when working from this fork    

</code></pre></blockquote>
and other changes. You will need to know how platformio.ini works.

Finally build, and download it to the chosen target. 
It now requires the SD card to have a ms_cfg.ini https://github.com/neilh10/ModularSensors/wiki/Feature-INI-file

This library is a volunteer open source effort by the author, and is built on the effort of a number of people who open sourced their effor with ModularSensors - thankyou thankyou. 
As an open source addition to ModularSensors you are free to use at your own discretion, and at your own risk. I've provided some description of what tests I've run at 
https://github.com/neilh10/ModularSensors/wiki/Testing-overview
  

For understanding ModularSensors, the best place to start is to visit enviroDIY.org. 
For bugs or issues with THIS fork, please open an issue. 
For changes made on this fork the best place to start is with a source differencing tool like http://meldmerge.org/


## Contributing
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions for the whole library. 

For this fork open an [issue](https://github.com/neilh10/ModularSensors/issues) to suggest and discuss potential changes/additions for this fork. 
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


For power contributors,fork-it, modify and generate Pull Requests.
https://github.com/neilh10/ModularSensors/wiki/git-cmds-for-preparing-for-a-PR

[//]: # ( @section mainpage_license License )
## License
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

Hardware designs shared are released, unless otherwise indicated, under the [CERN Open Hardware License 1.2](http://www.ohwr.org/licenses/cern-ohl/v1.2) (CERN_OHL).

[//]: # ( @section mainpage_acknowledgments Acknowledgments )
## Acknowledgments
See https://github.com/EnviroDIY/ModularSensors#acknowledgments   
[Beth Fisher](https://github.com/fisherba) for sharing/helping with her vision of ModularSensors.   
[Anthony Aufdenkampe](https://github.com/aufdenkampe) for releasing the Modbus Interface/WingBoard, and making it possible to access a class of +12V Modbus Industrial Sensors.   
[Sara Damiano](https://github.com/SRGDamia1) for being the "Class maestro" of ModularSensors :).   

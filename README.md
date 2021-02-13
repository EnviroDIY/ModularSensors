[//]: # ( @mainpage ModularSensors )
# ModularSensors
___

[//]: # ( @section mainpage_intro The EnviroDIY ModularSensors Library )
## The EnviroDIY ModularSensors Library

If you're new to EnviroDIY, I suggest you check out the [Just Getting Started](https://envirodiy.github.io/ModularSensors/page_getting_started.html) section of the documentation!

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly.
The ModularSensors library is specifically designed to support wireless, solar-powered environmental data logging applications, that is, to:
* Retrieve data from many physical sensors;
* Save that data to a SD memory card;
* Transmit that data wirelessly to a web server; and
* Put the processor, sensors and all other peripherals to sleep between readings to conserve power.
* Based in the riparian corridor
* Supports 12V/Modbus interface through Wingboard 
* powers sensors when used, and powers the board down to a quiescent of about 3mA between useage.

## New for this fork   

* Adds geographically scaling for multiple loggers using the same program/binays. It does this with a  custome ms_cfg.ini configuration file on the SD memory card
* Adds electronic configuration information to the Mayfly board, so that the readings can be traced to specific mayfly at a specific geographical loction.
* Manages the available battery power, with best management practices for power demand management. One option is the LiIon rechargeable battery + solar panel. Another option is standalone, no solar panel capability. Confgiruable in the ms_cfg.ini
* Tested for ruggedness/reliability with the Xbee LTE & WiFi S6 modules.
* prebuilt hex files at https://github.com/neilh10/ms_releases/wiki
* Specific development stream in ModularSensors\a\.. directories (Mayfly and other ARM SAMDx boards)   

* ONLY applies to ModularSensors\examples\tu_xx01 (Mayfly)
* Scales the rollout of multiple loggers with ms_cfg.ini configuration on the SD memory card
* Manages the available LiIon battery power, with best management practices for power demand management,
* Tested for ruggedness/reliability with the Xbee WiFi S6 module (and future the Xbee LTE family and Xbee ISM band modules)

To use, from the IDE Platformio (platformio.org) open folder ModularSensors\a\<select a folder> - and press icon "Build" (the tick mark), then download to the target.   
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

The ModularSensors library coordinates these tasks by "wrapping" native sensor libraries into a common interface of functions and returns.
These [wrapper functions](https://en.wikipedia.org/wiki/Wrapper_function) serve to harmonize and simplify the process of iterating through and logging data from a diverse set of sensors and variables.
Using the common sensor and variable interface, the library attempts to optimize measurement timing as much as possible to reduce logger "on-time" and power consumption.

Although this library was written primarily for the [EnviroDIY Mayfly data logger board](https://envirodiy.org/mayfly/), it is also designed to be [compatible with a variety of other Arduino-based boards](https://envirodiy.github.io/ModularSensors/page_processor_compatibility.html) as well.

There is extensive documentation available in the [ModularSensors github pages](https://envirodiy.github.io/ModularSensors/index.html) including details of the class structures and example code.

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [ModularSensors](#modularsensors)
  - [The EnviroDIY ModularSensors Library](#the-envirodiy-modularsensors-library)
  - [Supported Sensors](#supported-sensors)
  - [Data Endpoints](#data-endpoints)
  - [Supported Cellular/Wifi Modules:](#supported-cellularwifi-modules)
  - [Contributing](#contributing)
  - [License](#license)
  - [Acknowledgments](#acknowledgments)

[//]: # ( End GitHub Only )

[//]: # ( @section mainpage_supported_sensors Supported Sensors )
## Supported Sensors

For some generalized information about attaching sensors to an Arduino style board, see the [Sensor Notes page](https://envirodiy.github.io/ModularSensors/page_sensor_notes.html)

- [Processor Metrics: battery voltage, free RAM, sample count](https://envirodiy.github.io/ModularSensors/group__sensor__processor.html)
- [Maxim DS3231: real time clock](https://envirodiy.github.io/ModularSensors/group__sensor__ds3231.html)
- [Analog Electrical Conductivity: conductivity](https://envirodiy.github.io/ModularSensors/group__sensor__analog__cond.html)
- [AOSong AM2315: humidity & temperature](https://envirodiy.github.io/ModularSensors/group__sensor__am2315.html)
- [AOSong DHT: humidity & temperature](https://envirodiy.github.io/ModularSensors/group__sensor__dht.html)
- [Apogee SQ-212: quantum light sensor, via TI ADS1115](https://envirodiy.github.io/ModularSensors/group__sensor__sq212.html)
- [Atlas Scientific EZO Sensors](https://envirodiy.github.io/ModularSensors/group__atlas__group.html)
    - [EZO-CO2: Carbon Dioxide and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__atlas__co2.html)
    - [EZO-DO: Dissolved Oxygen](https://envirodiy.github.io/ModularSensors/group__sensor__atlas__do.html)
    - [EZO-EC: Conductivity, Total Dissolved Solids, Salinity, and Specific Gravity](https://envirodiy.github.io/ModularSensors/group__sensor__atlas__cond.html)
    - [EZO-ORP: Oxidation/Reduction Potential](https://envirodiy.github.io/ModularSensors/group__sensor__atlas__orp.html)
    - [EZO-pH: pH](https://envirodiy.github.io/ModularSensors/group__sensor__atlas__ph.html)
    - [EZO-RTD: Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__atlas__rtd.html)
- [Bosch BME280: barometric pressure, humidity & temperature](https://envirodiy.github.io/ModularSensors/group__sensor__bme280.html)
- [Campbell Scientific OBS-3+: turbidity, via TI ADS1115](https://envirodiy.github.io/ModularSensors/group__sensor__obs3.html)
- [Decagon Devices ES-2: conductivity ](https://envirodiy.github.io/ModularSensors/group__sensor__es2.html)
- [Freescale Semiconductor MPL115A2: barometric pressure and temperature](https://envirodiy.github.io/ModularSensors/group__sensor__mpl115a2.html)
- [External Arduino I2C Rain Tipping Bucket Counter: rainfall totals](https://envirodiy.github.io/ModularSensors/group__sensor__i2c__rain.html)
- [Insitu RDO PRO-X: dissolved oxygen](https://envirodiy.github.io/ModularSensors/group__sensor__insitu__rdo.html.html)
- [Keller Submersible Level Transmitters: pressure and temperature](https://envirodiy.github.io/ModularSensors/group__keller__group.html)
    - [Acculevel](https://envirodiy.github.io/ModularSensors/group__sensor__acculevel.html)
    - [Nanolevel](https://envirodiy.github.io/ModularSensors/group__sensor__nanolevel.html)
- [MaxBotix MaxSonar: water level](https://envirodiy.github.io/ModularSensors/group__sensor__maxbotix.html)
- [Maxim DS18: temperature](https://envirodiy.github.io/ModularSensors/group__sensor__ds18.html)
- [Measurement Specialties MS5803: pressure and temperature](https://envirodiy.github.io/ModularSensors/group__sensor__ms5803.html)
- Meter Environmental Soil Moisture Probes: soil Ea and volumetric water content
    - [Meter ECH2O 5TM](https://envirodiy.github.io/ModularSensors/group__sensor__fivetm.html)
    - [Meter Teros 11](https://envirodiy.github.io/ModularSensors/group__sensor__teros11.html)
- [Meter Environmental Hydros 21 (formerly Decagon Devices CTD-10): conductivity, temperature & depth](https://envirodiy.github.io/ModularSensors/group__sensor__hydros21.html)
- [Northern Widget Tally Event Counter: number of events](https://envirodiy.github.io/ModularSensors/group__sensor__tally.html)
- [PaleoTerra Redox Sensor: redox potential](https://envirodiy.github.io/ModularSensors/group__sensor__pt__redox.html)
- [TI ADS1115: external voltage with support for divided current](https://envirodiy.github.io/ModularSensors/group__sensor__ads1x15.html)
- [TI INA219: current, voltage, and power draw](https://envirodiy.github.io/ModularSensors/group__sensor__ina219.html)
- [Turner Cyclops-7F: various parameters](https://envirodiy.github.io/ModularSensors/group__sensor__cyclops.html)
- [Yosemitech: water quality sensors](https://envirodiy.github.io/ModularSensors/group__yosemitech__group.html)
    - [Y502-A or Y504-A: Optical DO and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y504.html)
    - [Y510-B: Optical Turbidity and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y510.html)
    - [Y511-A: Optical Turbidity and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y511.html)
    - [Y514-A: Optical Chlorophyll and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y514.html)
    - [Y520-A: Conductivity and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y520.html)
    - [Y532-A: Digital pH and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y532.html)
    - [Y533: ORP, pH, and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y533.html)
    - [Y550-B: UV254/COD, Turbidity, and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y550.html)
    - [Y4000 Multiparameter Sonde](https://envirodiy.github.io/ModularSensors/group__sensor__y4000.html)
- [Zebra-Tech D-Opto: dissolved oxygen](https://envirodiy.github.io/ModularSensors/group__sensor__dopto.html)


[//]: # ( @section mainpage_data_receivers Data Endpoints )
## Data Endpoints

Within ModularSensors, the "dataPublisher" objects add the functionality to send data to remote web services.
The currently supported services are the [Monitor My Watershed data portal](http://data.envirodiy.org/), [ThingSpeak](https://thingspeak.com/), and the [Ubidots IoT platform](https://ubidots.com).

- [Monitor My Watershed/EnviroDIY Data Portal](https://envirodiy.github.io/ModularSensors/class_enviro_d_i_y_publisher.html)
- [ThingSpeak](https://envirodiy.github.io/ModularSensors/class_thing_speak_publisher.html)
- [Ubidots IoT platform](https://envirodiy.github.io/ModularSensors/class_ubidots_publisher.html)

[//]: # ( @todo Page on Data Endpoints )


[//]: # ( @section mainpage_modems Supported Cellular/Wifi Modules )
## Supported Cellular/Wifi Modules:

For information common to all modems and for tables of the proper class, baud rate, and pins to uses, see the [Modem Notes page](https://envirodiy.github.io/ModularSensors/page_modem_notes.html).

- [Digi XBee](https://envirodiy.github.io/ModularSensors/group__modem__digi.html)
    - Digi XBee® 3 Cellular LTE-M/NB-IoT
    - Digi XBee® 3 Cellular LTE Cat 1 (AT&T or Verizon)
    - Digi XBee® Cellular 3G
    - Digi XBee® Cellular LTE Cat 1 (Verizon)
    - Digi XBee® Wi-Fi (S6B)
- [ESP8266](https://envirodiy.github.io/ModularSensors/group__modem__esp8266.html)
- [QuectelBG96](https://envirodiy.github.io/ModularSensors/group__modem__bg96.html)
- [Sequans Monarch](https://envirodiy.github.io/ModularSensors/group__modem__monarch.html)
- [SIM7000](https://envirodiy.github.io/ModularSensors/group__modem__sim7000.html)
- [SIM800](https://envirodiy.github.io/ModularSensors/group__modem__sim800.html), including the [Sodaq GPRSBee](https://envirodiy.github.io/ModularSensors/group__modem__gprsbee.html)
- u-blox LTE-M R4 and N4 series, including the [Sodaq uBee](https://envirodiy.github.io/ModularSensors/group__modem__ubee__ltem.html)
- u-blox 2G, 3G, and 4G, including the [Sodaq 3GBee](https://envirodiy.github.io/ModularSensors/group__modem__ubee__3g.html)


[//]: # ( @section mainpage_contributing Contributing )
## Contributing
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions.
Feel free to open issues about any bugs you find or any sensors you would like to have added.

If you would like to directly help with the coding development of the library, there are some [tips here](https://envirodiy.github.io/ModularSensors/page_for_developers.html) on how to set up PlatformIO so you can fork the library and test programs while in the library repo.
Please _take time to familiarize yourself with the [terminology, classes and data structures](https://envirodiy.github.io/ModularSensors/page_library_terminology.html) this library uses_.
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
[EnviroDIY](http://envirodiy.org/)™ is presented by the Stroud Water Research Center, with contributions from a community of enthusiasts sharing do-it-yourself ideas for environmental science and monitoring.

[Sara Damiano](https://github.com/SRGDamia1) is the primary developer of the EnviroDIY ModularSensors library, with input from many [other contributors](https://github.com/EnviroDIY/ModularSensors/graphs/contributors).

This project has benefited from the support from the following funders:

* William Penn Foundation
* US Environmental Protection Agency (EPA)
* National Science Foundation, awards [EAR-0724971](http://www.nsf.gov/awardsearch/showAward?AWD_ID=0724971), [EAR-1331856](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1331856), [ACI-1339834](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1339834)
* Stroud Water Research Center endowment

Other Contributors
[Beth Fisher](https://github.com/fisherba) for sharing/helping with her vision of ModularSensors.   
[Anthony Aufdenkampe](https://github.com/aufdenkampe) for releasing the Modbus Interface/WingBoard, and making it possible to access a class of +12V Modbus Industrial Sensors.   

 Acknowledgements for this fork
[Neil Hancock] is solely responsible for its content. 


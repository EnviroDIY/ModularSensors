# ModularSensors <!-- {#mainpage} -->

___

## The EnviroDIY ModularSensors Library <!-- {#mainpage_intro} -->

If you're new to EnviroDIY, I suggest you check out the [Just Getting Started](https://envirodiy.github.io/ModularSensors/page_getting_started.html) section of the documentation!

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly.
The ModularSensors library is specifically designed to support wireless, solar-powered environmental data logging applications, that is, to:
- Retrieve data from many physical sensors;
- Save that data to a SD memory card;
- Transmit that data wirelessly to a web server; and
- **Put the processor, sensors and all other peripherals to sleep between readings to conserve power.**

The ModularSensors library coordinates these tasks by "wrapping" native sensor libraries into a common interface of functions and returns.
These [wrapper functions](https://en.wikipedia.org/wiki/Wrapper_function) serve to harmonize and simplify the process of iterating through and logging data from a diverse set of sensors and variables.
Using the common sensor and variable interface, the library attempts to optimize measurement timing as much as possible to reduce logger "on-time" and power consumption.

Although this library was written primarily for the [EnviroDIY Mayfly data logger board](https://envirodiy.org/mayfly/), it is also designed to be [compatible with a variety of other Arduino-based boards](https://envirodiy.github.io/ModularSensors/page_processor_compatibility.html) as well.

There is extensive documentation available in the [ModularSensors github pages](https://envirodiy.github.io/ModularSensors/index.html) including details of the class structures and example code.

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

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

## Supported Sensors <!-- {#mainpage_supported_sensors} -->

For some generalized information about attaching sensors to an Arduino style board, see the [Sensor Notes page](https://envirodiy.github.io/ModularSensors/page_sensor_notes.html).

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
- [Bosch BMP388 and BMP390: barometric pressure & temperature](https://envirodiy.github.io/ModularSensors/group__sensor__bmp3xx.html)
- [Campbell Scientific OBS-3+: turbidity, via TI ADS1115](https://envirodiy.github.io/ModularSensors/group__sensor__obs3.html)
- [Campbell Scientific ClariVUE10: turbidity](https://envirodiy.github.io/ModularSensors/group__sensor__clarivue.html)
- [Decagon Devices ES-2: conductivity ](https://envirodiy.github.io/ModularSensors/group__sensor__es2.html)
- [Decagon Devices CTD-10: conductivity, temperature & depth ](https://envirodiy.github.io/ModularSensors/group__sensor__decagon__ctd.html)
- [Everlight ALS-PT19 Analog Light Sensor (via processor ADC)](https://envirodiy.github.io/ModularSensors/group__sensor__alspt19.html)
- [Freescale Semiconductor MPL115A2: barometric pressure and temperature](https://envirodiy.github.io/ModularSensors/group__sensor__mpl115a2.html)
- [External Arduino I2C Rain Tipping Bucket Counter: rainfall totals](https://envirodiy.github.io/ModularSensors/group__sensor__i2c__rain.html)
- [In-Situ RDO PRO-X: dissolved oxygen](https://envirodiy.github.io/ModularSensors/group__sensor__insitu__rdo.html)
- [In-Situ SDI-12 TROLLs: pressure, temperature, and depth](https://envirodiy.github.io/ModularSensors/group__sensor__insitu__troll.html)
- [Keller Submersible Level Transmitters: pressure and temperature](https://envirodiy.github.io/ModularSensors/group__keller__group.html)
    - [Acculevel](https://envirodiy.github.io/ModularSensors/group__sensor__acculevel.html)
    - [Nanolevel](https://envirodiy.github.io/ModularSensors/group__sensor__nanolevel.html)
- [MaxBotix MaxSonar: water level](https://envirodiy.github.io/ModularSensors/group__sensor__maxbotix.html)
- [Maxim DS18: temperature](https://envirodiy.github.io/ModularSensors/group__sensor__ds18.html)
- [Measurement Specialties MS5803: pressure and temperature](https://envirodiy.github.io/ModularSensors/group__sensor__ms5803.html)
- Meter Environmental Soil Moisture Probes: soil Ea and volumetric water content
    - [Meter ECH2O 5TM](https://envirodiy.github.io/ModularSensors/group__sensor__fivetm.html)
    - [Meter Teros 11](https://envirodiy.github.io/ModularSensors/group__sensor__teros11.html)
- [Meter Environmental Hydros 21: conductivity, temperature & depth](https://envirodiy.github.io/ModularSensors/group__sensor__hydros21.html)
- [Northern Widget Tally Event Counter: number of events](https://envirodiy.github.io/ModularSensors/group__sensor__tally.html)
- [PaleoTerra Redox Sensor: redox potential](https://envirodiy.github.io/ModularSensors/group__sensor__pt__redox.html)
- [Sensirion SHT40: humidity & temperature](https://envirodiy.github.io/ModularSensors/group__sensor__sht4x.html)
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
    - [Y533: ORP, and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y533.html)
    - [Y551: UV254/COD, Turbidity, and Temperature](https://envirodiy.github.io/ModularSensors/group__sensor__y551.html)
    - [Y560: Ammonium, Temperature, and pH](https://envirodiy.github.io/ModularSensors/group__sensor__y560.html)
    - [Y4000 Multiparameter Sonde](https://envirodiy.github.io/ModularSensors/group__sensor__y4000.html)
- [Zebra-Tech D-Opto: dissolved oxygen](https://envirodiy.github.io/ModularSensors/group__sensor__dopto.html)


## Data Endpoints <!-- {#mainpage_data_receivers} -->

Within ModularSensors, the "dataPublisher" objects add the functionality to send data to remote web services.
The currently supported services are the [Monitor My Watershed data portal](http://data.envirodiy.org/), [ThingSpeak](https://thingspeak.com/), and the [Ubidots IoT platform](https://ubidots.com).

- [Monitor My Watershed/EnviroDIY Data Portal](https://envirodiy.github.io/ModularSensors/class_enviro_d_i_y_publisher.html)
- [ThingSpeak](https://envirodiy.github.io/ModularSensors/class_thing_speak_publisher.html)
- [Ubidots IoT platform](https://envirodiy.github.io/ModularSensors/class_ubidots_publisher.html)

[//]: # ( @todo Page on Data Endpoints )


## Supported Cellular/Wifi Modules: <!-- {#mainpage_modems} -->

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
- [SIM7080](https://envirodiy.github.io/ModularSensors/group__modem__sim7080.html)
- [SIM7000](https://envirodiy.github.io/ModularSensors/group__modem__sim7000.html)
- [SIM800](https://envirodiy.github.io/ModularSensors/group__modem__sim800.html), including the [Sodaq GPRSBee](https://envirodiy.github.io/ModularSensors/group__modem__gprsbee.html)
- u-blox LTE-M R4 and N4 series, including the [Sodaq uBee](https://envirodiy.github.io/ModularSensors/group__modem__ubee__ltem.html)
- u-blox 2G, 3G, and 4G, including the [Sodaq 3GBee](https://envirodiy.github.io/ModularSensors/group__modem__ubee__3g.html)


## Contributing <!-- {#mainpage_contributing} -->
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions.
Feel free to open issues about any bugs you find or any sensors you would like to have added.

If you would like to directly help with the coding development of the library, there are some [tips here](https://envirodiy.github.io/ModularSensors/page_for_developers.html) on how to set up PlatformIO so you can fork the library and test programs while in the library repo.
Please _take time to familiarize yourself with the [terminology, classes and data structures](https://envirodiy.github.io/ModularSensors/page_library_terminology.html) this library uses_.
This library is built to fully take advantage of Objecting Oriented Programing (OOP) approaches and is larger and more complicated than many Arduino libraries.
There is _extensive_ documentation on our [github pages](https://envirodiy.github.io/ModularSensors/index.html) and an _enormous_ number of comments and debugging printouts in the code itself to help you get going.


## License <!-- {#mainpage_license} -->
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

Hardware designs shared are released, unless otherwise indicated, under the [CERN Open Hardware License 1.2](http://www.ohwr.org/licenses/cern-ohl/v1.2) (CERN_OHL).

## Acknowledgments <!-- {#mainpage_acknowledgments} -->
[EnviroDIY](http://envirodiy.org/)™ is presented by the Stroud Water Research Center, with contributions from a community of enthusiasts sharing do-it-yourself ideas for environmental science and monitoring.

[Sara Damiano](https://github.com/SRGDamia1) is the primary developer of the EnviroDIY ModularSensors library, with input from many [other contributors](https://github.com/EnviroDIY/ModularSensors/graphs/contributors).

This project has benefited from the support from the following funders:

* William Penn Foundation
* US Environmental Protection Agency (EPA)
* National Science Foundation, awards [EAR-0724971](http://www.nsf.gov/awardsearch/showAward?AWD_ID=0724971), [EAR-1331856](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1331856), [ACI-1339834](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1339834)
* Stroud Water Research Center endowment


[//]: # ( @m_innerpage{page_getting_started} )

[//]: # ( @m_innerpage{page_faq} )

[//]: # ( @m_innerpage{page_other_notes} )

[//]: # ( @m_innerpage{page_the_examples} )

[//]: # ( @m_innerpage{license_software-license-agreement-bsd-3-license} )

[//]: # ( @m_innerpage{change_log} )

[//]: # ( @m_innerpage{todo} )

[//]: # ( @m_innerpage{deprecated} )

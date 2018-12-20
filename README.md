# ModularSensors

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly. The ModularSensors library is specifically designed to support wireless, solar-powered environmental data logging applications, that is, to:
* Retrieve data from many physical sensors;
* Save that data to a SD memory card;
* Transmit that data wirelessly to a web server; and
* Put the processor, sensors and all other peripherals to sleep between readings to conserve power.

The ModularSensors library coordinates these tasks by "wrapping" native sensor libraries into a common interface of functions and returns. These [wrapper functions](https://en.wikipedia.org/wiki/Wrapper_function) serve to harmonize and simplify the process of iterating through and logging data from a diverse set of sensors and variables.  Using the common sensor and variable interface, the library attempts to optimize measurement timing as much as possible to reduce logger "on-time" and power consumption.

We have a guide for getting started [right here](https://github.com/EnviroDIY/ModularSensors/wiki/Getting-Started).

Although this library was written primarily for the [EnviroDIY Mayfly data logger board](https://envirodiy.org/mayfly/), it is also designed to be [compatible with a variety of other Arduino-based boards](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Compatibility) as well.

## Data can currently be sent to these web services:

- [WikiWatershed/EnviroDIY Data Portal](https://github.com/EnviroDIY/ModularSensors/wiki/EnviroDIY-Portal-Functions)
- [ThingSpeak](https://github.com/EnviroDIY/ModularSensors/wiki/ThingSpeak-Functions)

## These sensors are currently supported:

- [Apogee SQ-212: quantum light sensor, via TI ADS1115](https://github.com/EnviroDIY/ModularSensors/wiki/Apogee-SQ212)
- [AOSong AM2315: humidity & temperature](https://github.com/EnviroDIY/ModularSensors/wiki/AOSong-AM2315)
- [AOSong DHT: humidity & temperature](https://github.com/EnviroDIY/ModularSensors/wiki/AOSong-DHT)
- [Bosch BME280: barometric pressure, humidity & temperature](https://github.com/EnviroDIY/ModularSensors/wiki/Bosch-BME280)
- [Campbell Scientific OBS-3+: turbidity, via TI ADS1115](https://github.com/EnviroDIY/ModularSensors/wiki/Campbell-OBS3)
- [Meter Environmental ECH2O 5TM (formerly Decagon Devices 5TM): soil moisture](https://github.com/EnviroDIY/ModularSensors/wiki/Meter-ECH2O-5TM)
- [Meter Environmental Hydros 21 (formerly Decagon Devices CTD-10): conductivity, temperature & depth](https://github.com/EnviroDIY/ModularSensors/wiki/Meter-Hydros-21)
- [Decagon Devices ES-2: conductivity ](https://github.com/EnviroDIY/ModularSensors/wiki/Decagon-ES2)
- [External I2C Rain Tipping Bucket Counter: rainfall totals](https://github.com/EnviroDIY/ModularSensors/wiki/I2C-Tipping-Bucket)
- [External Voltage: via TI ADS1115](https://github.com/EnviroDIY/ModularSensors/wiki/TI-ADS1115-Voltage)
- [Freescale Semiconductor MPL115A2: barometric pressure and temperature](https://github.com/EnviroDIY/ModularSensors/wiki/Freescale-MPL115A2)
- [Keller Submersible Level Transmitters: pressure and temperature](https://github.com/EnviroDIY/ModularSensors/wiki/Keller-Level)
    - Acculevel
    - Nanolevel
- [MaxBotix MaxSonar: water level](https://github.com/EnviroDIY/ModularSensors/wiki/MaxBotix-MaxSonar)
- [Maxim DS18: temperature](https://github.com/EnviroDIY/ModularSensors/wiki/Maxim-DS18)
- [Maxim DS3231: real time clock](https://github.com/EnviroDIY/ModularSensors/wiki/Maxim-DS3231)
- [Measurement Specialties MS5803: pressure and temperature](https://github.com/EnviroDIY/ModularSensors/wiki/Measurement-Specialties-MS5803)
- [TI INA219: current, voltage, and power draw](https://github.com/EnviroDIY/ModularSensors/wiki/TI-INA219)
- [Yosemitech: water quality sensors](https://github.com/EnviroDIY/ModularSensors/wiki/Yosemitech-Sensors)
    - Y502-A or Y504-A: Optical DO and Temperature
    - Y510-B: Optical Turbidity and Temperature
    - Y511-A: Optical Turbidity and Temperature
    - Y514-A: Optical Chlorophyll and Temperature
    - Y520-A: Conductivity and Temperature
    - Y532-A: Digital pH and Temperature
    - Y533: ORP, pH, and Temperature
    - Y550-B: UV254/COD, Turbidity, and Temperature
    - Y4000 Multiparameter Sonde
- [Zebra-Tech D-Opto: dissolved oxygen](https://github.com/EnviroDIY/ModularSensors/wiki/ZebraTech-DOpto)
- [Processor Metrics: battery voltage, free RAM, sample count](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Metadata)



## Contributing
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions.  Feel free to open issues about any bugs you find or any sensors you would like to have added.

If you would like to directly help with the coding development of the library, there are some [tips here](https://github.com/EnviroDIY/ModularSensors/wiki/Developer-Setup) on how to set up PlatformIO so you can fork the library and test programs while in the library repo.  Please also take time to familiarize yourself with the classes and data structures this library uses.  This library is built to fully take advantage of Objecting Oriented Programing (OOP) approaches and is larger and somewhat more complicated than many Arduino libraries.  Make sure to familiarize yourself with the [terminology](https://github.com/EnviroDIY/ModularSensors/wiki/Terminology) and data structures the library uses.



## License
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

Hardware designs shared are released, unless otherwise indicated, under the [CERN Open Hardware License 1.2](http://www.ohwr.org/licenses/cern-ohl/v1.2) (CERN_OHL).

## Acknowledgments
[EnviroDIY](http://envirodiy.org/)™ is presented by the Stroud Water Research Center, with contributions from a community of enthusiasts sharing do-it-yourself ideas for environmental science and monitoring.

[Sara Damiano](https://github.com/SRGDamia1) is the primary developer of the EnviroDIY ModularSensors library, with input from many [other contributors](https://github.com/EnviroDIY/ModularSensors/graphs/contributors).

This project has benefited from the support from the following funders:

* William Penn Foundation
* US Environmental Protection Agency (EPA)
* National Science Foundation, awards [EAR-0724971](http://www.nsf.gov/awardsearch/showAward?AWD_ID=0724971), [EAR-1331856](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1331856), [ACI-1339834](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1339834)
* Stroud Water Research Center endowment

# ModularSensors

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly. 
This version of the ModularSensors library is a rugged solar powered wireless data logger, that:
* Based in the riparian corridor
* Polls local physical sensors readings
* Supports 12V/Modbus interface through Wingboard 
* Stores the readings  to a SD memory card;
* Transmit that data wirelessly to a web server; and
* powers sensors when used, and powers the board down to a quiescent of about 3mA between useage.   
## New for this fork   
* Scales the rollout of multiple loggers with configuration .ini on the SD memory card
* Manages the available LiIon battery power, with best management practisese for power demand management,
* Tested for ruggedness/reliability with the Xbee WiFi S6 module (and future the Xbee LTE family and Xbee ISM band modules)

This is open source fork of https://github.com/EnviroDIY/ModularSensors 
EnviroDIY/ModularSensors is a comprehensive package that covers a number of boards and example useages. Its the recommended starting point for anybody new to ModularSensors, its assumed you read the excellent https://github.com/EnviroDIY/ModularSensors/blob/master/README.md
There is an comprhensive manual on riparian monitoring using the Mayfly  https://www.envirodiy.org/mayfly-sensor-station-manual/

This fork focuses on a rugged reliable scaleable use of ModularSensors using the Mayfly 0.5b based on the mature AVR Mega1284 processor with 16Kb ram. 
Embedded processor functionality is largely dictated by size of the ram and flash memory.
Boards for riparian monitoring need solar & LiIon battery, wireless modules such as what the Mafly 0.5b + 12V Wingboard offer. 
Other more extensible boards based on the Arm Cortex M family will probably be supported over time -
 eg Adafruit Feather Alogger

To use this fork, start with a working ModularSensors 0.17.02 or later built using PlatformIO and working with a Mayfly 0.5b
EnviroDIY/ModularSensors/exampleslogging_to_EnviroDIY.ino
https://github.com/EnviroDIY/ModularSensors/tree/master/examples/logging_to_EnviroDIY
and in the platformio.ini substitute
<blockquote><pre><code>
    ;EnviroDIY_ModularSensors@=0.17.2
    https://github.com/neilh10/ModularSensors#release1
;  ^^ Use this when working from an official release of the library   
;    https://github.com/EnviroDIY/ModularSensors.git#develop   
;  ^^ Use this when if you want to pull from the develop branch    
    https://github.com/neilh10/Adafruit_INA219#envirodiy   
</code></pre></blockquote>

This library is a volunteer open source effort by the author, and is built on the effort of a number of people who open sourced their effor with ModularSensors - thankyou thankyou. 
As an open source addition to ModularSensors you are free to use at your own discretion, and at your own risk. I've provided some description of what tests I've run at 
https://github.com/neilh10/ModularSensors/wiki/Testing-overview
  

For understanding ModularSensors, the best place to start is to visit enviroDIY.org. 
For bugs or issues with THIS fork, please open an issue. 
For changes made on this fork the best place to start is with a source differencing tool like http://meldmerge.org/


## Contributing
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions for the whole library. 

For this fork Open an [issue](https://github.com/neilh10/ModularSensors/issues) to suggest and discuss potential changes/additions for this fork. 

For power contributors,fork-it, modify and generate Pull Requests.
https://github.com/neilh10/ModularSensors/wiki/git-cmds-for-preparing-for-a-PR

## License
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

Hardware designs shared are released, unless otherwise indicated, under the [CERN Open Hardware License 1.2](http://www.ohwr.org/licenses/cern-ohl/v1.2) (CERN_OHL).

## Acknowledgments
See https://github.com/EnviroDIY/ModularSensors#acknowledgments
[Beth Fisher](https://github.com/fisherba) for sharing and teaching ModularSensors
[Anthony Aufdenkampe](https://github.com/aufdenkampe) for releasing the Modbus Interface/WingBoard, and making it possible to access a class of +12V Modbus Industrial Sensors.
[Sara Damiano](https://github.com/SRGDamia1) for being the "force" behind ModularSensors:).

# kn_depth - keller Nanolevel Depth Monitor

A ruggidized data logger using the Keller Nanolevel and XbeeWiFi module using the Modular Sensors Library. 

Data Portals configured through a SD card, for more reliable testing and scaling. The same .hex can be deployed to many systems, and configured throuhg

To use the internet viewing capability you need a data portal account - sites supported
  (http://data.envirodiy.org/).  After you have registered the site and sensors, the portal will generate a registration token and universally unique identifier (UUID) for each site and further UUID's for each variable.  
  You will need to apply all of those UUID values into your Mayfly.ini .  
  thingSpeak - tbd.
Modems supported Xbee WiFi S6, and Xbee LTE (and tbd LoRa) 

_______

## TO create New Site
locally on disk create new directory eg firmware\xxx
create (or copy configured) ms_cfg.ini
edit ms_cfg.ini (eg notepad)
go to https://data.envirodiy.org/sites

(possibly dup - see https://github.com/neilh10/ModularSensors/wiki/Setting-up-data.enviroDIY-for-TU-RC)
create new site - and copy REGISTRATION_TOKEN/SAMPLING_FEATURE  
On web click on "Manage Sensors"
Click on + to create new sensor  - the order the sensors are created in as the order of the preview graphs.

Select

tbd
Press "View Token UUID list", highlight and copy "UUIDs[]" to ,cfg


_______

## To Use this Example: - the following still tbd
From Platformio File->Open Folfer  "ModularSensors/a/kn_depth" 
On the bottom bar - press the Tick mark - "build" 
ms_cfg customizations located in "firmware/xxx" 

Press build icon 
#### Prepare and set up PlatformIO
- Register a site and sensors at the WikiWatershed/EnviroDIY data portal (http://data.WikiWatershed.org/)
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.  Without this, the program won't compile or send data.
- Download logging_to_EnviroDIY.ino and put it into the src directory of your project.  Delete main.cpp in that folder.

#### Modify the Example

Per Site/board configuration goes in ms_cfg.ini that is copied to the SD drive. 
    - site specific wifi username/password or cellular APN!
    - registration token/ sampling feature 
    - UUID's for sensors .


- Modify ms_cfg.h for the connfiguration of processors needed. supports Feather M4 express with custom Wing, with _Digi XBee LTE & S6B (wifi)_ modem including 
- **The configuration of pin numbers and serial ports can be routed from ms_cfg.h !**

Modify xxx.ino as necessary for custom processing.

#### Upload!
- Stage the deployment in an easy to test location (eg the office) **before** deploying out in the wild!


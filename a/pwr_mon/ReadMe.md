# tu_mon - Trout Unlimited Monitor 

A ruggidized remote unit "power monitor" using XbeeCellular or XbeeWifi Wireless with the Modular Sensors Library on customized hardware on the Autonomo or Mayflyardware.

The powermonitoring is done using an INA291 I2C. 
https://www.adafruit.com/product/904
The part needs to be configure with I2C wires, and may need to have the current sensing resistore replaced.
Please give each modified configuration a unique ID, and each unique part a serial number.

All power connections must be described. Its the resoponsibility of the local installer to ensure they understand how the power flows. 

Data Portals are configured through a SD card, for more reliable testing and scaling.

To use the internet viewing capability you need a data portal account - sites supported
  (http://data.envirodiy.org/).  After you have registered the site and sensors, the portal will generate a registration token and universally unique identifier (UUID) for each site and further UUID's for each variable.  
  You will need to apply all of those UUID values into your Mayfly.ini .  
  thingSpeak - tbd.
Modems supported Xbee WiFi S6, and Xbee LTE (and tbd LoRa) 

_______

## TO create New Site
locally on disk create new directory eg firmware\tu_rc_01
create (or copy configured) ms_cfg.ini
edit ms_cfg.ini (eg notepad)
go to https://data.envirodiy.org/sites

(possibly dup - see https://github.com/neilh10/ModularSensors/wiki/Setting-up-data.enviroDIY-for-TU-RC)
create new site - and copy REGISTRATION_TOKEN/SAMPLING_FEATURE  
On web click on "Manage Sensors"
Click on + to create new sensor  - the order the sensors are created in as the order of the preview graphs.

Select

Manufacturer: All  Model Ext Voltage Divider Variable: _Battery 
Units : V  Medium: Equipment  
Notes: Regulator Voltage (V) 0-26V

Manufacturer: All  Model Calculated Variable: _discharge 
Units...(any)  Medium: Equipment  
Notes: Measures the current (Amps)

Manufacturer: Envirodiy Model Mayfly Varianle: _SampleNum
Medium: Equipment  
Notes: Incrementing Number showing health of system

Manufacturer: Envirodiy Model Mayfly Varianle: _Batt 
Medium: Equipment  Notes: LiIon Battery Boltage


Finally - "BACK TO SITE DETAILS"
Press "View Token UUID list", highlight and copy "UUIDs[]" to ,cfg


_______

## To Use this Example: - the following still tbd
From PIO File->Open Folfer  "ModularSensors/tu_rc_01" 
On the bottom bar - press the Tick mark - "build" 
ms_cfg "firmware/tu_rc_01.cfg" 


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


- Modify ms_cfg.h for the connfiguration of processors needed. supports Mayfly and Autonomo, with _Digi XBee LTE & S6B (wifi)_ modem including 
- **The configuration of pin numbers and serial ports can be routed from ms_cfg.h !**

Modify xxx.ino as necessary for custom processing.

#### Upload!
- Stage the deployment in an easy to test location (eg the office) **before** deploying out in the wild!


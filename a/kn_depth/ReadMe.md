# kn_depth - short for keller Nanolevel Depth Monitor
A ruggidized Modular Sensors Library optomized for a few pieces of communications hardware.

Data Portals configured through a SD card, for more reliable testing and scaling.

To use the internet viewing capability you need a data portal account - sites supported
  (http://data.envirodiy.org/).  After you have registered the site and sensors, the portal will generate a registration token and universally unique identifier (UUID) for each site and further UUID's for each variable.  
  You will need to apply all of those UUID values into your Mayfly.ini .  
  thingSpeak - tbd.
Modems supported Xbee WiFi S6, and (tbd Xbee LTE) 

_______

## To Use this Example: - the following still tbd
From Platformio File->Open Folfer  "a/kn_depth" 
On the bottom bar - press the Tick mark - "build" 

Press build icon 
#### Prepare and set up PlatformIO
- Register a site and sensors at the WikiWatershed/EnviroDIY data portal (http://data.WikiWatershed.org/)
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.  Without this, the program won't compile or send data.
- Download logging_to_EnviroDIY.ino and put it into the src directory of your project.  Delete main.cpp in that folder.

#### Modify the Example
- Modify logging_to_EnviroDIY.ino to have the modem, sensor, and variable objects that you are interested in.
    - This example is written for a _Digi XBee S6B (wifi)_ modem.  Change this to whatever modem you are using.  Pastable chunks of code for each modem are available in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home).
    - Don't forget to put in your wifi username/password or cellular APN!
    - The only "sensor" included in this example is the temperature sensor on the clock.  Add code for all of your sensors.  See the pages for the individual sensors in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home) for code snippets/examples.
- **Make sure the pin numbers and serial ports selected in your code match with how things are physically attached to your board!**
- Add all of your variables to your variable array.
- Paste all of your variable UUID's for your site into your variable array, replacing all of the ```"12345678-abcd-1234-efgh-1234567890ab"``` values.
- Replace the registration token and sampling feature values in the example with those for your site:

```cpp
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
```

#### Upload!
- Stage the deployment in an easy to test location (eg the office) **before** deploying out in the wild!

#### Developing new code and changing libarys 
use the atlMayfly.ino and include files in this directory. platformio.ini:src_dir = .
This appears to use .\lib as a search path before .piolibdeps

(toBeChecked:) For mature release builds, depending on instrument setup, create a directory beneath this one eg
kellernano\atlMayfly.ino
platformio.ini:src_dir = kellernano

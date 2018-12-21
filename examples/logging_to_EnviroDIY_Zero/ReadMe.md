# Example using the Modular Sensors Library to save data to an SD card and send data to the EnviroDIY Data Portal

This shows the use of an "EnviroDIY logger" object on a SAMD21 (Arduino Zero) board.  For the purposes of this library, the main difference between SAMD boards and AVR boards is the availability of an processor real time clock on the SAMD boards and the many more hardware serial ports on the SAMD boards.  The processor real-time clock is not quite as accurate as the DS3231 required for AVR processors, but avoids involving an extra chip.  Hardware serial ports, on the other hand, are much more accurate than software serial ports and should always be used before resorting to a software serial port.  SAMD21 boards have 6 "SERCOM" ports that can be used.  The SerCom ports available for serial ports vary on different boards, so you must be aware of which ones are available for the board you are using.  The [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/SAMD21-SERCOMs) has examples of setting up two extra serial ports on an Adafruit Feather M0.

Before using this example, you must register a site and sensors at the data portal (http://data.envirodiy.org/).  After you have registered the site and sensors, the portal will generate a registration token and universally unique identifier (UUID) for each site and further UUID's for each variable.  You will need to copy all of those UUID values into your sketch to replace the "12345678-abcd-1234-efgh-1234567890ab" place holders in this example.  Please do not try to run the example exactly as written, but delete or add the chunks of code pertaining to sensors that you do not have attached.

This is the example you should use for any site that you want to have tied to the WikiWatershed, even if you won't be sending live data.  In cases where you won't use the live data, delete all of the lines pertaining to the modem and make sure your loop is set to ```logData()``` rather than ```logDataAndSend()```.  This ensures that you the file written to your SD card will be ready to drag and drop to add data to the portal.

_______

## To Use this Example:

#### Prepare and set up PlatformIO
- Register a site and sensors at the WikiWatershed/EnviroDIY data portal (http://data.WikiWatershed.org/)
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.  Without this, the program won't compile or send data.
- Download logging_to_EnviroDIY.ino and put it into the src directory of your project.  Delete main.cpp in that folder.

#### Modify the Example
- Modify logging_to_EnviroDIY_Zero.ino to have the modem, sensor, and variable objects that you are interested in.
    - This example is written for a _Digi XBee S6B (wifi)_ modem.  Change this to whatever modem you are using.  Pastable chunks of code for each modem are available in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home).
    - Don't forget to put in your wifi username/password or cellular APN!
    - There are no sensors included in this example.  Add code for all of your sensors.  See the pages for the individual sensors in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home) for code snippets/examples.
- **Make sure the pin numbers and serial ports selected in your code match with how things are physically attached to your board!**
- Add all of your variables to your variable array.
- Paste all of your variable UUID's for your site into your variable array, replacing all of the ```"12345678-abcd-1234-efgh-1234567890ab"``` values.
- Replace the registration token and sampling feature values in the example with those for your site:

```cpp
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
```

#### Upload!
- Test everything at home **before** deploying out in the wild!

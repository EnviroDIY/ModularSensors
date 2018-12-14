# Example using the Modular Sensors Library to save data to an SD card and send data to the EnviroDIY Data Portal

This shows the use of an "EnviroDIY logger" object on a SAMD21 (Arduino Zero) board.  It is very similar to the logging_to_EnviroDIY example, except that it defines multiple hardware serial ports for the sensors that communicate with the logger using any sort of serial communication.  The SAMD boards have many more hardware serial ports available than the AVR boards, and those hardware ports should always be used before resorting to a software serial port.  The exact selection of pins and ports in this example uses the SerCom ports available on a Adafruit Feather M0.  The SerCom ports available do vary on different boards, so you must be aware of which ones are available for the board you are using.  Before using this example, you must register a site and sensors at the data portal (http://data.envirodiy.org/).  After you have registered the site and sensors, the portal will generate a registration token and universally unique identifier (UUID) for each site and further UUID's for each variable.  You will need to copy all of those UUID values into your sketch to replace the "12345678-abcd-1234-efgh-1234567890ab" place holders in this example.  Please do not try to run the example exactly as written, but delete the chunks of code pertaining to sensors that you do not have attached.


To Use this Example:
- Register a site and sensors at the WikiWatershed/EnviroDIY data portal (http://data.WikiWatershed.org/)
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.  Without this, the program won't compile or send data.
- Download logging_to_EnviroDIY.ino and put it into the src directory of your project.  Delete main.cpp in that folder.
- Modify logging_to_EnviroDIY.ino to have the only sensor and variable objects that you are interested.  Also make sure you have the correct modem configurations.  See the pages for the individual sensors and modems in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home) for code snippets/examples.  _Please do not try to run the example exactly as written, but delete the chunks of code pertaining to sensors that you do not have attached._
    - Don't forget to put in your wifi username/password or cellular APN!
    - Pay special attention to the pins and SERCOM assignments if you are using more than one Serial device!
- Paste all of your variable UUID's for your site into your variable array, replacing all of the ```12345678-abcd-1234-efgh-1234567890ab``` values.
- Replace the registration token and sampling feature values in the example with those for your site:
```cpp
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
```
- Upload and test everything at home **before** deploying out in the wild!

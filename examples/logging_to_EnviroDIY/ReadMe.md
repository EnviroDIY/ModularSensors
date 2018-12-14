# Example using the Modular Sensors Library to save data to an SD card and send data to the EnviroDIY Data Portal

This shows the use of an "EnviroDIY logger" object for an AVR board.  It takes the simple_logging example one step farther in creating a modem object that is used to send data to the EnviroDIY data portal.

Before using this example, you must register a site and sensors at the data portal (http://data.envirodiy.org/).  After you have registered the site and sensors, the portal will generate a registration token and universally unique identifier (UUID) for each site and further UUID's for each variable.  You will need to copy all of those UUID values into your sketch to replace the ```12345678-abcd-1234-efgh-1234567890ab``` place holders in this example.

This is the example you should use for any site that you want to have tied to the WikiWatershed, even if you won't be sending live data.  In cases where you won't use the live data, delete all of the lines pertaining to the modem and make sure your loop is set to ```logData()``` rather than ```logDataAndSend()```.  This ensures that you the file written to your SD card will be ready to drag and drop to add data to the portal.

To Use this Example:
- Register a site and sensors at the WikiWatershed/EnviroDIY data portal (http://data.WikiWatershed.org/)
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.  Without this, the program won't compile or send data.
- Download logging_to_EnviroDIY.ino and put it into the src directory of your project.  Delete main.cpp in that folder.
- Modify logging_to_EnviroDIY.ino to have the only sensor and variable objects that you are interested.  Also make sure you have the correct modem configurations.  See the pages for the individual sensors and modems in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home) for code snippets/examples.  _Please do not try to run the example exactly as written, but delete the chunks of code pertaining to sensors that you do not have attached._
    - Don't forget to put in your wifi username/password or cellular APN!
- Paste all of your variable UUID's for your site into your variable array, replacing all of the ```12345678-abcd-1234-efgh-1234567890ab``` values.
- Replace the registration token and sampling feature values in the example with those for your site:
```cpp
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
```
- Upload and test everything at home **before** deploying out in the wild!

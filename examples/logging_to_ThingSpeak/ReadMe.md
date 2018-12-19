# Example using the Modular Sensors Library to save data to an SD card and send data to [ThingSpeak](https://thingspeak.com)

This shows the use of a "ThingSpeak logger" object.  Data is sent to ThingSpeak using MQTT.

_______

## To Use this Example:

#### Prepare and set up PlatformIO
- Create a channel on ThingSpeak with fields to receive your data.
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.  Without this, the program won't compile or send data.
- Download logging_to_ThingSpeak.ino and put it into the src directory of your project.  Delete main.cpp in that folder.

#### Modify the Example
- Modify logging_to_ThingSpeak.ino to have the sensor and variable objects that you are interested.  Also make sure you have the correct modem configurations.  See the pages for the individual sensors and modems in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home) for code snippets/examples.
    - Don't forget to put in your wifi username/password or cellular APN!
- Order the variables in your variable array in the same order as your fields are on ThingSpeak.
    - This order is __crucial__.  The results from the variables in the VariableArray will be sent to ThingSpeak in the order they are in the array; that is, the first variable in the array will be sent as Field1, the second as Field2, etc.
    - Any UUID's or custom variable codes are ignored for ThingSpeak.  They will only appear in the header of your file on the SD card.
- Find this information for your ThingSpeak account and channel and put it into logging_to_ThingSpeak.ino:

```cpp
const char *thingSpeakMQTTKey = "XXXXXXXXXXXXXXXX";  // Your MQTT API Key from Account > MyProfile.
const char *thingSpeakChannelID = "######";  // The numeric channel id for your channel
const char *thingSpeakChannelKey = "XXXXXXXXXXXXXXXX";  // The Write API Key for your channel
```

#### Upload!
- Test everything at home **before** deploying out in the wild!

[//]: # ( @page example_thingspeak ThingSpeak Example )
# Using ModularSensors to save data to an SD card and send data to ThingSpeak

This shows the use of a "ThingSpeak logger" object.
Data is sent to [ThingSpeak](https://thingspeak.com) using MQTT.

_______

[//]: # ( @section thingspeak_using To Use this Example: )
## To Use this Example:

[//]: # ( @subsection thingspeak_pio Prepare and set up PlatformIO )
## Prepare and set up PlatformIO
- Create a channel on ThingSpeak with fields to receive your data.
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your platformio configuration has the lib_ldf_mode and build flags set as they are in the example.
Without this, the program won't compile or send data.
- Download logging_to_ThingSpeak.ino and put it into the src directory of your project.
Delete main.cpp in that folder.

[//]: # ( @subsection thingspeak_modify Modify the Example )
## Modify the Example
- Modify logging_to_ThingSpeak.ino to have the modem, sensor, and variable objects that you are interested in.
    - This example is written for an _ESP8266 (wifi)_ modem.
Change this to whatever modem you are using.
Pastable chunks of code for each modem are available in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home).
    - Don't forget to put in your wifi username/password or cellular APN!
    - This example is written for a Campbell OBS3+ and a Meter Hydros 21.
Remove those sensors if you are not using them and add code for all of your sensors.
See the pages for the individual sensors in the [wiki](https://github.com/EnviroDIY/ModularSensors/wiki/Home) for code snippets/examples.
        - Remember, no more than **8** variables/fields can be sent to a single ThingSpeak channel.
If you want to send data to multiple channels, you must create individual logger objects with unique publishers attached for each channel you want to send to.
- **Make sure the pin numbers and serial ports selected in your code match with how things are physically attached to your board!**
- Order the variables in your variable array in the same order as your fields are on ThingSpeak.
    - This order is __crucial__.
The results from the variables in the VariableArray will be sent to ThingSpeak in the order they are in the array; that is, the first variable in the array will be sent as Field1, the second as Field2, etc.
    - Any UUID's or custom variable codes are ignored for ThingSpeak.
They will only appear in the header of your file on the SD card.
- Find this information for your ThingSpeak account and channel and put it into logging_to_ThingSpeak.ino:

```cpp
const char *thingSpeakMQTTKey = "XXXXXXXXXXXXXXXX";  // Your MQTT API Key from Account > MyProfile.
const char *thingSpeakChannelID = "######";  // The numeric channel id for your channel
const char *thingSpeakChannelKey = "XXXXXXXXXXXXXXXX";  // The Write API Key for your channel
```

[//]: # ( @subsection thingspeak_upload Upload! )
## Upload!
- Test everything at home **before** deploying out in the wild!

_______

[//]: # ( @section thingspeak_full The Complete Example Code: )

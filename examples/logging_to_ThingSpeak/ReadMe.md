# Sending data to ThingSpeak <!-- {#example_thingspeak} -->

This shows the use of a "ThingSpeak logger" object.
Data is sent to [ThingSpeak](https://thingspeak.com) using MQTT.

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [Sending data to ThingSpeak](#sending-data-to-thingspeak)
- [Unique Features of the ThingSpeak Example](#unique-features-of-the-thingspeak-example)
- [To Use this Example](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Modify the Example](#modify-the-example)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

# Unique Features of the ThingSpeak Example <!-- {#example_thingspeak_unique} -->
- A single logger publishes data to ThingSpeak.
- Uses an Espressif ESP8266 to publish data.

# To Use this Example <!-- {#example_thingspeak_using} -->

## Prepare and set up PlatformIO <!-- {#example_thingspeak_pio} -->
- Create a channel on ThingSpeak with fields to receive your data.
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/logging_to_ThingSpeak/platformio.ini) file in the examples/logging_to_ThingSpeak folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [logging_to_ThingSpeak.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/logging_to_ThingSpeak/logging_to_ThingSpeak.ino) and save it to your computer.
    - After opening the link, you should be able to right click anywhere on the page and select "Save Page As".
    - Move it into the src directory of your project.
    - Delete main.cpp in that folder.

## Modify the Example <!-- {#example_thingspeak_modify} -->
- Modify logging_to_ThingSpeak.ino to have the modem, sensor, and variable objects that you are interested in.
    - This example is written for an _ESP8266 (wifi)_ modem.
Change this to whatever modem you are using.
Pastable chunks of code for each modem are available in the individual sensor documentation or in the menu a la carte example.
    - Don't forget to put in your wifi username/password or cellular APN!
    - This example is written for a Campbell OBS3+ and a Meter Hydros 21.
Remove those sensors if you are not using them and add code for all of your sensors.
See the pages for the individual sensors in the [documentation](https://envirodiy.github.io/ModularSensors/index.html) for code snippets/examples.
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

## Upload! <!-- {#example_thingspeak_upload} -->
- Test everything at home **before** deploying out in the wild!

_______


[//]: # ( @section example_thingspeak_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} logging_to_ThingSpeak/platformio.ini )

[//]: # ( @section example_thingspeak_code The Complete Code )

[//]: # ( @include{lineno} logging_to_ThingSpeak/logging_to_ThingSpeak.ino )

# Sending Data to Monitor My Watershed/EnviroDIY <!-- {#example_mmw} -->

This sketch reduces menu_a_la_carte.ino to provide an example of how to log to https://monitormywatershed.org/ from two sensors, the BME280 and DS18. To complete the set up for logging to the web portal, the UUIDs for the site and each variable would need to be added to the sketch.

The settings for other data portals were removed from the example.

The modem settings were left unchanged because the sketch will test successfully without modem connection (wait patiently, it takes a few minutes).

This is the example you should use to deploy a logger with a modem to stream live data to the Monitor My Watershed data portal.

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [Sending Data to Monitor My Watershed/EnviroDIY](#sending-data-to-monitor-my-watershedenvirodiy)
- [Unique Features of the Monitor My Watershed Example](#unique-features-of-the-monitor-my-watershed-example)
- [To Use this Example](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Set the universally universal identifiers (UUID) for each variable](#set-the-universally-universal-identifiers-uuid-for-each-variable)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

# Unique Features of the Monitor My Watershed Example <!-- {#example_mmw_unique} -->
- A single logger publishes data to the Monitor My Watershed data portal.
- Uses a cellular Digi XBee or XBee3

# To Use this Example <!-- {#example_mmw_using} -->

## Prepare and set up PlatformIO <!-- {#example_mmw_pio} -->
- Register a site and sensors at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/logging_to_MMW/platformio.ini) file in the examples/logging_to_MMW folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [logging_to_MMW.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/logging_to_MMW/logging_to_MMW.ino) and save it to your computer.
    - After opening the link, you should be able to right click anywhere on the page and select "Save Page As".
    - Move it into the src directory of your project.
    - Delete main.cpp in that folder.

## Set the logger ID <!-- {#example_mmw_logger_id} -->
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

## Set the universally universal identifiers (UUID) for each variable <!-- {#example_mmw_uuids} -->
- Go back to the web page for your site at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- For each variable, find the dummy UUID (`"12345678-abcd-1234-ef00-1234567890ab"`) and replace it with the real UUID for the variable.

## Upload! <!-- {#example_mmw_upload} -->
- Test everything at home **before** deploying out in the wild!

_______

[//]: # ( @section example_mmw_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} logging_to_MMW/platformio.ini )

[//]: # ( @section example_mmw_code The Complete Code )

[//]: # ( @include{lineno} logging_to_MMW/logging_to_MMW.ino )

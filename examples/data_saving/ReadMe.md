# Minimizing Cellular Data Use <!-- {#example_data_saving} -->

This is another double logger example, but in this case, both loggers are going at the same interval and the only difference between the loggers is the list of variables.
There are two sets of variables, all coming from Yosemitech sensors.
Because each sensor outputs temperature and we don't want to waste cellular data sending out multiple nearly identical temperature values, we have one logger that logs every possible variable result to the SD card and another logger that sends only unique results to the EnviroDIY data portal.

The modem used in this example is a SIM800 based Sodaq GPRSBee r6.

The sensors used in this example are Yosemitech Y504 Dissolved Oxygen Sensor, Yosemitech Y511 Turbidity Sensor with Wiper, Yosemitech Y514 Chlorophyll Sensor, and Yosemitech Y520 Conductivity Sensor.

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [Minimizing Cellular Data Use](#minimizing-cellular-data-use)
- [Unique Features of the Data Saving Example](#unique-features-of-the-data-saving-example)
- [To Use this Example](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Set the universally universal identifiers (UUID) for each variable](#set-the-universally-universal-identifiers-uuid-for-each-variable)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

# Unique Features of the Data Saving Example <!-- {#example_data_saving_unique} -->
- Uses AltSoftSerial to create an additional serial port for RS485 communication.
- All variables are created and named with their parent sensor (as opposed to being created within the variable array).
- Two different variable arrays and loggers are created and used.
  - Many of the same variables are used in both arrays.
  - Only one of the loggers publishes data.
- The `loop` function is expanded into its components rather than using the `logData` functions.
  - This demonstrates *how* to write the loop out, without using the `logData` functions.
  - It also shows how to forcibly set serial pins `LOW` at the start and end of the loop in order to prevent power loss through an RS485 adapter.

# To Use this Example <!-- {#example_data_saving_using} -->

## Prepare and set up PlatformIO <!-- {#example_data_saving_pio} -->
- Register a site and sensors at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/data_saving/platformio.ini) file in the examples/data_saving folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [data_saving.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/data_saving/data_saving.ino) and save it to your computer.
    - After opening the link, you should be able to right click anywhere on the page and select "Save Page As".
    - Move it into the src directory of your project.
    - Delete main.cpp in that folder.

## Set the logger ID <!-- {#example_data_saving_logger_id} -->
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

## Set the universally universal identifiers (UUID) for each variable <!-- {#example_data_saving_uuids} -->
- Go back to the web page for your site at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- For each variable, find the dummy UUID (`"12345678-abcd-1234-ef00-1234567890ab"`) and replace it with the real UUID for the variable.

## Upload! <!-- {#example_data_saving_upload} -->
- Test everything at home **before** deploying out in the wild!

_______


[//]: # ( @section example_data_saving_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} data_saving/platformio.ini )

[//]: # ( @section example_data_saving_code The Complete Code )

[//]: # ( @include{lineno} data_saving/data_saving.ino )

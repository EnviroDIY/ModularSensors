# Calculating Results based on Measured Values <!-- {#example_baro_rho} -->

This example demonstrates how to work with calculated variables and calculates water depth by correcting the total pressure measured by a MeaSpec MS5803 with the atmospheric pressure measured by a Bosch BME280 environmental sensor and the temperature measured by a Maxim DS18 temperature probe.

The modem used in this example is a SIM800 based Sodaq GPRSBee r6.

The sensors used in this example are a Maxim DS18 temperature probe, a Bosch BME280 environmental sensor, and a Measurement Specialties MS5803-14BA pressure sensor.

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [Calculating Results based on Measured Values](#calculating-results-based-on-measured-values)
- [Unique Features of the Barometric Correction Example](#unique-features-of-the-barometric-correction-example)
- [To Use this Example](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Set the universally universal identifiers (UUID) for each variable](#set-the-universally-universal-identifiers-uuid-for-each-variable)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

# Unique Features of the Barometric Correction Example <!-- {#example_baro_rho_unique} -->
- All variables are created and named with their parent sensor (as opposed to being created within the variable array).
- There are multiple calculated variables created and used.

# To Use this Example <!-- {#example_baro_rho_using} -->

## Prepare and set up PlatformIO <!-- {#example_baro_rho_pio} -->
- Register a site and sensors at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/baro_rho_correction/platformio.ini) file in the examples/baro_rho_correction folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [baro_rho_correction.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/baro_rho_correction/baro_rho_correction.ino) and save it to your computer.
    - After opening the link, you should be able to right click anywhere on the page and select "Save Page As".
    - Move it into the src directory of your project.
    - Delete main.cpp in that folder.

## Set the logger ID <!-- {#example_baro_rho_logger_id} -->
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

## Set the universally universal identifiers (UUID) for each variable <!-- {#example_baro_rho_uuids} -->
- Go back to the web page for your site at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- For each variable, find the dummy UUID (`"12345678-abcd-1234-ef00-1234567890ab"`) and replace it with the real UUID for the variable.

## Upload! <!-- {#example_baro_rho_upload} -->
- Test everything at home **before** deploying out in the wild!

_______

[//]: # ( @section example_baro_rho_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} baro_rho_correction/platformio.ini )

[//]: # ( @section example_baro_rho_code The Complete Code )

[//]: # ( @include{lineno} baro_rho_correction/baro_rho_correction.ino )

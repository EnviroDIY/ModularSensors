[//]: # ( @page example_baro_rho Barometric Pressure Correction )
# Using ModularSensors to Calculate Results based on Measured Values

This example demonstrates how to work with calculated variables and calculates water depth by correcting the total pressure measured by a MeaSpec MS5803 with the atmospheric pressure measured by a Bosch BME280 environmental sensor and the temperature measured by a Maxim DS18 temperature probe.

The modem used in this example is a SIM800 based Sodaq GPRSBee r6.

The sensors used in this example are a Maxim DS18 temperature probe, a Bosch BME280 environmental sensor, and a Measurement Specialties MS5803-14BA pressure sensor.

_______

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to Calculate Results based on Measured Values](#using-modularsensors-to-calculate-results-based-on-measured-values)
- [Unique Features of the Barometric Correction Example](#unique-features-of-the-barometric-correction-example)
- [To Use this Example:](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Set the universally universal identifiers (UUID) for each variable](#set-the-universally-universal-identifiers-uuid-for-each-variable)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

[//]: # ( @section example_baro_rho_unique Unique Features of the Barometric Correction Example )
# Unique Features of the Barometric Correction Example
- All variables are created and named with their parent sensor (as opposed to being created within the variable array).
- There are multiple calculated variables created and used.

[//]: # ( @section example_baro_rho_using To Use this Example: )
# To Use this Example:

[//]: # ( @subsection example_baro_rho_pio Prepare and set up PlatformIO )
## Prepare and set up PlatformIO
- Register a site and sensors at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- Create a new PlatformIO project
- Copy and paste the contents of the platformio.ini file in this example into the platformio.ini for your new project
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
Without this, the program won't compile or send data.
- Download baro_rho_correction.ino and put it into the src directory of your project.
    - Delete main.cpp in that folder.

[//]: # ( @subsection example_baro_rho_logger_id Set the logger ID )
## Set the logger ID
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

[//]: # ( @subsection example_baro_rho_uuids Set the universally universal identifiers (UUID) for each variable )
## Set the universally universal identifiers (UUID) for each variable
- Go back to the web page for your site at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- For each variable, find the dummy UUID (`"12345678-abcd-1234-ef00-1234567890ab"`) and replace it with the real UUID for the variable.

[//]: # ( @subsection example_baro_rho_upload Upload! )
## Upload!
- Test everything at home **before** deploying out in the wild!

_______

[//]: # ( @section example_baro_rho_pio PlatformIO Configuration )

[//]: # ( @include{lineno} baro_rho_correction/platformio.ini )

[//]: # ( @section example_baro_rho_code The Complete Code )

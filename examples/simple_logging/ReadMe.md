[//]: # ( @page example_simple_logging Simple Logging Example )
# Using ModularSensors to save data to an SD card

This shows the simplest use of a "logger" object.
That is, creating an array of variable objects and then creating a logger object that utilizes those variables to update all of the variable results together and save the data to a SD card.
The processor then goes to sleep between readings.

This is the example you should use to deploy a logger somewhere where you don't want or have access to a way of streaming live data and you won't want to upload data to the Monitor My Watershed data portal.

_______

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to save data to an SD card](#using-modularsensors-to-save-data-to-an-sd-card)
- [Unique Features of the Simple Logging Example](#unique-features-of-the-simple-logging-example)
- [To Use this Example:](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

[//]: # ( @section example_simple_logging_unique Unique Features of the Simple Logging Example )
# Unique Features of the Simple Logging Example
- Only logs data to an SD card.

[//]: # ( @section example_simple_logging_using To Use this Example: )
# To Use this Example:

[//]: # ( @subsection example_simple_logging_pio Prepare and set up PlatformIO )
## Prepare and set up PlatformIO
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/simple_logging/platformio.ini) file in the examples/simple_logging folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [simple_logging.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/simple_logging/simple_logging.ino) and save it to your computer.  Put it into the src directory of your project.
    - Delete main.cpp in that folder.

[//]: # ( @subsection example_simple_logging_logger_id Set the logger ID )
## Set the logger ID
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

[//]: # ( @subsection example_simple_logging_upload Upload! )
## Upload!
- Test everything at home **before** deploying out in the wild!

_______


[//]: # ( @section example_simple_logging_pio_config PlatformIO Configuration \)

[//]: # ( @include{lineno} simple_logging/platformio.ini )

[//]: # ( @section example_simple_logging_code The Complete Code )

[//]: # ( @page example_double_log Double %Logger Example )
# Using ModularSensors to Record data from Two Different Groups of Sensors at Two Different Time Intervals

This is a more complicated example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.
This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.

_______

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to Record data from Two Different Groups of Sensors at Two Different Time Intervals](#using-modularsensors-to-record-data-from-two-different-groups-of-sensors-at-two-different-time-intervals)
- [Unique Features of the Double Logger Example](#unique-features-of-the-double-logger-example)
- [To Use this Example:](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

[//]: # ( @section example_double_log_unique Unique Features of the Double %Logger Example )
# Unique Features of the Double Logger Example
- Two different variable arrays and loggers are created and used.
  - The Variables for the arrays are created within the array.
  - There is no variable overlap between the two arrays or loggers.
- The `loop` function is expanded into its components rather than using the `logData` functions.
  - This demonstrates *how* to write the loop out, without using the `logData` functions.
  - This shows which functions are required for each of the two loggers and which can be used in common.

[//]: # ( @section example_double_log_using To Use this Example: )
# To Use this Example:

[//]: # ( @subsection example_double_log_pio Prepare and set up PlatformIO )
## Prepare and set up PlatformIO
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/double_logger/platformio.ini) file in the examples/double_logger folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [double_logger.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/double_logger/double_logger.ino) and save it to your computer.
    - After opening the link, you should be able to right click anywhere on the page and select "Save Page As".
    - Move it into the src directory of your project.
    - Delete main.cpp in that folder.

[//]: # ( @subsection example_double_log_logger_id Set the logger ID )
## Set the logger ID
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

[//]: # ( @subsection example_double_log_upload Upload! )
## Upload!
- Test everything at home **before** deploying out in the wild!

_______


[//]: # ( @section example_double_log_pio PlatformIO Configuration )

[//]: # ( @include{lineno} double_logger/platformio.ini )

[//]: # ( @section example_double_log_code The Complete Code )

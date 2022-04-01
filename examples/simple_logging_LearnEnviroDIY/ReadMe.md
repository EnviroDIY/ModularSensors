# Learn EnviroDIY Course <!-- {#example_learn_envirodiy} -->

This shows the simplest use of a "logger" object.
That is, creating an array of variable objects and then creating a logger object that utilizes those variables to update all of the variable results together and save the data to a SD card.
The processor then goes to sleep between readings.
This example calls on two of the sensors available in this library.
The example may be run exactly as written.

This is the example you should use to deploy a logger somewhere where you don't want or have access to a way of streaming live data and you won't want to upload data to the Monitor My Watershed data portal.

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [Learn EnviroDIY Course](#learn-envirodiy-course)
- [Unique Features of the Learn EnviroDIY Example](#unique-features-of-the-learn-envirodiy-example)
- [To Use this Example](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

# Unique Features of the Learn EnviroDIY Example <!-- {#example_learn_envirodiy_unique} -->
- Only logs data to an SD card.
- Uses a few more sensors than the other simple logging example

# To Use this Example <!-- {#example_learn_envirodiy_using} -->

## Prepare and set up PlatformIO <!-- {#example_learn_envirodiy_pio} -->
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/simple_logging_LearnEnviroDIY/platformio.ini) file in the examples/simple_logging_LearnEnviroDIY folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [simple_logging_LearnEnviroDIY.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/simple_logging_LearnEnviroDIY/simple_logging_LearnEnviroDIY.ino) and save it to your computer.  Put it into the src directory of your project.
    - Delete main.cpp in that folder.

## Set the logger ID <!-- {#example_learn_envirodiy_logger_id} -->
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

## Upload! <!-- {#example_learn_envirodiy_upload} -->
- Test everything at home **before** deploying out in the wild!

_______



[//]: # ( @section example_learn_envirodiy_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} simple_logging_LearnEnviroDIY/platformio.ini )

[//]: # ( @section example_learn_envirodiy_code The Complete Code )

[//]: # ( @include{lineno} simple_logging_LearnEnviroDIY/simple_logging_LearnEnviroDIY.ino )

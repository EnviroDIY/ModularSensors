[//]: # ( @page example_single_sensor Single Sensor Example )
# Using ModularSensors to Communicate with a Single Sensor

This somewhat trivial example show making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.
It also shows creating a calculated variable which is the water depth.

_______

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to Communicate with a Single Sensor](#using-modularsensors-to-communicate-with-a-single-sensor)
- [Unique Features of the Single Sensor Example](#unique-features-of-the-single-sensor-example)
- [To Use this Example:](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

[//]: # ( @section example_single_sensor_unique Unique Features of the Single Sensor Example )
# Unique Features of the Single Sensor Example
- Only communicates with and collects data from a single sensor.
- Does not make use of any VariableArray or logging features.

[//]: # ( @section example_single_sensor_using To Use this Example: )
# To Use this Example:

[//]: # ( @subsection example_single_sensor_pio Prepare and set up PlatformIO )
## Prepare and set up PlatformIO
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/single_sensor/platformio.ini) file in the examples/single_sensor folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [single_sensor.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/single_sensor/single_sensor.ino) and save it to your computer.  Put it into the src directory of your project.
    - Delete main.cpp in that folder.

[//]: # ( @subsection example_single_sensor_upload Upload! )
## Upload!
- Upload and see what happens

_______


[//]: # ( @section example_single_sensor_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} single_sensor/platformio.ini )

[//]: # ( @section example_single_sensor_code The Complete Code )

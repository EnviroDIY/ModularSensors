[//]: # ( @page baro_rho_walkthrough Barometric Correction Walk-Through )
# Barometric Correction Example Walk-Through

_NOTE:  The code snippets in this walkthrough may not appear on GitHub._

[//]: # ( @copydoc baro_rho_example )
___

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Barometric Correction Example Walk-Through](#barometric-correction-example-walk-through)
  - [Defines and Includes](#defines-and-includes)
    - [Defines for the Arduino IDE](#defines-for-the-arduino-ide)
    - [Library Includes](#library-includes)
  - [Logger and Modem Settings](#logger-and-modem-settings)
    - [Logger Settings](#logger-settings)
    - [Wifi/Cellular Modem Settings](#wificellular-modem-settings)
  - [Sensors and Measured Variables](#sensors-and-measured-variables)
    - [The processor as a sensor](#the-processor-as-a-sensor)
    - [Maxim DS3231 RTC as a sensor](#maxim-ds3231-rtc-as-a-sensor)
    - [Bosch BME280 environmental sensor](#bosch-bme280-environmental-sensor)
    - [Maxim DS18 one wire temperature sensor](#maxim-ds18-one-wire-temperature-sensor)
    - [Measurement Specialties MS503 pressure and temperature sensor](#measurement-specialties-ms503-pressure-and-temperature-sensor)
  - [Calculated Variables](#calculated-variables)
    - [Water pressure](#water-pressure)
    - [Raw water depth](#raw-water-depth)
    - [Corrected water depth](#corrected-water-depth)
  - [Creating the array, logger, publishers](#creating-the-array-logger-publishers)
    - [The variable array](#the-variable-array)
    - [The Logger Object](#the-logger-object)
    - [Data Publisher](#data-publisher)
  - [Extra Working Functions](#extra-working-functions)
  - [Arduino Setup Function](#arduino-setup-function)
  - [Arduino Loop Function](#arduino-loop-function)
[//]: # ( End GitHub Only )


[//]: # ( @section baro_rho_defines-and-includes Defines and Includes )
## Defines and Includes

[//]: # ( @subsection baro_rho_defines Defines for the Arduino IDE )
### Defines for the Arduino IDE
The top few lines of the examples set defines of buffer sizes and yields needed for the Arduino IDE.
That IDE read any defines within the top few lines and applies them as build flags for the processor.

[//]: # ( @snippet{lineno} baro_rho_correction.ino defines )

If you are using PlatformIO, you should instead set these as build flags in your platformio.ini.
This is standard behaviour for C++.

```ini
build_flags =
    -DSDI12_EXTERNAL_PCINT
    -DNEOSWSERIAL_EXTERNAL_PCINT
    -DMQTT_MAX_PACKET_SIZE=240
    -DTINY_GSM_RX_BUFFER=64
    -DTINY_GSM_YIELD_MS=2
```
___

[//]: # ( @subsection baro_rho_includes Library Includes )
### Library Includes

Next, include the libraries needed for every program using ModularSensors.

[//]: # ( @snippet{lineno} baro_rho_correction.ino includes )
___

[//]: # ( @section baro_rho_logger-and-modem-settings Logger and Modem Settings )
## Logger and Modem Settings

[//]: # ( @subsection baro_rho_logger_settings Logger Settings )
### Logger Settings

Set options for the logging and dataLogger object.
This includes setting the time zone (daylight savings time is **NOT** applied) and setting all of the input and output pins related to the logger.

[//]: # ( @snippet{lineno} baro_rho_correction.ino logger_settings )
___

[//]: # ( @subsection baro_rho_modem-settings Wifi/Cellular Modem Settings )
### Wifi/Cellular Modem Settings

Now set up the modem and the internet connection options.
This examples is using a Sodaq GPRSBee R6 or R7.

[//]: # ( @snippet{lineno} baro_rho_correction.ino modem_settings )
___

[//]: # ( @section baro_rho_sensors_and_vars Sensors and Measured Variables )
## Sensors and Measured Variables

[//]: # ( @subsection baro_rho_processor_sensor The processor as a sensor )
### The processor as a sensor

Set options for using the processor as a sensor to report battery level, processor free ram, and sample number.
Note that because we intend to use all of our variables in two places, we are creating the Variable objects here instead of just creating the Sensor.

[//]: # ( @snippet{lineno} baro_rho_correction.ino processor_sensor )
___

[//]: # ( @subsection baro_rho_ds3231 Maxim DS3231 RTC as a sensor )
### Maxim DS3231 RTC as a sensor

In addition to the time, we can also use the required DS3231 real time clock to report the temperature of the circuit board.
This temperature is _not_ equivalent to an environmental temperature measurement and should only be used to as a diagnostic.
As above, we create both the sensor and the variables measured by it.

[//]: # ( @snippet{lineno} baro_rho_correction.ino ds3231 )
___

[//]: # ( @subsection baro_rho_bme280 Bosch BME280 environmental sensor )
### Bosch BME280 environmental sensor

Set options for the Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
Create the #Sensor object and all of the #Variable objects.

[//]: # ( @snippet{lineno} baro_rho_correction.ino bme280 )
___

[//]: # ( @subsection baro_rho_ds18 Maxim DS18 one wire temperature sensor )
### Maxim DS18 one wire temperature sensor

Set options for the Maxim DS18 One Wire Temperature Sensor
Create the #Sensor object and all of the #Variable objects.

[//]: # ( @snippet{lineno} baro_rho_correction.ino ds18 )
___

[//]: # ( @subsection baro_rho_ms5803  Measurement Specialties MS503 pressure and temperature sensor )
###  Measurement Specialties MS503 pressure and temperature sensor

Set options for the Measurement Specialties MS503 pressure and temperature sensor.
Create the #Sensor object and all of the #Variable objects.

[//]: # ( @snippet{lineno} baro_rho_correction.ino ms5803 )
___

[//]: # ( @section baro_rho_name_me Calculated Variables )
## Calculated Variables

Create new #Variable objects calculated from the measured variables.
For these calculate variables, we must not only supply a function for the calculation, but also all of the metadata about the variable - like the name of the variable and its units.

[//]: # ( @subsection baro_rho_name_me Water pressure )
### Water pressure

The water pressure is calculated by subtracting the atmospheric preasure measured by the BME280 from the total pressure from both water and atmosphere measured by the MS5803.

[//]: # ( @snippet{lineno} baro_rho_correction.ino calculated_pressure )
___

[//]: # ( @subsection baro_rho_name_me Raw water depth )
### Raw water depth

The water depth is calculated from the water pressure, assuming pure water at 4°C.

[//]: # ( @snippet{lineno} baro_rho_correction.ino calculated_uncorrected_depth )
___

[//]: # ( @subsection baro_rho_name_me Corrected water depth )
### Corrected water depth

The water depth measurement can be improved by adjusting for the density of water at the real water temperature as measured by the DS18.

[//]: # ( @snippet{lineno} baro_rho_correction.ino calculated_corrected_depth )
___

[//]: # ( @section baro_rho_name_me Creating the array, logger, publishers )
## Creating the array, logger, publishers

[//]: # ( @subsection baro_rho_name_me The variable array )
### The variable array

Create a #VariableArray containing all of the #Variable objects that we are logging the values of.
Since we've created all of the variables above, we only need to call them by name here.

[//]: # ( @snippet{lineno} baro_rho_correction.ino variable_arrays )
___

[//]: # ( @subsection baro_rho_name_me The Logger Object )
### The Logger Object

Now that we've created the array, we can actually create the #Logger object.

[//]: # ( @snippet{lineno} baro_rho_correction.ino loggers )
___

[//]: # ( @subsection baro_rho_name_me Data Publisher )
### Data Publisher

Finally, create a #dataPublisher to the [Monitor My Watershed / EnviroDIY Data Sharing Portal.](http://monitormywatershed.org/)

[//]: # ( @snippet{lineno} baro_rho_correction.ino publishers )
___

[//]: # ( @section baro_rho_name_me Extra Working Functions )
## Extra Working Functions

Here we're creating a few extra functions on the global scope.
The flash function is used at board start up just to give an indication that the board has restarted.
The battery function calls the #ProcessorStats sensor to check the battery level before attempting to log or publish data.

[//]: # ( @snippet{lineno} baro_rho_correction.ino working_functions )
___

[//]: # ( @section baro_rho_name_me Arduino Setup Function )
## Arduino Setup Function

This is our setup function.
In Arduino coding, the classic "main" function is replaced by two functions: setup() and loop().
The setup() function runs once when the board boots or restarts.
It usually contains many functions that set the mode of input and output pins and prints out some debugging information to the serial port.
These functions are frequently named "begin".

[//]: # ( @snippet{lineno} baro_rho_correction.ino setup )
___

[//]: # ( @section baro_rho_name_me Arduino Loop Function )
## Arduino Loop Function

This is the loop function which will run repeatedly as long as the board is turned on.

[//]: # ( @snippet{lineno} baro_rho_correction.ino loop )
[//]: # ( @page menu_walkthrough The a la carte Menu Walk-Through )
# The a la carte Menu Walk-Through

_NOTE:  The code snippets in this walkthrough may not appear on GitHub._

[//]: # ( @copydoc menu_example )
___

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [The a la carte Menu Walk-Through](#the-a-la-carte-menu-walk-through)
  - [Defines and Includes](#defines-and-includes)
    - [Defines for the Arduino IDE](#defines-for-the-arduino-ide)
    - [Library Includes](#library-includes)
  - [Logger and Modem Settings](#logger-and-modem-settings)
    - [Extra Serial Ports](#extra-serial-ports)
      - [AVR Boards](#avr-boards)
    - [Logger Settings](#logger-settings)
    - [Wifi/Cellular Modem Settings](#wificellular-modem-settings)
  - [Sensors and Measured Variables](#sensors-and-measured-variables)
    - [The processor as a sensor](#the-processor-as-a-sensor)
    - [Maxim DS3231 RTC as a sensor](#maxim-ds3231-rtc-as-a-sensor)
    - [Bosch BME280 environmental sensor](#bosch-bme280-environmental-sensor)
    - [Maxim DS18 one wire temperature sensor](#maxim-ds18-one-wire-temperature-sensor)
    - [Measurement Specialties MS503 pressure and temperature sensor](#measurement-specialties-ms503-pressure-and-temperature-sensor)
  - [Calculated Variables](#calculated-variables)
  - [Creating the array, logger, publishers](#creating-the-array-logger-publishers)
    - [The variable array](#the-variable-array)
    - [The Logger Object](#the-logger-object)
    - [Data Publisher](#data-publisher)
  - [Extra Working Functions](#extra-working-functions)
  - [Arduino Setup Function](#arduino-setup-function)
  - [Arduino Loop Function](#arduino-loop-function)
[//]: # ( End GitHub Only )



## Defines and Includes


### Defines for the Arduino IDE
The top few lines of the examples set defines of buffer sizes and yields needed for the Arduino IDE.
That IDE read any defines within the top few lines and applies them as build flags for the processor.
This is _not_ standard behavior for C++ (which is what Arduino code really is) - this is a unique aspect of the Arduino IDE.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino defines )

If you are using PlatformIO, you should instead set these as global build flags in your platformio.ini.
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


### Library Includes

Next, include the libraries needed for every program using ModularSensors.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino includes )
___


## Logger and Modem Settings

### Extra Serial Ports

Many different sensors communicate using some sort of serial or transistor-transistor-logic (TTL) protocol.
Among these are any sensors using RS232, RS485, RS422.
Generally each serial variant (or sometimes each sensor) needs a dedicated serial "port" - its own connection to the processor.
Most processors have built in dedicated wires for serial communication - "Hardware" serial.
See the page on [Arduino streams](@ref arduino_streams) for much more detail about serial connections with Arduino processors.

#### AVR Boards

Most Arduino AVR style boards have very few (ie, one, or none) dedicated serial ports _available_ after counting out the programming serial port.
So to connect anything else, we need to try to emulate the processor serial functionality with a software library.



This is probable the scariest part of this example


### Logger Settings

Here we set options for the logging and dataLogger object.
This includes setting the time zone (daylight savings time is **NOT** applied) and setting all of the input and output pins related to the logger.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino logger_settings )
___


### Wifi/Cellular Modem Settings

Now set up the modem and the internet connection options.
This examples is using a Sodaq GPRSBee R6 or R7.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino modem_settings )
___


## Sensors and Measured Variables


### The processor as a sensor

Set options for using the processor as a sensor to report battery level, processor free ram, and sample number.
Note that because we intend to use all of our variables in two places, we are creating the Variable objects here instead of just creating the Sensor.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino processor_sensor )
___


### Maxim DS3231 RTC as a sensor

In addition to the time, we can also use the required DS3231 real time clock to report the temperature of the circuit board.
This temperature is _not_ equivalent to an environmental temperature measurement and should only be used to as a diagnostic.
As above, we create both the sensor and the variables measured by it.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino ds3231 )
___


### Bosch BME280 environmental sensor

Set options for the Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
Create the #Sensor object and all of the #Variable objects.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino bme280 )
___


### Maxim DS18 one wire temperature sensor

Set options for the Maxim DS18 One Wire Temperature Sensor
Create the #Sensor object and all of the #Variable objects.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino ds18 )
___


###  Measurement Specialties MS503 pressure and temperature sensor

Set options for the Measurement Specialties MS503 pressure and temperature sensor.
Create the #Sensor object and all of the #Variable objects.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino ms5803 )
___


## Calculated Variables

Create new #Variable objects calculated from the measured variables.
For these calculate variables, we must not only supply a function for the calculation, but also all of the metadata about the variable - like the name of the variable and its units.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino calculated_corrected_depth )
___


## Creating the array, logger, publishers


### The variable array

Create a #VariableArray containing all of the #Variable objects that we are logging the values of.
Since we've created all of the variables above, we only need to call them by name here.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino variable_arrays )
___


### The Logger Object

Now that we've created the array, we can actually create the #Logger object.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino loggers )
___


### Data Publisher

Finally, create a #dataPublisher to the [Monitor My Watershed / EnviroDIY Data Sharing Portal.](http://monitormywatershed.org/)

[//]: # ( @snippet{lineno} menu_a_la_carte.ino publishers )
___


## Extra Working Functions

Here we're creating a few extra functions on the global scope.
The flash function is used at board start up just to give an indication that the board has restarted.
The battery function calls the #ProcessorStats sensor to check the battery level before attempting to log or publish data.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino working_functions )
___


## Arduino Setup Function

This is our setup function.
In Arduino coding, the classic "main" function is replaced by two functions: setup() and loop().
The setup() function runs once when the board boots or restarts.
It usually contains many functions that set the mode of input and output pins and prints out some debugging information to the serial port.
These functions are frequently named "begin".

[//]: # ( @snippet{lineno} menu_a_la_carte.ino setup )
___


## Arduino Loop Function

This is the loop function which will run repeatedly as long as the board is turned on.

[//]: # ( @snippet{lineno} menu_a_la_carte.ino loop )
# ModularSensors

This Arduino library gives environmental sensors a common interface of functions for use with Arduino-compatible dataloggers, such as the EnviroDIY Mayfly. The ModularSensors library is specifically designed to support wireless, solar-powered environmental data logging applications, that is, to:
* Retrieve data from many physical sensors;
* Save that data to a SD memory card;
* Transmit that data wirelessly to a web server; and
* Put the processor, sensors and all other peripherals to sleep between readings to conserve power.

The ModularSensors library coordinates these tasks by "wrapping" native sensor libraries into a common interface of functions and returns. These [wrapper functions](https://en.wikipedia.org/wiki/Wrapper_function) serve to harmonize and simplify the process of iterating through and logging data from a diverse set of sensors and variables.

Although this library was written primarily for the [EnviroDIY Mayfly data logger board](https://envirodiy.org/mayfly/), it is also designed to be [compatible with a variety of other Arduino-based boards](#compatibility) as well.

### Contents:
- [Getting Started](#getStarted)
  - [What are sensors and variables?](#whatHo)
  - [Physical Dependencies](#pdeps)
  - [Library Dependencies](#ldeps)
- [Basic Sensor and Variable Functions](#Basic)
    - [Individual Sensors Code Examples](#individuals)
- [Grouped Sensor Functions](#Grouped)
    - [VariableArray Code Examples](#ArrayExamples)
- Logger Functions
    - [Basic Logger Functions](#Logger)
    - [Modem and Internet Functions](#Modem)
    - [EnviroDIY Logger Functions](#DIYlogger)
    - [Logger Code Examples](#LoggerExamples)
- Available Sensors
    - [Apogee SQ-212: quantum light sensor, via TI ADS1115](#SQ212)
    - [AOSong AM2315: humidity & temperature](#AM2315)
    - [AOSong DHT: humidity & temperature](#DHT)
    - [Bosch BME280: barometric pressure, humidity & temperature](#BME280)
    - [Campbell Scientific OBS-3+: turbidity, via TI ADS1115](#OBS3)
    - [Meter Environmental ECH2O 5TM (formerly Decagon Devices 5TM): soil moisture](#5TM)
    - [Meter Environmental Hydros 21 (formerly Decagon Devices CTD-10): conductivity, temperature & depth](#CTD)
    - [Decagon Devices ES-2: conductivity ](#ES2)
    - [External I2C Rain Tipping Bucket Counter: rainfall totals](#ExtTips)
    - [External Voltage: via TI ADS1115](#ExtVolt)
    - [Freescale Semiconductor MPL115A2: barometric pressure and temperature](#MPL115A2)
    - [Keller Submersible Level Transmitters: pressure and temperature](#keller)
    - [MaxBotix MaxSonar: water level](#MaxBotix)
    - [Maxim DS18: temperature](#DS18)
    - [Maxim DS3231: real time clock](#DS3231)
    - [Measurement Specialties MS5803: pressure and temperature](#MS5803)
    - [Yosemitech: water quality sensors](#Yosemitech)
    - [Zebra-Tech D-Opto: dissolved oxygen](#dOpto)
    - [Processor Metadata Treated as Sensors](#Onboard)
- [Help: Common problems and FAQ's](#help)
    - [Power Draw over Data Lines](#parasites)
- [Notes on Arduino Streams and Software Serial](#SoftwareSerial)
- [Processor/Board Compatibility](#compatibility)
- [Contributing](#contribute)
- [License](#license)
- [Acknowledgments](#acknowledgments)


## <a name="getStarted"></a>Getting Started

Get started by reading this section, collecting the [Physical Dependencies](#pdeps), and installing the [Library Dependencies](#ldeps).  Then try out one of our sketches in the [Examples](https://github.com/EnviroDIY/ModularSensors/tree/master/examples) folder.

To use a sensor and variable in your sketch, you must separately include xxx.h for each sensor you intend to use.  While this may force you to write many more include statements, it decreases the library RAM usage on your Arduino board.  Regardless of how many sensors you intend to use, however, you must install all of the [dependent libraries](#ldeps) on your _computer_ for the Arduino software, PlatformIO or any other Integrated Development Environment (IDE) software to be able to compile the library.

Each sensor is implemented as a subclass of the "Sensor" class contained in "SensorBase.h".  Each variable is separately implemented as a subclass of the "Variable" class contained in "VariableBase.h".  The variables are tied to the sensor using an "[Observer](https://en.wikipedia.org/wiki/Observer_pattern)" software pattern.

The "VariableArray" class contained in "VariableArray.h" defines the logic for iterating through many variable objects.  The VariableArray class takes advantage of various time stamps within the Sensor class to optimize the timing of communcations with many sensors.

The "Logger" class defines functions for sleeping the processor and writing to an SD card.  The logger subclasses add functionality for communicating with the internet and sending data to particular data receivers.



### <a name="whatHo"></a>What are sensors and variables?

Within this library, a sensor, a variable, and a logger mean very specific things:

**Sensor** - A sensor is some sort of device that is capable of taking one or more measurements using some sort of method.  Most often we can think of these as probes or other instruments that can give back information about the world around them.  Sensors can usually be given power or have that power cut.  They may be awoken or activated and then returned to a sleeping/low power use state.  The may be able to be asked to begin a single reading.  They _**must**_ be capable of returning the value of their readings to a logger of some type.

**Variable** - A variable is a result value taken by a sensor _or_ calculated from the results of one or more sensors.  It is characterized by a name (what it is a measurement of), a unit of measurement, and a resolution.  The [names](http://vocabulary.odm2.org/variablename/) and [units](http://vocabulary.odm2.org/units/) of measurements for all variables come from the controlled vocabularies developed for the ODM2 data system.  (http://vocabulary.odm2.org/)  The resolution is determined by the method used to take the measurement by the sensor.  A variable may also be assigned a universally unique identifier (UUID) and a unique variable code.  Many sensors are capable of measuring multiple variables at a single time.  For example, a Decagon CTD-10 is a _sensor_.  It is able to measure 3 _variables_: specific conductance, temperature, and water depth.  The variable named "specificConductance" has _units_ of microsiemens per centimeter (µS/cm) and a _resolution_ of 1 µS/cm.  Each measured variable is explicitly tied to the "parent" sensor that "notifies" the variable when a new value has been measured.  Each calculated variable has a parent function returning a float which is the value for that variable.

**Logger** - A logger is a device that can control all functions of the sensors that are attached to it and save the values of all variables measured by those sensors to an attached SD card.  In this library, all loggers are Arduino-style small processor circuit boards.

**Modem** - A modem is a device that can be controlled by a logger to send out data directly to the world wide web.



### <a name="pdeps"></a>Physical Dependencies

This library is designed for wireless, solar-powered environmental data logging applications, that is, to log data from many physical sensors and to put the processor and all peripherals to sleep to conserver power between readings.  The most banal functions of the library require only an AVR or SAMD processor, but making real use of this library requires:

- A sufficiently powerful AVR or SAMD processor mounted on some sort of circuit board.  (See [Processor/Board Compatibility](#compatibility) for more details on specific processors and boards that are supported.)
    - For all AVR processors, you must also have a [Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) high precision I2C real-time clock with the SQE/INT pin connected to a pin on your processor which supports either external or pin-change interrupts.
    - For SAMD boards, this library makes use of their on-board (though less accurate) real-time clock.
- A SD card reader attached to the processor via SPI.
- Environmental sensors
- A battery to power the system
- A solar charging circuit
- A modem-type unit to communicate remote data (Optional for logging data, but required for sending data directly to the internet.  See [Modem and Internet Functions](#Modem) for supported models.)
- Protected water-proof enclosures and mountings for all of the above
- An OTG cable to connect serial output from the board to a cell phone (Optional, but very helpful for debugging.)

### <a name="ldeps"></a>Library Dependencies

In order to support multiple functions and sensors, there are quite a lot of sub-libraries that this library is dependent on.  _Even if you do not use the modules, you must have all of the dependencies installed for the library itself to properly compile._  Please check the [library.json](https://github.com/EnviroDIY/ModularSensors/blob/master/library.json) file for more details on the versions required of each library.  If you are using [PlatformIO](https://platformio.org), you can list "EnviroDIY_ModularSensors" in the ```lib_deps``` section of your platformio.ini file and all of these libraries will be installed automatically.  If using the "standard" Arduino IDE, you must install each of these libraries individually, or in a bundle from the [EnviroDIY Libraries](https://github.com/EnviroDIY/Libraries) meta-repository.

- [EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt) - Administrates and handles pin change interrupts, allowing the logger to sleep and save battery.  This also controls the interrupts for the versions of SoftwareSerial and SDI-12 linked below that have been stripped of interrupt control.  Because we use this library, _you must always add the line ```#include <EnableInterrupt.h>``` to the top of your sketch._
- AVR sleep library - This is for low power sleeping for AVR processors. (This library is built in to the Arduino and PlatformIO IDEs.)
- [EnviroDIY DS-3231](https://github.com/EnviroDIY/Sodaq_DS3231) - For real time clock control
- [RTCZero library](https://github.com/arduino-libraries/RTCZero) - This real time clock control and low power sleeping on SAMD processors. (This library may be built in to the Arduino IDE.)  NOTE:  If using an AVR board, you must explicitly _ignore_ this library when compiling with PlatformIO or you will have compiler errors.
- [SdFat library](https://github.com/greiman/SdFat) - This enables communication with the SD card.
- [EnviroDIY version of the TinyGSM library](https://github.com/EnviroDIY/TinyGSM) - This provides internet (TCP/IP) connectivity.
- [Adafruit ADS1X15 library](https://github.com/soligen2010/Adafruit_ADS1X15/) - For high-resolution analog to digital conversion.  Note that this is soligen2010's fork of the original Adafruit library; it corrects many problems in the Adafruit library such as a bug which gives the same output on all four inputs regardless of their values.  Do _NOT_ use the original Adafruit version!
- [EnviroDIY Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12/tree/ExtInts) - For control of SDI-12 based sensors.  This modified version is needed so there are no pin change interrupt conflicts with the SoftwareSerial library or the software pin change interrupt library used to wake the processor.
- [SensorModbusMaster](https://github.com/EnviroDIY/SensorModbusMaster) - for easy communication with Modbus devices.
- [OneWire](https://github.com/PaulStoffregen/OneWire) - This enables communication with Maxim/Dallas OneWire devices.
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) - for communication with the DS18 line of Maxim/Dallas OneWire temperature probes.
- [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor) - a dependency of several other Adafruit libraries
- [Adafruit AM2315 library](https://github.com/adafruit/Adafruit_AM2315) - for the AOSong AM2315 temperature and humidity sensor.
- [Adafruit DHT library](https://github.com/adafruit/DHT-sensor-library) - for other AOSong temperature and humidity sensors.
- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library) - for the Bosch BME280 environmental sensor.
- [Adafruit MPL115A2 library](https://github.com/adafruit/Adafruit_MPL115A2) - for the Freescale Semiconductor MPL115A2 barometer.
- [YosemitechModbus](https://github.com/EnviroDIY/YosemitechModbus) - for all Yosemitech modbus environmental sensors.
- [Northern Widget MS5803 Library](https://github.com/NorthernWidget/MS5803) - for the TE Connectivity MEAS MS5803 pressure sensor
- [EnviroDIY KellerModbus Library](https://github.com/EnviroDIY/KellerModbus) - for all Keller modbus pressure and water level sensors.

## <a name="Basic"></a>Basic Sensor and Variable Functions

### Functions Available for Each Sensor
Generally useful functions:
- **Constructor** - Each sensor has a unique constructor, the exact format of which is dependent on the individual sensor.
- **getSensorName()** - This gets the name of the sensor and returns it as a string.
- **getSensorLocation()** - This returns the Arduino pin sending and receiving data or other sensor installation information as a string.  This is the location where the sensor is connected to the data logger, NOT the position of the sensor in the environment.  Generally this value is set in the constructor for the sensor.
- **setNumberMeasurementsToAverage(int nReadings)** - Sets the number of readings for the sensor to take.  This value can also be set by the constructor.  NOTE:  This will beome the number of readings actually taken by a sensor prior to data averaging.  Any "bad" (-9999) values returned by the sensor will not be included in the final averaging.  This the actual number of "good" values that are averaged may be less than what is set by setNumberMeasurementsToAverage or in the sensor constructor.
- **getNumberMeasurementsToAverage()** - Returns an unsigned 8-bit integer with the number of readings the sensor will be taking before averaging and giving a final result.
- **getStatus()** - This returns the 8-bit code for the current status of the sensor:
    - Bit 0 - 0=Not powered, 1=Powered
    - Bit 1 - 0=Has NOT been set up, 1=Has been setup
    - Bit 2 - 0=Is NOT warmed up, 1=Is warmed up
    - Bit 3 - 0=Not awake/actively measuring, 1=Is awake/actively measuring
    - Bit 4 - 0=Readings not stable, 1=Readings should be stable
    - Bit 5 - 0=Measurement requested, 1=No measurements have been requested
    - Bit 6 - 1=Waiting for measurement completion (IFF bit 3 and 4 are set!), 1=Measurement complete (IFF bit 3 and 4 are set!),
    - Bit 7 - 0=No known errors, 1=Some sort of error has occurred
- **updateStatusBits()** - This function checks the current status.
- **setup()** - This "sets up" the sensor - setting up serial ports, etc required for the given sensor.  This must always be called for each sensor within the "setup" loop of your Arduino program _before_ calling the corresponding variable setup.
- **update()** - This updates the sensor values and returns true when finished.  For digital sensors with a single information return, this only needs to be called once for each sensor, even if there are multiple variable subclasses for the sensor.  In general, the update function powers the sensor, wakes/activates it, tells it to start measurements and get values as many times as requested, averages all the values, notifies the attached variables that new values are available, and then puts the sensor back to sleep if it had been asleep at the start of the update.

These functions are also available for each sensor, but should be used with caution.  These functions do not have any built in waits or checks to ensure they are executed in the correct order.  _**For most purposes, you should use the update function!**_  All of these functions are intended to maximize efficiency when using many variables from many sensors together in a [VariableArray](#Grouped).
- **powerUp()** - This sends power to the sensor.  No return.
- **powerDown()** - This cuts the sensor power.  No return.
- **wake()** - This wakes up/activates the sensor up so it is in the state it needs to be to begin taking measurements.  Returns true if successful.  You must remember to first powerUp() the sensor if necessary and allow enough time between the powerUp() and the wake() for the sensor to be ready to respond or call the waitForWarmUp() function.
- **sleep()** - This puts the sensor to sleep/de-activates it, but does NOT power it down.  Returns true if successful.
- **startSingleMeasurement()** - This tells the sensor to start a single measurement.  Returns true if successful.  Before calling this function, ensure that the sensor has power, has been activated, and you have waited long enough for the readings to be stable.  (Use the waitForStability() function as necessary.)
- **addSingleMeasurementResult()** - This gets the results from a single measurement that has already been started.  Returns true if successful.  This function is the heart of what every sensor object does.  When creating a new sensor object type, this function and the constructor are the only functions that must be uniquely implemented.
- **clearValues()** - Clears the result values array for a sensor
- **verifyAndAddMeasurementResult()** - This verifies that a measurement is OK (ie, not -9999) before adding it to the sensor's result array.  This is used to prevent real values from being averaged with -9999 returns.
- **averageMeasurements()** - Averages the values in the sensor's result array.
- **notifyVariables()** - Notifies attached variables of new values.  It is the sensor's job to notify variables!
- **registerVariable()** - The compliment to a variable's "attachSensor(int varNum, Sensor \*parentSense)" function.  These functions tie the variable and sensor together.  This is generally called by the variable, not by the sensor.
- **checkPowerOn()** - Returns true if a sensors assigned power pin is currently "HIGH."
- **isWarmedUp()** - Checks whether or not enough time has passed between the sensor receiving power and being ready to respond to logger commands.
- **waitForWarmUp()** - Delays until time is passed for sensor warm-up.
- **isStable()** - Checks whether or not enough time has passed between the sensor being awoken/activated and being ready to output stable values.
- **waitForStability()** - Delays until time is passed for sensor stability.
- **isMeasurementComplete()** - Checks whether or not enough time has passed between when the sensor was asked to take a single measurement and when that measurement should be complete.
- **waitForMeasurementCompletion()** - Delays until time is passed for measurement completion.

### Functions for Each Variable
- **Constructor** - There are two forms of the constructor, one for measured variables (ie, ones whose values come directly from a senor) and another for calculated variables (ie, ones whose values are calculated from other vales).
    - For _measured_ variables, the base variable constructor should not be used, but instead the constructor for the specific type of variable tied to a sensor should be used.  (That is, use the constructor for the MaxBotixSonar_Range variable sub-object, not the raw variable constructor.)  Each sensor-measured variable constructor requires a pointer to its parent sensor as the first argument of the constructor.  There are also two optional string entries, for a universally unique identifier (UUID or GUID) and a custom variable code.  _The UUID must always be listed first!_  In cases where you would like a custom variable code, but do not have a UUID, you **must** enter '""' as your UUID.
    - For _calculated_ variables, you use the base variable object constructor.  The constructor requires a function which returns a float as its first argument, followed by Strings from the variable's name and unit.  Both of these strings should be values from the [ODM2 controlled vocabularies](http://vocabulary.odm2.org/).  Next an integer variable resolution is required.  Then two Strings for the variable UUID and variable code.  _All arguments are required in the calculated variable constructor!_
- **getVarName()** - This returns the variable's name, using http://vocabulary.odm2.org/variablename/, as a String.
- **getVarUnit()** - This returns the variable's unit, using http://vocabulary.odm2.org/units/, as a String.
- **getVarCode()** - This returns a String with a customized code for the variable, if one is given, and a default if not
- **getVarUUID()** - This returns the universally unique identifier of a variables, if one is assigned, as a String
- **setup()** - This "sets up" the variable - attaching it to its parent sensor.  This must always be called for each sensor within the "setup" loop of your Arduino program _after_ calling the sensor setup.
- **getValue(bool updateValue)** - This returns the current value of the variable as a float.  By default, it does not ask the parent sensor for a new value, but simply returns the last value a parent sensor notified it of, no matter the age of the value.  If you would like to ask the sensor to measure a new value and for that new value to be returned, set the boolean flag as true.  Calculated variables will never ask any sensors for updates.
- **getValueString(bool updateValue)** - This is identical to getValue, except that it returns a string with the proper precision available from the sensor.
- **attachSensor(int varNum, Sensor \*parentSense)** - The compliment to a sensor's registerVariable() function.  This attaches a variable object to the sensor that is giving the value to the variable.  The variable is generally responsible for calling this function!  This should never be called for a calculated variable.
- **onSensorUpdate()** - This is the variable's response to the sensor's notifyVariables() function.  It accepts the new value from the sensor.  This is generally called by the sensor.  This should never be called for a calculated variable.
- - **getParentSensorName()** - This is a helper - it returns the name of the parent sensor, if applicable
- - **getParentSensorLocation()** - This is a helper - it returns the "location" of the parent sensor, if applicable

### <a name="individuals"></a>Examples Using Individual Sensor and Variable Functions
To access and get values from a sensor, you must create an instance of the sensor class you are interested in using its constructor.  Each variable has different parameters that you must specify; these are described below within the section for each sensor.  You must then create a new instance for each _variable_, and reference a pointer to the parent sensor in the constructor.  Many variables can (and should) call the same parent sensor.  The variables are specific to the individual sensor because each sensor collects data and returns data in a unique way.  The constructors are all best called outside of the "setup()" or "loop()" functions.  The setup functions are then called (sensor, then variables) in the main "setup()" function and the update() and getValues() are called in the loop().  A very simple program to get data from a Decagon CTD might be something like:

```cpp
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t measurementsToAverage = 10;  // The number of readings to average
const int SDI12Data = 7;  // The pin the CTD is attached to
const int SDI12Power = 22;  // The sensor power pin (use -1 if not applicable)
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, measurementsToAverage);
DecagonCTD_Cond cond(&ctd);  // The ampersand (&) *must* be included
DecagonCTD_Temp temp(&ctd);
DecagonCTD_Depth depth(&ctd);

setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);

    // Set up the sensor first and then the variables
    ctd.setup();
    cond.setup();
    temp.setup();
    depth.setup();
}

loop()
{
    ctd.update();
    Serial.print("Current conductivity: ");
    Serial.println(cond.getValue());
    Serial.print("Current temperature: ");
    Serial.println(temp.getValue());
    Serial.print("Current water depth: ");
    Serial.println(depth.getValue());
    delay(15000);
}
```

The "[single_sensor](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/single_sensor)" example in the examples folder shows the same functionality for a MaxBotix Ultrasonic Range Finder.  This example also includes using a calculated variable to output a water depth calculated from the sonar range.

## <a name="Grouped"></a>Grouped Sensor Functions
Having a unified set of functions to access many sensors allows us to quickly poll through a list of sensors to get all results quickly.  To this end, "VariableArray.h" adds the class "VariableArray" with functions to use on an array of pointers to variable objects.

### Functions Available for a VariableArray Object:
- **VariableArray(int variableCount, Variable variableList[])** - This is the constructor; it creates the variable array object.  Note that the objects in the variable list must be pointers, not the variable objects themselves.  The pointers may be to calculated or measured variable objects.
- **getVariableCount()** - Simply returns the number of variables.
- **getSensorCount()** - Returns the number of independent sensors.  This will often be different from the number of variables because many sensors can return multiple variables.
- **setupSensors()** - This sets up all of the variables in the array and their respective sensors by running all of their setup() functions.  If a sensor doesn't respond to its setup command, the command is called 5 times in attempt to make a connection.  If all sensors are set up successfully, returns true.
- **sensorsPowerUp()** - This gives power to all sensors, skipping repeated sensors.  No return.
- **sensorsWake()** - This wakes all sensors, skipping repeated sensors.  Returns true.
- **sensorsSleep()** - This puts all sensors to sleep, skipping repeated sensors.  Returns true.
- **sensorsPowerDown()** - This cuts power to all sensors, skipping repeated sensors.  No return.
- **updateAllSensors()** - This updates all sensor values, skipping repeated sensors.  Returns true.  Does NOT return any values.
- **printSensorData(Stream stream)** - This prints current sensor values along with meta-data to a stream (either hardware or software serial).  By default, it will print to the first Serial port.  Note that the input is a pointer to a stream instance so to use a hardware serial instance you must use an ampersand before the serial name (ie, &Serial1).

### <a name="ArrayExamples"></a>VariableArray Examples:

To use the VariableArray module, you must first create the array of pointers.  This should be done outside of the setup() or loop() functions.  Remember that for measured variables you must first create a new sensor instance and then one or more new variable instances for that sensor (depending on how many values it can return.)  The sensor functions for sensors within a variable array take advantage of all of the timestamps and status bits within the sensor object to minimize the amount of time that all sensors are powered and the processor is awake.  That is, the first sensor to be warmed up will be set up or activated first; the first sensor to stabilize will be asked for values first.  All calculations for any calculated variables happen after all the sensor updating has finished.  The order of the variables within the array should not matter, though for code readability, I strongly suggest putting all the variables attached to a single sensor next to each other in the array.

The asterisk must be put in front of the variable name to indicate that it is a pointer to your variable object.  With many variables, it is easier to create the object and the pointer to it all at once in the variable array.  This can be done using the "new" keyword like so:

```cpp
// Create new variable objects in an array named "variableList" using the "new" keyword
// UUID's and customVarCodes are optional
Variable \*variableList[] = {
    new Sensor1_Variable1(&parentSensor1, "UUID", "customVarCode1"),
    new Sensor1_Variable2(&parentSensor1, "UUID", "customVarCode2"),
    ...
    new SensorX_VariableX(&parentSensorx, "UUID", "customVarCode4")
};
// Count the number of variables in the array (you can count them manually, too)
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray myVarArray(variableCount, variableList);
```

You can also create and name variable pointer objects outside of the array and then reference those pointers inside of the array like so:

```cpp
// Create measured variable objects and return pointers to them
// NOTE:  We are actually creating subclasses of variables here (tied to particular
// sensors) but returning pointers to the variable superclass as required for
// the variable array.
Variable *var1 = new sensor1_var1(&parentSensor1, "UUID", "customVarCode1");
Variable *var2 = new sensor1_var2(&parentSensor1, "UUID", "customVarCode1");
Variable *var3 = new sensor2_var1(&parentSensor2, "UUID", "customVarCode1");

// Create a calculated measured variable object and return a pointer to it
float calculationFunction(void)
{
    return var1->getValue() + 10;
}
Variable *calcVar4 = new Variable(calculationFunction, "varName",
                                  "varUnit", varResolution,
                                  "UUID", "varCode");

// Continue creating variables..
Variable *varX = new sensorX_varX(&parentSensorX, "UUID", "customVarCodeX");

// Put the already created variable type pointers into an array
Variable \*variableList[] = {
    var1,
    var2,
    var3,
    var4,
    ...
    varX
};
// Count the number of variables in the array (you can count them manually, too)
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray myVarArray(variableCount, variableList);
```

Creating the variable pointers outside of the array is particularly helpful when you want to reference the same variables in multiple arrays or you want to do any post measurement calculations on the variables.

Once you have created the array of pointers, you can initialize the VariableArray module and setup all of the variables and their parent sensors at once in the setup function:

```cpp
// Set up all the sensors AND variables (BOTH are done by this function)
myVars.setupSensors();
```

You can then get values or variable names for all of the sensors within the loop with calls like:

```cpp
// Send power to all of the sensors
myVarArray.sensorsPowerUp();
// Wake up all of the sensors
myVarArray.sensorsWake();
// Update the values from all attached sensors
myVarArray.updateAllSensors();
// Put sensors to sleep
myVarArray.sensorsSleep();
// Cut sensor power
myVarArray.sensorsPowerDown();
// Print the data to the screen
myVarArray.printSensorData();
```

## <a name="Logger"></a>Basic Logger Functions
Our main reason to unify the output from many sensors and variables is to easily log the data to an SD card and to send it to a live streaming data receiver, like the [MonitorMyWatershed/EnviroDIY data portal](http://data.envirodiy.org/).  There are several modules available to use with the sensors to log data and stream data:  LoggerBase, LoggerEnviroDIY, and LoggerModem.  The classes Logger (in LoggerBase) is a parent class for LoggerEnviroDIY (in LoggerEnviroDIY).  Both also contain an internal VariableArray object.  The Logger class has the abilities to communicate with a real time clock, to put the board into deep sleep between readings to conserver power, and to write the data from the sensors to a csv file on a connected SD card.  The LoggerEnviroDIY class can also have a LoggerModem attached, synchronize the time to NIST, and format and send data to the [EnviroDIY data portal](http://data.envirodiy.org/).  The LoggerModem module is essentially a wrapper for [TinyGSM](https://github.com/EnviroDIY/TinyGSM) which adds quick functions for turning modem on and off to save power and to synchronize the real-time clock with the [NIST Internet time service](https://www.nist.gov/pml/time-and-frequency-division/services/internet-time-service-its).

### Functions Available for a Logger Object:

#### Setup and initialization functions:

- **Logger(const char *loggerID, uint16_t loggingIntervalMinutes, int8_t SDCardPin, int8_t mcuWakePin, VariableArray *inputArray)** - The constructor; creates the logger object.  Note that the variableList[], and loggerID are pointers.  The SDCardPin is the pin of the chip select/slave select for the SPI connection to the SD card.
  - NOTE regarding *loggingIntervalMinutes*: For the first 20 minutes that a logger has been powered up for a deployment, the logger will take readings at 2 minute intervals for 10 measurements, to assist with confirming that the deployment is successful. Afterwards, the time between measurements will revert to the number of minutes set with *loggingIntervalMinutes*.
- **setAlertPin(int ledPin)** - Optionally sets a pin to put out an alert that a measurement is being logged.  This is intended to be a pin with a LED on it so you can see the light come on when a measurement is being taken.
- **setTestingModePin(int buttonPin)** - Optionally sets a pin that is attached to a button or some other interrupt source to force the logger to enter sensor testing mode.

#### Timezone functions:

- **setTimeZone(int timeZone)** - Sets the timezone that you wish data to be logged in (in +/- hours from UTC).  _This must always be set!_
- **getTimeZone()** - Returns the set timezone.
- **setTZOffset(int offset)** - This sets the offset between the built-in clock and the timezone the data should be logged in.  If your clock is set in UTC, then the TZOffset should be the same as the TimeZone.  For example, if you would like your clock to be set in UTC but your data should be output in Eastern Standard Time, both setTimeZone and setTZOffset should be called with -5.  On the other hand, if your clock is already set EST, you do not need to call the setTZOffset function (or can call it with 0).
A note about timezones:  It is possible to create multiple logger objects in your code if you want to log different sensors at different intervals, _but every logger object will always have the same timezone and timezone offset_.  If you attempt to call these functions more than once for different loggers, whatever value was called last will apply to every logger.
- **getTZOffset()** - Returns the set timezone offset.

#### Functions to access the clock in proper format and time zone:

- **syncRTClock(uint32_t timestamp)** - This synchronizes the real time clock with the provided timestamp, which should be a unix timestamp _in UTC_.
- **getNowEpoch()** - This gets the current epoch time (unix timestamp - number of seconds since Jan 1, 1970) and corrects it for the specified logger time zone offset.
- **formatDateTime_ISO8601(DateTime dt)** - Formats a DateTime object into an ISO8601 formatted Arduino String.
- **formatDateTime_ISO8601(uint32_t unixTime)** - Formats a unix timestamp into an ISO8601 formatted Arduino String.
- **checkInterval()** - This returns true if the _current_ time is an even interval of the logging interval, otherwise false.  This uses getNowEpoch() to get the curernt time.
- **markTime()** - This sets static variables for the date/time - this is needed so that all data outputs (SD, EnviroDIY, serial printing, etc) print the same time for updating the sensors - even though the routines to update the sensors and to output the data may take several seconds.  It is not currently possible to output the instantaneous time an individual sensor was updated, just a single marked time.  By custom, this should be called before updating the sensors, not after.  If you do not call this function before saving or sending data, there will be no timestamps associated with your data.  This is called for you every time the checkInterval() function is run.
- **checkMarkedInterval()** - This returns true if the _marked_ time is an even interval of the logging interval, otherwise false.  This uses the static time value set by markTime() to get the time.  It does not check the current time.


#### Functions for the processor sleep modes:

- **setupSleep()** - Sets up the processor sleep mode and the interrupts to wake the processor back up.  This should be called in the setup function.
- **systemSleep()** - Puts the system into deep sleep mode.  This should be called at the very end of the loop function.  Please keep in mind that this does NOT call the wake and sleep functions of the sensors themselves; you must call those separately.  (This separation is for timing reasons.)

#### Functions for logging data:

- **setFileName(fileName)** - This sets a specified file name for data to be saved as, if you want to decide on it in advance.  Note that you must include the file extension (ie., '.txt') in the file name.  If you do not call the setFileName function with a specific name, a csv file name will automatically be generated from the logger id and the current date.
- **getFileName()** - This returns the current filename as an Arduino String.
- **createLogFile(String filename, bool writeDefaultHeader = false)** or **createLogFile(bool writeDefaultHeader = false)** - These functions create a file on an SD card and set the created/modified/accessed timestamps in that file.  The filename may either be the one automatically generated by the logger id and the date, the one set by setFileName(String), or can be specified in the function.  If asked to, these functions will also write a header to the file based on the variable information from the variable array.  This can be used to force a logger to create a file with a secondary file name.
- **logToSD(String filename, String rec)** or **logToSD(String rec)** or **logToSD(void)** - These functions create a file on an SD card and set the modified/accessed timestamps in that file.  The filename may either be the one automatically generated by the logger id and the date, the one set by setFileName(String), or can be specified in the function.  If the file does not already exist, the file will be created.  The line to be written to the file can either be specified or will be a comma separated list of the current values of all variables in the variable array.
- **streamFileHeader(Stream \*stream)** - This prints a header for a csv out to an Arduino stream.  The header will be ordered based on the order variables are listed in the VariableArray.
    - If you would prefer to manually work with and change the header, the function **generateFileHeader()** returns an Aruduino String with a comma separated list of headers for the csv.  This header _will_ be a very, very long string.  _If the string is too long, it may not be printed or cause the entire program to crash!_  Only try to work directly with the header string for VariableArray's with a small number of variables.
- **streamSensorDataCSV(Stream \*stream)**  - This returns an Arduino String containing the time and a comma separated list of sensor values.  The data will be ordered based on the order variables are listed in the VariableArray.
    - If you would like the manipulate or add to the csv data, **generateSensorDataCSV()** returns an Arduino String containing the same information as is printed streamSensorDataCSV(Stream \*stream).  Before using this function, take into account the same warnings as given with generateFileHeader(), although this string will be much, much shorter than the string created by generateFileHeader().

#### Functions for sensor testing and communication debugging:

By default, some status and set-up information will be sent to the Serial (for AVR processors) or SerialUSB (for SAMD processors).  To stop all print out or to change the port the print out goes to, open ModSensorDebugger.h and change or remove lines 15-21 that define the STANDARD_SERIAL_OUTPUT.

There is also a built-in function for "testing" sensors within sensor arrays - that is, continuously displaying current sensor values to test that the sensors are working properly:
- **testingMode()** - Enters a "testing" mode for the sensors.  It prints out all of the sensor results for 25 records worth of data with a 5-second delay between readings.  The printouts go to whichever serial port is given in the ```#define STANDARD_SERIAL_OUTPUT``` statement.  Testing mode can be entered directly in a set-up or loop function by calling the ```testingMode()``` function.  If you have set a pin for testing mode via an interrupt with the setTestingModePin(buttonPin) function, you can also enter testing mode any time except when the logger is already taking a data point by creating an interrupt on that pin (ie, by pushing a button).

For more intense _code_ debugging for any individual component of the library (sensor communication, modem communication, variable array functions, etc), open the source file header (\*.h), for that component.  Find the line ```// #define DEBUGGING_SERIAL_OUTPUT xxxxx```, where xxxxx is the name of a serial output (ie, Serial or USBSerial).  Remove the two comment slashes from that line.  Then recompile and upload your code.  This will (sometimes dramatically) increase the number of statements going out to the debugging serial port.  A few sensors also the line ```// #define DEEP_DEBUGGING_SERIAL_OUTPUT xxxxx```, uncommenting of which will send even more information to the defined port.  Note that this type of debugging is intended to help find errors in the code of this library, not problems with the sensors themselves!

####  Convenience functions to do it all:

- **begin()** - Starts all the sensors, the variable array, and the logger.  Must be in the setup function.
- **log()** - Logs data, must be the entire content of the loop function.

### <a name="Modem"></a>Functions for a LoggerModem:

A loggerModem serves two functions:  First, it communicates with the internet via WiFi or cellular service and sends data to remote services.  Second, it acts as a sensor which can return the strength of the WiFi or cellular connection.  A loggerModem object is a combination of a [TinyGsm](https://github.com/EnviroDIY/TinyGSM) (modem instance), a TinyGsmClient, and a ModemOnOff to control modem power.

Before creating a loggerModem instance, _you must define your modem at top of your sketch_, before any include statements.  ie:
- ```#define TINY_GSM_MODEM_SIM900``` - for a SIMCom SIM900, or variant thereof (including older [Sodaq GPRSBees](https://shop.sodaq.com/en/gprsbee.html))
- ```#define TINY_GSM_MODEM_SIM800``` - for a SIMCom SIM800 or variant thereof (including current [Sodaq GPRSBees](https://shop.sodaq.com/en/gprsbee.html))
- ```#define TINY_GSM_MODEM_SIM808``` - for a SIMCom SIM808 (essentially a SIMCom SIM800 with GPS support)
- ```#define TINY_GSM_MODEM_SIM868``` - for a SIMCom SIM868 (another SIM800 variant with GPS support)
- ```#define TINY_GSM_MODEM_UBLOX``` - for most u-blox cellular modems (LEON-G100, LISA-U2xx, SARA-G3xx, SARA-U2xx, TOBY-L2xx, LARA-R2xx, MPCI-L2xx, or a Digi 3G XBee running in bypass mode)
- ```#define TINY_GSM_MODEM_M95``` - for an Quectel M95
- ```#define TINY_GSM_MODEM_BG96``` - for an Quectel BG96
- ```#define TINY_GSM_MODEM_A6``` - for an AI-Thinker A6
- ```#define TINY_GSM_MODEM_A7``` - for an AI-Thinker A7
- ```#define TINY_GSM_MODEM_M590``` - for a Neoway M590
- ```#define TINY_GSM_MODEM_MC60``` - for a Quectel MC60
- ```#define TINY_GSM_MODEM_MC60E``` - for a Quectel MC60E
- ```#define TINY_GSM_MODEM_ESP8266``` - for an ESP8266 using the _default AT command firmware_
- ```#define TINY_GSM_MODEM_XBEE``` - for Digi brand WiFi or Cellular XBee's running in normal (transparent) mode

Then you must create the modem object:

```cpp
// Create the modem object
loggerModem modem;
```

See [TinyGSM's documentation](https://github.com/vshymanskyy/TinyGSM/blob/master/README.md) for a more details about of all of the chip variants and modules that are supported.

After defining your modem, set it up using one of these two commands, depending on whether you are using cellular or WiFi communication:

- **setupModem(Stream modemStream, int vcc33Pin, int modemStatusPin, int modemSleepRqPin, ModemSleepType sleepType, const char \*APN)** - Sets up the internet communcation with a cellular modem.  Note that the modemStream and APN should be pointers.  Use -1 for any pins that are not connected.
- **setupModem(Stream modemStream, int vcc33Pin, int modemStatusPin, int modemSleepRqPin, ModemSleepType sleepType, const char \*ssid, const char \*pwd)** - Sets up the internet communication with a WiFi modem.  Note that the modemStream, ssid, and password should be pointers.  Use -1 for any pins that are not connected.
- The **vcc33Pin** is the pin that controls whether or not the modem itself is powered.  Use -1 if your modem is always receiving power or if you want to control modem power independently.
    - NOTE:  _Many_ modem chips require more power than the 0.5A that most Arduino-style boards can provide!  The power draw is particularly high during network connection and sending.  Some chips require up to 2.5A.  _Know your modem's specs!_  If it requires more power than your board can provide, ensure that the modem has an alternate battery connection or power source!
- The **modemStatusPin** is the pin that indicates whether the modem is turned on and it is clear to send data.  If you use -1, the modem is assumed to always be ready.
- The **modemSleepRqPin** is the _pin_ used to put the modem to sleep or to wake it up.
- The **ModemSleepType** controls _how the modemSleepRqPin is used_ to put the modem to sleep between readings.
    - Use _"modem_sleep_held"_ if the SleepRq pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.
    - Use _"modem_sleep_pulsed"_ if the SleepRq pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.
    - Use _"modem_sleep_reverse"_ if the SleepRq pin is held LOW to keep the modem awake, as with all XBees.
    - Use *"modem_always_on"* if you do not want the library to control the modem power and sleep.
- Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.

Once the modem has been set up, it has all the functions of sensor object.  These additional functions are available:

- **connectInternet()** - Connects to the internet via WiFi or cellular network.  Returns true if connection is successful.
- **openTCP(const char host, uint16_t port)** - Makes a TCP connection to a host URL and port.  (The most common port for public URLs is "80"; if you don't know the port, try this first.)  Returns 1 if successful.
- **openTCP(IPAddress ip, uint16_t port)** - Makes a TCP connection to a host ip address and port.  Returns 1 if successful.
- **closeTCP()** - Breaks the TCP connection.
- **disconnectInternet()** - Disconnects from the network, if applicable.
- **modemPowerUp()** - Turns the modem on.  Returns true if connection is successful.
- **modemPowerDown()** - Turns the modem off and empties the send and receive buffer.  Returns true if connection is successful.
- **getNISTTime()** - Returns the current Unix time stamp (_in UTC_) from NIST via the TIME protocol (rfc868).

As mentioned above, the cellular modems themselves are also sensors with the following variables:

```cpp
// Create the RSSI and signal strength variable objects for the modem and return
// variable-type pointers to them
Variable *modemRSSI = Modem_RSSI(&modem, "UUID", "customVarCode");  // Received Signal Strength Indication, in dB
Variable *modemSinalPct = Modem_SignalPercent(&modem, "UUID", "customVarCode");  // "Percent" signal strength
```

The modem does not behave quite the same as all the other sensors do, though.  Setup must be done with the ```setupModem(...)``` function; the normal ```setup()``` function does not do anything.  The ```powerUp()``` and ```powerDown()``` functions also do not work, the modem will only go on with the ```modemPowerUp()``` function and off with the ```modemPowerDown()``` function.

Special note for Sodaq GPRSBee modems: To start the modem you will need to power the logger board off, connect the battery to the logger board, and finally attach the modem to the logger board.  Then you may power the board and run your sketch. We have found that attaching a GPRSBee modem to power in a different sequence results in the modem reporting zero signal strength.


### <a name="DIYlogger"></a>Additional Functions Available for a LoggerEnviroDIY Object:
These functions attach a modem and set up the required registration token and sampling feature UUID for the EnviroDIY web streaming data loader API.  **All three** functions must be called before calling any of the other EnviroDIYLogger functions.  You *must* also add the correct variable UUID's to the constructors for each variable you are using.  All of the UUID and token values can be obtained after registering at http://data.envirodiy.org/.

- **attachModem(loggerModem &modem)** - Attaches a loggerModem to the logger, which the logger then can use to send data to the internet.  See [Modem and Internet Functions](#Modem) for more information on how the modem must be set up before it is attached to the logger.  You must include an ampersand to tie in the already created modem!  If you do not attach a modem, you cannot send data to The EnviroDIY data portal!
- **setToken(const char \*registrationToken)** - Sets the registration token to access the EnviroDIY streaming data loader API.  Note that the input is a pointer to the registrationToken.
- **setSamplingFeatureUUID(const char \*samplingFeatureUUID)** - Sets the universally unique identifier (UUID or GUID) of the sampling feature.  Note that the input is a pointer to the samplingFeatureUUID.

Within the loop, these functions can then format and send out data:

- **streamSensorDataJSON(Stream \*stream)** - Prints a properly formatted EnviroDIY datqa portal JSON string to the selected Arduino stream.
    - If you would like the manipulate or add to the csv data, **generateSensorDataJSON()** returns an Arduino String containing the same information as is printed by streamSensorDataJSON(Stream \*stream).  Keep in mind, the JSON is likely to be a very long string and _may crash your program_.  Only use generateSensorDataJSON() when working with a small variable array
- **postDataEnviroDIY(String enviroDIYjson = "");** - Creates proper HTTP headers and sends the entered JSON to the EnviroDIY data portal.  Depends on the having a modem attached.  Returns an HTML response code.  If you do not enter a JSON, the streamSensorDataJSON(Stream \*stream) function will be used internally to generate one.  Please keep in mind when attempting to enter a String JSON that very long strings _may crash your program_.

### <a name="LoggerExamples"></a>Logger Examples:

To set up logging, you must first include the appropriate logging module and create a new logger instance.  This must happen outside of the setup and loop functions:

```cpp
// Import Logger Module
#include <LoggerBase.h>
// Create a new logger instance
Logger logger;
```

--OR--

```cpp
// Import Logger Module
#include <LoggerEnviroDIY.h>
// Create a new logger instance
LoggerEnviroDIY EnviroDIYLogger(loggerID, loggingIntervalMinutes, SDCardPin, mcuWakePin, inputArray);
```

_Within the setup function_, you must then set the logger time zones and attach informational pins.  For the EnviroDIY logger, you must also set up the communication.:

```cpp
// Set the time zone and offset from the RTC
logger.setTimeZone(timeZone);
logger.setTZOffset(offset);
// OPTIONAL - specify a pin to give an alert when a measurement is taken
// This should generally be a pin with an LED
setAlertPin(int ledPin);
// OPTIONAL - specify a pin for entering testing mode
setTestingModePin(int buttonPin);
// Begin the logger;
logger.begin();
```

--OR--

```cpp
// Set the time zone and offset from the RTC
EnviroDIYLogger.setTimeZone(timeZone);
EnviroDIYLogger.setTZOffset(offset);
// OPTIONAL - specify a pin to give an alert when a measurement is taken
// This should generally be a pin with an LED
setAlertPin(ledPin);
// OPTIONAL - specify a pin for entering testing mode
setTestingModePin(int buttonPin);
// Set up the communication with EnviroDIY
EnviroDIYLogger.setToken(registrationToken);
EnviroDIYLogger.setSamplingFeature(samplingFeature);
EnviroDIYLogger.setUUIDs(UUIDs[]);

// Set up the logger modem
modem.setupModem(modemStream, vcc33Pin, modemStatusPin, modemSleepRqPin, sleepType, APN);

// Attach the modem to the logger
EnviroDIYLogger.attachModem(modem);

// Begin the logger;
EnviroDIYLogger.begin();
```

_Within the main loop function_, all logging and sending of data can be done using a single program line.  Because the built-in log functions already handle sleeping and waking the board processor, **there cannot be nothing else within the loop function.**  This is a wonderful helper if you are setting up your logger to do only the "normal" things.  Please keep in mind, _this is not the only option_.

```cpp
void loop()
{
    logger.log();
}
```

--OR--

```cpp
void loop()
{
    EnviroDIYLogger.log();
}
```
If you would like to do other things within the loop function, you should access the component logging functions individually instead of using the short-cut functions.  In this case, here are some guidelines for writing a loop function:

- If you want to log on an even interval, use ```if (checkInterval())``` or ```if (checkMarkedInterval())``` to verify that the current or marked time is an even interval of the logging interval..
- Call the ```markTime()``` function before printing/sending/saving any data that you want associate with a timestamp.
- Power up all of your sensors with ```sensorsPowerUp()```.
- Wake up all your sensors with ```sensorsWake()```.
- Update all the sensors in your VariableArray together with ```updateAllSensors()```.
- Immediately after running ```updateAllSensors()```, put sensors to sleep to save power with ```sensorsSleep()```.
- Power down all of your sensors with ```sensorsPowerDown()```.
- After updating the sensors, then call any functions you want to send/print/save data.
- Finish by putting the logger back to sleep, if desired, with ```systemSleep()```.

The [double_logger example program](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/double_logger) demonstrates using a custom loop function in order to log two different groups of sensors at different logging intervals.  The [data_saving example program](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/data_saving) shows using a custom loop in order to save cellular data by saving data from many variables on the SD card, but only sending a portion of the data to the EnviroDIY data portal.


## Available sensors

There are a number of sensors supported by this library.  Depending on the sensor, it may communicate with the Arduino board using as a serial peripheral interface (SPI), inter-integrated circuit (I2C, also called "Wire," "Two Wire", or "TWI"), or some type of universal synchronous/asynchronous receiver/transmitter (UART/USART, or simply "serial") protocol.  (USART or serial includes transistor-transistor logic (TTL), RS232 (adapter needed), and RS485 (adapter needed) communication).  See the section on [Processor Compatibility](#compatibility) for more specific notes on which pins are available for each type of communication on the various supported processors.

Essentially all of the sensors can have their power supplies turned off between readings, but not all boards are able to switch output power on and off.  When the sensor constructor asks for the Arduino pin controlling power on/off, use -1 for any board which is not capable of switching the output power on and off.

Please, please, when setting up multiple sensors on a logger, be smart about it.  Don't try to connect too many sensors all at once or you're likely to exceed your logger's power regulator or come across strange interferences between them.  _**TEST YOUR LOGGER WITH ALL SENSORS ATTACHED BEFORE DEPLOYING IT TO THE FIELD!**_  Don't even think about skipping the in-lab testing!  Theoretically every single sensor possible could be attached to the same processor, but the reality is that boards have finite numbers of pins, solar panels can only create so much charge, and not all sensors like each other very much.
_____

### <a name="AM2315"></a>[AOSong AM2315](www.aosong.com/asp_bin/Products/en/AM2315.pdf) Encased I2C Temperature/Humidity Sensor

The AOSong AM2315 and [CM2311](http://www.aosong.com/en/products/details.asp?id=193) communicate with the board via I2C.  Because this sensor can have only one I2C address (0xB8), it is only possible to connect one of these sensors to your system.  This sensor should be attached to a 3.3-5.5V power source and the power supply to the sensor can be stopped between measurements.

The only input needed for the sensor constructor is the Arduino pin controlling power on/off and optionally the number of readings to average:

```cpp
#include <AOSongAM2315.h>
// Create and return the AM2315 sensor object
AOSongAM2315 am2315(I2CPower, measurementsToAverage);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the temperature and humidity variable objects for the AM2315 and return variable-type pointers to them
Variable *am2315Humid = new AOSongAM2315_Humidity(&am2315, "UUID", "customVarCode");  // Percent relative humidity
//  Resolution is 0.1 % RH (16 bit)
//  Accuracy is ± 2 % RH
//  Range is 0 - 100 % RH
Variable *am2315Temp= new AOSongAM2315_Temp(&am2315, "UUID", "customVarCode");  // Temperature in °C
// Resolution is 0.1°C (16 bit)
// Accuracy is ±0.1°C
// Range is -40°C to +125°C
```
_____

### <a name="DHT"></a>[AOSong DHT](http://www.aosong.com/en/products/index.asp) Digital-Output Relative Humidity & Temperature Sensor

This module will work with an AOSong [DHT11/CHT11](http://www.aosong.com/en/products/details.asp?id=109), DHT21/AM2301, and [DHT22/AM2302/CM2302](http://www.aosong.com/en/products/details.asp?id=117).  These sensors use a single-bus single wire digital signaling protocol.  They can be connected to any digital pin.  Please keep in mind that, per manufacturer instructions, these sensors should not be polled more frequently than once every 2 seconds.  These sensors should be attached to a 3.3-6V power source and the power supply to the sensor can be stopped between measurements.  The communication with these sensors is slow and _interrupts are turned off during communication_.  (See the Adafruit DHT library's DHT.cpp for details.)  Keep this in mind if using this sensor in combination with a rain gauge or other interrupt-driven sensor.  Also note that the temperature sensor built into the AOSong DHT is a Maxim DS18 sensor.

The Arduino pin controlling power on/off, the Arduino pin receiving data, and the sensor type are required for the sensor constructor.  The number of readings to average is optional:

```cpp
#include <AOSongDHT.h>/
// Create and return the DHT sensor object
AOSongDHT dht(DHTPower, DHTPin, dhtType, measurementsToAverage);
```

The three available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the temperature, humidity, and heat index variable objects for the DHT and return variable-type pointers to them
Variable *dhtHumid = new AOSongDHT_Humidity(&dht, "UUID", "customVarCode");  // Percent relative humidity
//  Resolution is 0.1 % RH for DHT22 and 1 % RH for DHT11
//  Accuracy is ± 2 % RH for DHT22 and ± 5 % RH for DHT11
//  Range is 0 to 100 % RH
Variable *dhtTemp = new AOSongDHT_Temp(&dht, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1°C
//  Accuracy is ±0.5°C for DHT22 and ± ±2°C for DHT11
//  Range is -40°C to +80°C
Variable *dhtHI = new AOSongDHT_HI(&dht, "UUID", "customVarCode");  // Calculated Heat Index
//  Resolution is 0.1°C
//  Accuracy is ±0.5°C for DHT22 and ± ±2°C for DHT11
//  Range is -40°C to +80°C
```
_____

### <a name="SQ212"></a>[Apogee SQ-212 Quantum Light Sensor](https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/) Photosynthetically Active Radiation (PAR)
This library will work with the Apogee SQ-212 and SQ-212 analog quantum light sensors, and could be readily adapted to work with similar sensors (e.g. SQ-215 or SQ225) with by simply changing the calibration factors.  These sensors send out a simple analog signal.  To convert that to a high resolution digital signal, the sensor must be attached to a TI ADS1115 ADD converter.  The TI ADS1115 ADD communicates with the main processor via I2C.  In the majority of break-out boards, and on the Mayfly, the I2C address of the ADS1x15 is set as 0x48 by tying the address pin to ground.  Up to four of these ADD's be used by changing the address value by changing the connection of the address pin on the ADS1x15.  The ADS1x15 requires an input voltage of 2.0-5.5V, but this library assumes the ADS is powered with 3.3V.  The PAR sensors should be attached to a 5-24V power source and the power supply to the sensor can be stopped between measurements.

The Arduino pin controlling power on/off and the analog data pin _on the TI ADS1115_ are required for the sensor constructor.  If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the third argument.

```cpp
#include <ApogeeSQ212.h>
// Create and return the Apogee SQ-212 sensor object
ApogeeSQ212 SQ212(SQ212Power, SQ212Data, ADS1x15_i2cAddress, measurementsToAverage);
```

The one available variable is:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the PAR variable object for the Apogee SQ-212 and return a variable-type pointer to it
Variable *sq212PAR = new ApogeeSQ212_PAR(&SQ212, "UUID", "customVarCode");  // Photosynthetically Active Radiation (PAR), in units of μmol m-2 s-1, or microeinsteins per square meter per second
//  Resolution is 0.04 µmol m-2 s-1 (16 bit ADC)
//  Accuracy is ± 0.5%
//  Range is 0 to 2500 µmol m-2 s-1
```
_____

### <a name="BME280"></a>[Bosch BME280](https://www.bosch-sensortec.com/bst/products/all_products/bme280) Integrated Environmental Sensor

Although this sensor has the option of either I2C or SPI communication, this library only supports I2C.  The default I2C address varies by manufacturer and is either 0x77 or 0x76. The Adafruit and Sparkfun defaults are both 0x77 and Seeed/Grove default is 0x76, though all can be changed by physical modification of the sensor, if necessary (by cutting the board connection for the manufacturer default and soldering the optional address jumpers).  To connect two of these sensors to your system, you must ensure they are soldered so as to have different I2C addresses.  No more than two can be attached (unless you use a multiplexer).  This module is likely to also work with the [Bosch BMP280 Barometric Pressure Sensor](https://www.bosch-sensortec.com/bst/products/all_products/bmp280), though it has not been tested on it.  These sensors should be attached to a 1.7-3.6V power source and the power supply to the sensor can be stopped between measurements.

The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex is optional as is the number of readings to average:

```cpp
#include <BoschBME280.h>
// Create and return the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, i2cAddressHex, measurementsToAverage);
```

The four available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the four variable objects for the BME280 and return variable-type pointers to them
Variable *bme280Temp = new BoschBME280_Temp(&bme280, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.01°C
//  Accuracy is ±0.5°C
//  Range is -40°C to +85°C
Variable *bme280Humid = new BoschBME280_Humidity(&bme280, "UUID", "customVarCode");  // Percent relative humidity
//  Resolution is 0.008 % RH (16 bit)
//  Accuracy is ± 3 % RH
//  Range is 0-100 % RH
Variable *bme280Press = new BoschBME280_Pressure(&bme280, "UUID", "customVarCode");  // Barometric pressure in pascals
//  Resolution is 0.18Pa
//  Absolute Accuracy is ±1hPa
//  Relative Accuracy is ±0.12hPa
//  Range is 300 to 1100 hPa
Variable *bme280Alt = new BoschBME280_Altitude(&bme280, "UUID", "customVarCode");  // Altitude in meters, calculated from barometric pressure
//  Resolution is 1m
//  Accuracy depends on geographic location
```
_____

### <a name="OBS3"></a>[Campbell Scientific OBS-3+](https://www.campbellsci.com/obs-3plus)

The version of the OBS-3+ that this library supports sends out a simple analog signal between 0 and 2.5V.  (The 5V and 4-20mA versions are _not_ supported by this library.)  To convert the analog signal to a high resolution digital signal, the sensor must be attached to a [TI ADS1115](http://www.ti.com/product/ADS1115) 16-bit ADD converter.  (This module is built into the EnviroDIY Mayfly.)  The TI ADS1115 ADD communicates with the main processor via I2C.  In the majority of break-out boards, and on the Mayfly, the I2C address of the ADS1x15 is set as 0x48 by tying the address pin to ground.  Up to four of these ADD's be used by changing the address value by changing the connection of the address pin on the ADS1x15.  The ADS1x15 requires an input voltage of 2.0-5.5V, but this library assumes the ADS is powered with 3.3V.  The OBS-3 itself requires a 5-15V power supply, which can be turned off between measurements.  (It will actually run on power as low as 3.3V.)  The power supply is connected to the red wire, low range output comes from the blue wire, high range output comes from the white wire, and the black, green, and silver/unshielded wires should all be connected to ground.

The Arduino pin controlling power on/off, analog data pin _on the TI ADS1115_, and calibration values _in Volts_ for Ax^2 + Bx + C are required for the sensor constructor.  A custom variable code can be entered as a second argument in the variable constructors, and it is very strongly recommended that you use this otherwise it will be very difficult to determine which return is high and which is low range on the sensor.  If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the third argument.  Do NOT forget that if you want to give a number of measurements to average, that comes _after_ the i2c address in the constructor!

Note that to access both the high and low range returns, two instances must be created, one at the low range return pin and one at the high pin.

The main constructor for the sensor object is (called once each for high and low range):

```cpp
#include <CampbellOBS3.h>
// Create and return the low-range sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_x2_coeff_A, OBSLow_x1_coeff_B, OBSLow_x0_coeff_C, ADS1x15_i2cAddress, measurementsToAverage);
// Create the high-range sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_x2_coeff_A, OBSHigh_x1_coeff_B, OBSHigh_x0_coeff_C, ADS1x15_i2cAddress, measurementsToAverage);
```

The single available variable is (called once each for high and low range):

```cpp
// Create and return the low-range variable object and return a variable-type pointer to it
Variable *obs3lowTurbid = new CampbellOBS3_Turbidity(&osb3low, "UUID", "customLowVarCode");  // Low Range Turbidity in NTU
// Create the high-range variable object and return a variable-type pointer to it
Variable *obs3highTurbid = new CampbellOBS3_Turbidity(&osb3high, "UUID", "customHighVarCode");  // High Range Turbidity in NTU
//  Ranges: (depends on sediment size, particle shape, and reflectivity)
//      Turbidity (low/high): 250/1000 NTU; 500/2000 NTU; 1000/4000 NTU
//      Mud: 5000 to 10,000 mg L–1
//      Sand: 50,000 to 100,000 mg L–1
//  Accuracy: (whichever is larger)
//      Turbidity: 2% of reading or 0.5 NTU
//      Mud: 2% of reading or 1 mg L–1
//      Sand: 4% of reading or 10 mg L–1
// Resolution:
//    16-bit ADC
//        Turbidity: 0.004/0.01 NTU; 0.008/0.03 NTU; 0.01/0.06 NTU
//    12-bit ADC
//        Turbidity: 0.06/0.2 NTU; 0.1/0.5 NTU; 0.2/1.0 NTU
```
_____

### <a name="5TM"></a>[Meter Environmental ECH2O 5TM](https://www.metergroup.com/environment/products/ech2o-5tm-soil-moisture/) Soil Moisture and Temperature Sensor

The 5TM soil moisture sensor communicates with the board using the [SDI-12 protocol](http://www.sdi-12.org/) (and the [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12)).  It requires a 3.5-12V power supply, which can be turned off between measurements.  While contrary to the manual, they will run with power as low as 3.3V.  On the 5TM with a stereo cable, the power is connected to the tip, data to the ring, and ground to the sleeve.  On the bare-wire version, the power is connected to the _white_ cable, data to _red_, and ground to the unshielded cable.  Meter Environmental was formerly known as Decagon Devices and sold this sensor as the 5TM.

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.  Optionally, you can include a number of distinct readings to average.  The data pin must be a pin that supports pin-change interrupts.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.

Keep in mind that SDI12 is a slow communication protocol (only 1200 baud) and _ALL interrupts are turned off during communication_.  This means that if you have any interrupt driven sensors (like a tipping bucket) attached with an SDI12 sensor, no interrupts (or tips) will be registered during SDI12 communication.

The main constructor for the sensor object is:

```cpp
#include <Decagon5TM.h>
// Create and return the Decagon 5TM sensor object
Decagon5TM fivetm(TMSDI12address, SDI12Power, SDI12Data, measurementsToAverage);
```

The three available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the Ea, Temperature, and Volumetric Water Content variables for the 5TM  and return variable-type pointers to them
Variable *fivetmEA = new Decagon5TM_Ea(&fivetm, "UUID", "customVarCode");  // Ea/Matric Potential Variable in farads per meter
Variable *fivetmTemp = new Decagon5TM_VWC(&fivetm, "UUID", "customVarCode");  // Volumetric water content as percent, calculated from Ea via TOPP equation
//  Resolution is 0.0008 m3/m3 (0.08% VWC) from 0 – 50% VW
//  Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typical
//  Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
//  Range is 0 – 1 m3/m3 (0 – 100% VWC)
Variable *fivetmVWC = new Decagon5TM_Temp(&fivetm, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1°C
//  Accuracy is ± 1°C
//  Range is - 40°C to + 50°C
```
_____

### <a name="CTD"></a>[Meter Environmental Hydros 21](https://www.metergroup.com/environment/products/hydros-21-water-level-monitoring/) 3-in-1 Water Level Sensor (Electrical Conductivity, Temperature, and Depth Sensor)

The Meter Environmental Hydros 21 has the same type of connections and communication protocol as the [Meter Environmental ECH2O 5TM](#5TM).  Meter Environmental was formerly known as Decagon Devices and sold this sensor as the CTD-5 or  CTD-10.

The main constructor for the sensor object is:

```cpp
#include <DecagonCTD.h>
// Create and return the Decagon CTD sensor object
DecagonCTD ctd(CTDSDI12address, SDI12Power, SDI12Data, measurementsToAverage);
```

The three available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the Conductivity, Temperature, and Water Depth variables for the CTD and return variable-type pointers to them
Variable *ctdCond = new DecagonCTD_Cond(&ctd, "UUID", "customVarCode");  // Conductivity in µS/cm
//  Resolution is 0.001 mS/cm = 1 µS/cm
//  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
//  Range is 0 – 120 mS/cm (bulk)
Variable *ctdTemp = new DecagonCTD_Temp(&ctd, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1°C
//  Accuracy is ±1°C
//  Range is -11°C to +49°C
Variable *ctdDepth = new DecagonCTD_Depth(&ctd, "UUID", "customVarCode");  // Water depth in mm
//  Resolution is 2 mm
//  Accuracy is ±0.05% of full scale
//  Range is 0 to 5 m or 0 to 10 m, depending on model
```
_____

### <a name="ES2"></a>[Decagon Devices ES-2](http://www.decagon.com/en/hydrology/water-level-temperature-electrical-conductivity/es-2-electrical-conductivity-temperature/) Electrical Conductivity Sensor

_NOTE:  Decagon Devices has become Meter Environmental and no longer sells this sensor._  If you still have one of these sensors, it follows the same connection and communication protocol as the [Meter Environmental ECH2O 5TM](#5TM).

The main constructor for the sensor object is:

```cpp
#include <DecagonES2.h>
// Create and return the Decagon ES2 sensor object
DecagonES2 es2(ES2SDI12address, SDI12Power, SDI12Data, measurementsToAverage);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the Conductivity and Water Depth variables for the ES2 and return variable-type pointers to them
Variable *es2Cond = new DecagonES2_Cond(&es2, "UUID", "customVarCode");  // Conductivity in µS/cm
//  Resolution is 0.001 mS/cm = 1 µS/cm
//  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
//  Range is 0 – 120 mS/cm (bulk)
Variable *es2Temp = new DecagonES2_Temp(&es2, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1°C
//  Accuracy is ±1°C
//  Range is -40°C to +50°C
```
_____

### <a name="ExtVolt"></a>External Voltage, via [TI ADS1115](http://www.ti.com/product/ADS1115)
The TI ADS1115 ADD is a high resolution ADS that communicates with the board via I2C.  In the majority of break-out boards, and on the Mayfly, the I2C address of the ADS1x15 is set as 0x48 by tying the address pin to ground.  Up to four of these ADD's be used by changing the address value by changing the connection of the address pin on the ADS1x15.  The ADS1x15 requires an input voltage of 2.0-5.5V, but this library assumes the ADS is powered with 3.3V.  For measuring raw external voltages, this library also allows you to give a gain factor/multiplier to account for use of a voltage divider, such as the [Seeed Grove Voltage Divider](http://wiki.seeedstudio.com/Grove-Voltage_Divider/).

The Arduino pin controlling power on/off and the analog data pin _on the TI ADS1115_ are required for the sensor constructor.  If using a voltage divider to increase the measurable voltage range, enter the gain multiplier as the third argument.  If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the fourth argument.  The number of measurements to average, if more than one is desired, goes as the fifth argument.

```cpp
#include <ExternalVoltage.h>
// Create and return the voltage sensor
ExternalVoltage extvolt(VoltPower, VoltData, VoltGain, ADS1x15_i2cAddress, measurementsToAverage);
```

The one available variable is:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the voltage variable and return a variable-type pointer to it
Variable *extVoltage = new ExternalVoltage_Volt(&extvolt, "UUID", "customVarCode");  // raw voltage in volts
// Range:
//   without voltage divider:  0 - 3.6V
//   1/gain = 3x: 0.3 ~ 12.9v
//   1/gain = 10x: 1 ~ 43v
// Accuracy is < ± 1%
// Resolution: 16-bit ADC:
//   without voltage divider:  0.05mV
//   1/gain = 3x: 0.2mV
//   1/gain = 10x: 0.65 mV
```
_____

### <a name="MaxBotix"></a>[MaxBotix MaxSonar](http://www.maxbotix.com/Ultrasonic_Sensors/High_Accuracy_Sensors.htm) - HRXL MaxSonar WR or WRS Series with TTL Outputs

The IP67 rated HRXL-MaxSonar-WR ultrasonic rangefinders offer 1mm resolution, 2.7-5.5VDC operation, a narrow beam pattern, high power output, noise rejection, automatic calibration, and temperature compensation.  Depending on the precise model, the range finders have ranges between 300 and 9999mm and read rates of 6-7.5Hz.  This library supports TTL or RS323 sensor output, though an RS232-to-TTL adapter is needed for the RS232 models.  Analog and pulse-width outputs are not supported.  The MaxBotix sensors require a 2.7V-5.5V power supply to pin 6 on the sensor (which can be turned off between measurements) and the level of the TTL returned by the MaxSonar will match the power level it is supplied with.   The digital TTL or RS232 output is sent out on pin 5 on the sensor.  Pin 7 of the MaxSonar must be connected to power ground and pin 4 can optionally be used to trigger the MaxSonar.

If you are using the [MaxBotix HR-MaxTemp](https://www.maxbotix.com/Ultrasonic_Sensors/MB7955.htm) MB7955 temperature compensator on your MaxBotix (which greatly improves data quality), the white wire from the MaxTemp should be attached to pin 1 (square) on the MaxSonar.  The red wire should be attached to Pin 6 (V+) and shield wires from the MaxTemp should both be attached to Pin 7 (GND) (Caution, the images from MaxBotix are misleading!).  The MaxTemp communicates directly with the MaxSonar and there is no need to make any changes on the Aruduino itself for the MaxTemp.

The MaxBotix sensor have two different modes: free-ranging and triggered.  Unless the trigger pin is externally held low, the sensor will continuously take readings at a rate of 6Hz or greater and immediate report each result over the digital output pin.  (That is, it will be in free-ranging mode.)  When continuously powered and operating in free-range mode, the data output is automatically filtered to help improve accuracy.  If you are turning the power to the sensor off between readings, there is no advantage to using the free-ranging because many readings must be taken before the filter becomes effective.  In this case, you may save some power by setting up a trigger pin and manually trigger individual readings.

The Arduino pin controlling power on/off, a stream instance for received data (ie, ```Serial```), and the Arduino pin controlling the trigger are required for the sensor constructor.  (Use -1 for the trigger pin if you do not have it connected.)  Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.

This library supports using multiple MaxBotix sensors on the same logger, with a few caveats:  
 - Any sensor operating in free-ranging mode (powered at the same time as any other sensors with the trigger pins unconnected) must have a dedicated stream instance/serial port.
 - To have two sensors operating in free-ranging mode, they must each have a dedicated stream instance/serial port *AND* you must specify a unique _negative_ pin number for the trigger pin.  Giving a negative pin number ensures that the Arduino will not attempt to trigger trigger individual readings but will still be able to tell the sensors apart.  (Software-wise, simply specifying the different streams is not enough!)  Keep in mind that two or more free ranging sensors must be spaced far enough apart in the field to prevent interference between the sonar beams.
 - Two or more sensors may send data to the same stream instance/serial port if both sensors are being triggered and each is triggered by a different trigger pin.
 - "Daisy chaining" sensors so the pulse-width output of one sensor acts as the trigger for a second sensor is not supported.

The main constructor for the sensor object is:  (The trigger pin and number of readings to average are optional.)

```cpp
#include <MaxBotixSonar.h>
// Create and return the Maxbotix sonar sensor object;
MaxBotixSonar sonar(sonarStream, SonarPower, SonarTrigger, measurementsToAverage);
```

The single available variable is:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the sonar range variable object and return a variable-type pointer to it
Variable *hrxlRange = new MaxBotixSonar_Range(&sonar, "UUID", "customVarCode");  // Ultrasonic range in mm
//  Resolution is 1mm
//  Accuracy is ± 1%
//  Range is 300mm 5000mm or 500mm to 9999mm, depending on  model
```

In addition to the constructors for the sensor and variable, you must remember to "begin" your stream instance within the main setup function.  The baud rate must be set to 9600 for all MaxBotix sensors.

```cpp
sonarStream.begin(9600);
```
_____

### <a name="DS18"></a>[Maxim DS18 Temperature Probes](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18S20.html)

The Maxim temperature probes communicate using the OneWire library, which can be used on any digital pin on any of the supported boards.  The same module should work with a [DS18B20](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html), [DS18S20](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18S20.html), [DS1822](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1822.html), [MAX31820](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/MAX31820.html), and the no-longer-sold [DS1820](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1820.html) sensor.  These sensors can be attached to a 3.0-5.5V power source or they can take "parasitic power" from the data line.  When using the more typical setup with power, ground, and data lines, a 4.7k resistor must be attached as a pull-up between the data and power lines.  The one-wire communication protocol is slow and _interrupts are turned off during communication_.  Keep this in mind if using this sensor in combination with a rain gauge or other interrupt-driven sensor.

The OneWire hex address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.  The hex address is an array of 8 hex values, for example:  {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0}.  To get the address of your sensor, plug a single sensor into your device and run the [oneWireSearch](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/oneWireSearch/oneWireSearch.ino) example or the [Single](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.pde) example provided within the Dallas Temperature library.  The sensor address is programmed at the factory and cannot be changed.

The main constructor for the sensor object is:

```cpp
#include <MaximDS18.h>
// Create and return a Maxim DS18 sensor object - address known
MaximDS18 ds18(OneWireAddress, powerPin, dataPin, measurementsToAverage);
```

_If and only if you have exactly one sensor attached on your OneWire pin or bus_, you can use this constructor to save yourself the trouble of finding the address:

```cpp
#include <MaximDS18.h>
// Create and return a Maxim DS18 sensor object - address NOT known
MaximDS18 ds18(powerPin, dataPin, measurementsToAverage);
```

The single available variable is:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the temperature variable object for the DS18 and return a variable-type pointer to it
MaximDS18_Temp(&ds18, "UUID", "customVarCode");  // Temperature in °C
// Resolution is between 0.0625°C (12 bit) and 0.5°C (9-bit)
// Accuracy is ±0.5°C from -10°C to +85°C for DS18S20 and DS18B20, ±2°C for DS1822 and MAX31820
// Range is -55°C to +125°C (-67°F to +257°F)
```
_____

### <a name="DS3231"></a>[Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) Real Time Clock

The I2C [Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) real time clock (RTC) is absolutely required for time-keeping on all AVR boards.  This library also makes use of it for its on-board temperature sensor.  The DS3231 requires a 3.3V power supply.

The only argument for the constructor is the number of readings to average, as the RTC requires constant power and is connected via I2C:

```cpp
#include <MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(measurementsToAverage);
```

The only available variables is:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the temperature variable object for the DS3231 and return a variable-type pointer to it
MaximDS3231_Temp(&ds3231, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.25°C
//  Accuracy is ±3°C
//  Range is 0°C to +70°C
```
_____

### <a name="MS5803"></a>[Measurement Specialties MS5803](http://www.te.com/usa-en/product-CAT-BLPS0013.html) Digital Pressure Sensor

These sensors come in several different pressure ranges.  The maximum measurable pressure is assumed to be 14bar (the most common model), but this can be changed in the constructor.  Although this sensor has the option of either I2C or SPI communication, this library only supports I2C.  _The I2C sensor address is assumed to be 0x76_, though it can be changed to 0x77 in the constructor if necessary. The Sparkfun default is 0x76 (14 BA currently available). Northern Widget supplies MS5803 sensors with 2 BA maximum pressure and their default is usually 0x77, but varies by model as outlined in their library (github.com/NorthernWidget/TP-Downhole_Library). The sensor address is determined by how the sensor is soldered onto its breakout board.  To connect two of these sensors to your system, you must ensure they are soldered so as to have different I2C addresses.  No more than two can be attached.  These sensors should be attached to a 1.7-3.6V power source and the power supply to the sensor can be stopped between measurements.  NOTE:  These I2C addresses are the same as those available for the Bosch BME280 Barometric Pressure Sensor!  If you are also using one of those sensors, make sure that the address for that sensor does not conflict with the address of this sensor.  

The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex and maximum pressure are optional as is the number of readings to average:

```cpp
#include <MeaSpecMS5803.h>
// Create and return the MeaSpec MS5803 sensor object
MeaSpecMS5803 ms5803(I2CPower, i2cAddressHex, maxPressure, measurementsToAverage);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the pressure and temperature variable objects for the MS5803 and return variable-type pointers to them
Variable *ms5803Press = new MeaSpecMS5803_Pressure(&ms5803, "UUID", "customVarCode");  // pressure in millibar
// For Pressure (sensor designed for water pressure):
//   Resolution is 1 / 0.6 / 0.4 / 0.3 / 0.2 mbar (where 1 mbar = 100 pascals)
//      at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
//   Accuracy 0 to +40°C is ±20mbar
//   Accuracy -40°C to +85°C is ±40mbar
//   Range is 0 to 14 bar
//   Long term stability is -20 mbar/yr
Variable *ms5803Temp = new MeaSpecMS5803_Temp(&ms5803, "UUID", "customVarCode");  // temperature in °C
// For Temperature:
//   Resolution is <0.01°C
//   Accuracy is ±0.8°C
//   Range is -40°C to +85°C
```
_____

### <a name="MPL115A2"></a>[Freescale Semiconductor MPL115A2](https://www.nxp.com/docs/en/data-sheet/MPL115A2.pdf) Miniature I2C Digital Barometer

The MPL115A2 communicates with the board via I2C.  Because this sensor can have only one I2C address (0x60), it is only possible to connect one of these sensors to your system.  This sensor should be attached to a 2.375-5.5V power source and the power supply to the sensor can be stopped between measurements.

The only input needed for the sensor constructor is the Arduino pin controlling power on/off and optionally the number of readings to average:

```cpp
#include <FreescaleMPL115A2.h>
// Create and return the Freescale MPL115A2 sensor object
MPL115A2 mpl115a2(I2CPower, measurementsToAverage);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the pressure and temperature variable objects for the MPL115A2 and return variable-type pointers to them
Variable *mpl115a2Press = new MPL115A2_Pressure(&mpl115a2, "UUID", "customVarCode");  // Baraometric pressure in kPa
//  Resolution is 0.15 kPa
//  Accuracy is ±1 kPa
//  Range is 50 to 115 kPa
Variable *mpl115a2Temp = new MPL115A2_Temp(&mpl115a2, "UUID", "customVarCode");  // Temperature in °C
```
_____

### <a name="ExtTips"></a>[External I2C Rain Tipping Bucket Counter](https://github.com/EnviroDIY/TippingBucketRainCounter)
This module is for use with a simple external I2C tipping bucket counter.  This is *NOT* for direct counting of tips using an interrupt on the main processor.  The construction and programming of the tipping bucket counter is documented in the GitHub link above.  It is assumed that the processor of the tip counter takes care of its own power management.

All constructor arguments are optional, but the first argument is for the I2C address of the tip counter (if not 0x08) and the second is for the depth of rain (in mm) per tip event (if not 0.2mm).  Most metric tipping buckets are calibrated to have 1 tip per 0.2mm of rain.  Most English tipping buckets are calibrated to have 1 tip per 0.01" of rain, which is 0.254mm.  Note that you cannot input a number of measurements to average because averaging does not make sense with this kind of counted variable.

```cpp
#include <RainCounterI2C.h>
// Create and return the rain counter sensor object
RainCounterI2C tip(RainCounterI2CAddress, depthPerTipEvent);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the tips and rain depth variable objects for the rain counter and return variable-type pointers to them
Variable *rainTips = new RainCounterI2C_Tips(&tip, "UUID", "customVarCode");  // raw count of tips
Variable *rainDepth = new RainCounterI2C_Depth(&tip, "UUID", "customVarCode");  // rain depth in mm
// Range, accuracy, and resolution depend on the actual tipping bucket module
```
_____

### <a name="keller"></a>[Keller Submersible Water Level Transmitters](http://www.te.com/usa-en/product-CAT-BLPS0013.html) Pressure Sensor

Many Keller pressure and water level sensors can communicate via Modbus RTU over RS485. The functions below should work with any Keller Series 30, Class 5, Group 20 sensor (such as the Keller Acculevel) that are Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week). Note that these have only been tested with the Acculevel. More documentation for our implementation of the Keller Modbus communication Protocol commands and responses, along with information about the various variables, can be found in the [EnviroDIY KellerModbus library](https://github.com/EnviroDIY/KellerModbus). Sensors ship with default Slave addresses set to 0x01, but these can be reset. These Keller sensors expect an input voltage of 9-28 VDC, so they also require a voltage booster and an RS485 to TTL Serial converter with logic level shifting from the higher output voltage to the 3.3V or 5V of the Arduino data logging board.

Digital communication with Keller sensors configured for SDI12 communication protocols are not supported by these functions.

The sensor constructor requires as input: modbus address, the pin controlling sensor power, and a stream instance for data (ie, ```Serial```).  The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.  (Use -1 for the enable pin if your adapter does not have one and you want to average more than one reading.) Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.  In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to communicate with these sensors, because it isn't stable enough.  AltSoftSerial and HardwareSerial work fine.

```cpp
#include <KellerAcculevel.h>
// Create and return the Keller AccuLevel sensor object
KellerAcculevel acculevel(acculevelModbusAddress, modbusSerial, modbusPower, max485EnablePin, acculevelNumberReadings);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the pressure, temperature, and water height variable objects for the AccuLevel and return variable-type pointers to them
Variable *acculevelPress = new KellerAcculevel_Pressure(&acculevel, "UUID", "customVarCode");  // vented & barometric pressure corrected water pressure in millibar
Variable *acculevelTemp = new KellerAcculevel_Temp(&acculevel, "UUID", "customVarCode");  // water temperature in °C
Variable *acculevelHeight = new KellerAcculevel_Height(&acculevel, "UUID", "customVarCode");  // water height above the sensor in meters

```
_____

### <a name="Yosemitech"></a>[Yosemitech Brand Environmental Sensors ](http://www.yosemitech.com/en/)

This library currently supports the following Yosemitech sensors:

- [Y502-A or Y504-A Optical Dissolved Oxygen Sensors](http://www.yosemitech.com/en/product-10.html)
- [Y510-B Optical Turbidity Sensor](http://www.yosemitech.com/en/product-17.html)
- [Y511-A Optical Turbidity Sensor with Wiper](http://www.yosemitech.com/en/product-16.html)
- [Y514-A Chlorophyll Sensor with Wiper](http://www.yosemitech.com/en/product-14.html)
- [Y520-A 4-Electrode Conductivity Sensor](http://www.yosemitech.com/en/product-18.html)
- Y532-A Digital pH Sensor
- Y533 ORP Sensor
- [Y550-B UV254/COD Sensor with Wiper](http://www.yosemitech.com/en/product-21.html)
- [Y4000 Multiparameter Sonde](http://www.yosemitech.com/en/product-20.html)

All of these sensors require a 5-12V power supply and the power supply can be stopped between measurements.  (_Note that any user settings (such as brushing frequency) will be lost if the sensor loses power._)  They communicate via [Modbus RTU](https://en.wikipedia.org/wiki/Modbus) over [RS-485](https://en.wikipedia.org/wiki/RS-485).  To interface with them, you will need an RS485-to-TTL adapter.  The white wire of the Yosemitech sensor will connect to the "B" pin of the adapter and the green wire will connect to "A".  The red wire from the sensor should connect to the 5-12V power supply and the black to ground.  The Vcc pin on the adapter should be connected to another power supply (voltage depends on the specific adapter) and the ground to the same ground.  The red wire from the sensor _does not_ connect to the Vcc of the adapter.  The R/RO/RXD pin from the adapter connects to the TXD on the Arduino board and the D/DI/TXD pin from the adapter connects to the RXD.  If applicable, tie the RE and DE (receive/data enable) pins together and connect them to another pin on your board.  While this library supports an external enable pin, we have had very bad luck with most of them.  Adapters with automatic direction control tend to use very slightly more power, but have more stable communication.  There are a number of RS485-to-TTL adapters available.  When shopping for one, be mindful of the logic level of the TTL output by the adapter.  The MAX485, one of the most popular adapters, has a 5V logic level in the TTL signal.  This will _fry_ any board like the Mayfly that uses 3.3V logic.  You would need a voltage shifter in between the Mayfly and the MAX485 to make it work.

The sensor modbus address, the pin controlling sensor power, a stream instance for data (ie, ```Serial```), the Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter, and the number of readings to average are required for the sensor constructor.  (Use -1 for the enable pin if your adapter does not have one.)  For all of these sensors except pH, Yosemitech strongly recommends averaging 10 readings for each measurement.  Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.  In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to communicate with these sensors, because it isn't stable enough.  AltSoftSerial and HardwareSerial work fine.

By default, this library cuts power to the sensors between readings, causing them to lose track of their brushing interval.  The library manually activates the brushes as part of the "wake" command.  There are currently no other ways to set the brushing interval in this library.

The various sensor and variable constructors are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Dissolved Oxygen Sensor
#include <YosemitechY504.h>  // Use this for both the Y502-A and Y504-A
// Create and return the Y504 DO sensor object
YosemitechY504 y504(y504modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the dissolved oxygen percent, dissolved oxygen concentration, and temperature variable objects for the Y504 and return variable-type pointers to them
Variable *y504DOpct = new YosemitechY504_DOpct(&y504, "UUID", "customVarCode");  // DO percent saturation
//  Resolution is 0.00000005 %
//  Accuracy is ± 1 %
//  Range is 0-200% Saturation
Variable *y504DOmgL = new YosemitechY504_DOmgL(&y504, "UUID", "customVarCode");  // DO concentration in mg/L, calculated from percent saturation
//  Resolution is 0.000000005 mg/L
//  Accuracy is 1%
//  Range is 0-20mg/L or 0-200% Air Saturation
Variable *y504Temp = new YosemitechY504_Temp(&y504, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// Turbidity Sensor without wiper
#include <YosemitechY510.h>  // Use this for the Y510-B
// Create and return the Y510-B turbidity sensor object
YosemitechY510 y510(y510modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the turbidity and temperature variable objects for the Y510 and return variable-type pointers to them
Variable *y510Turb = new YosemitechY510_Turbidity(&y510, "UUID", "customVarCode");  // Turbidity in NTU
//  Resolution is 0.0000002 NTU
//  Accuracy is ± 5 % or 0.3 NTU
//  Range is 0.1 to 1000 NTU
Variable *y510Temp = new YosemitechY510_Temp(&y510, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// Turbidity Sensor with wiper
#include <YosemitechY511.h>  // Use this for the Y511-A
// Create and return the Y511-A turbidity sensor object
YosemitechY511 y511(y511modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the turbidity and temperature variable objects for the Y511 and return variable-type pointers to them
Variable *y511Turb = new YosemitechY511_Turbidity(&y511, "UUID", "customVarCode");  // Turbidity in NTU
//  Resolution is 0.0000002 NTU
//  Accuracy is ± 5 % or 0.3 NTU
//  Range is 0.1 to 1000 NTU
Variable *y511Temp = new YosemitechY511_Temp(&y511, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// Chlorophyll Sensor
#include <YosemitechY514.h>
// Create and return the Y514 chlorophyll sensor object
YosemitechY514 y514(y514modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the chlorophyll concentration and temperature variable objects for the Y514 and return variable-type pointers to them
Variable *y514Chloro = new YosemitechY514_Chlorophyll(&y514, "UUID", "customVarCode");  // Chlorophyll concentration in µg/L
//  Resolution is 0.1 µg/L / 0.1 RFU
//  Accuracy is ± 1 %
//  Range is 0 to 400 µg/L or 0 to 100 RFU
Variable *y514Temp = new YosemitechY514_Temp(&y514, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// Conductivity Sensor
#include <YosemitechY520.h>
// Create and return the Y520 conductivity sensor object
YosemitechY520 y520(y520modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the specific conductance and temperature variable objects for the Y520 and return variable-type pointers to them
Variable *y520Cond = new YosemitechY520_Cond(&y520, "UUID", "customVarCode");  // Conductivity in µS/cm
//  Resolution is 0.1 µS/cm
//  Accuracy is ± 1 % Full Scale
//  Range is 1 µS/cm to 200 mS/cm
Variable *y520Temp = new YosemitechY520_Temp(&y520, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// pH Sensor
#include <YosemitechY532.h>
// Create and return the Y532 pH sensor object
YosemitechY532 y532(y532modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the pH, voltage, and temperature variable objects for the Y532 and return variable-type pointers to them
Variable *y532pH = new YosemitechY532_pH(&y532, "UUID", "customVarCode");  // pH
//  Resolution is 0.01 pH units
//  Accuracy is ± 0.1 pH units
//  Range is 2 to 12 pH units
Variable *y532Volt = new YosemitechY532_Voltage(&y532, "UUID", "customVarCode");  // Electrode electrical potential in mV
// Resolution is 1 mV
// Accuracy is ± 20 mV
// Range is -999 ~ 999 mV
Variable *y532Temp = new YosemitechY532_Temp(&y532, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// ORP Sensor
#include <YosemitechY533.h>
// Create and return the Y533 ORP sensor object
YosemitechY533 y533(y533modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the pH, voltage, and temperature variable objects for the Y533 and return variable-type pointers to them
Variable *y533pH = new YosemitechY533_pH(&y533, "UUID", "customVarCode");  // pH
//  Resolution is 0.01 pH units
//  Accuracy is ± 0.1 pH units
//  Range is 2 to 12 pH units
Variable *y533Volt = new YosemitechY533_Voltage(&y533, "UUID", "customVarCode");  // Electrode electrical potential in mV
// Resolution is 1 mV
// Accuracy is ± 20 mV
// Range is -999 ~ 999 mV
Variable *y533Temp = new YosemitechY533_Temp(&y533, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
```

```cpp
// COD/UV254 Sensor
#include <YosemitechY550.h>
// Create and return the Y5550 COD/UV254 sensor object
YosemitechY550 y550(y550modbusAddress, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create the COD, turbidity, and temperature variable objects for the Y550 and return variable-type pointers to them
Variable *y550COD = new YosemitechY550_COD(&y550, "UUID", "customVarCode");  // COD in mg/L equiv. KHP
//  Resolution is 0.01 mg/L COD
//  Accuracy is ??
//  Range is 0.75 to 370 mg/L COD (equiv. KHP) 0.2 - 150 mg/L TOC (equiv. KHP)
Variable *y550Turb = new YosemitechY550_Turbidity(&y550, "UUID", "customVarCode");  // Turbidity in NTU
//  Resolution is 0.0000002 NTU
//  Accuracy is ± 5 % or 0.3 NTU
//  Range is 0.1 to 1000 NTU
Variable *y550Temp = new YosemitechY550_Temp(&y550, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.00000001 °C
//  Accuracy is ± 0.2°C
//  Range is 5°C to + 45°C
```

```cpp
// Multiparameter Sonde
#include <YosemitechY4000.h>
// Create and return the Y4000 multiparameter sonde sensor object
YosemitechY4000 y4000(YosemitechY4000, modbusSerial, modbusPower, max485EnablePin, measurementsToAverage);
// Create all of the variable objects for the Y4000 and return variable-type pointers to them
Variable *y4000DOmgl = new YosemitechY4000_DOmgL(&y4000, "UUID", "customVarCode");  // DO concentration in mg/L, calculated from percent saturation
//  Resolution is 0.01 mg/L
//  Accuracy is ± 0.3 mg/L
//  Range is 0-20mg/L or 0-200% Air Saturation
Variable *y4000Turb = new YosemitechY4000_Turbidity(&y4000, "UUID", "customVarCode");  // Turbidity in NTU
//  Resolution is 0.0000002 NTU
//  Accuracy is ± 5 % or 0.3 NTU
//  Range is 0.1 to 1000 NTU
Variable *y4000Cond = new YosemitechY4000_Cond(&y4000, "UUID", "customVarCode");  // Conductivity in µS/cm
//  Resolution is 0.1 µS/cm
//  Accuracy is ± 1 % Full Scale
//  Range is 1 µS/cm to 200 mS/cm
Variable *y4000pH = new YosemitechY4000_pH(&y4000, "UUID", "customVarCode");  // pH
//  Resolution is 0.01 pH units
//  Accuracy is ± 0.1 pH units
//  Range is 2 to 12 pH units
Variable *y4000Temp = new YosemitechY4000_Temp(&y4000, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.1 °C
//  Accuracy is ± 0.2°C
//  Range is 0°C to + 50°C
Variable *y4000ORP = new YosemitechY4000_ORP(&y4000, "UUID", "customVarCode");  // Electrode electrical potential in mV
// Resolution is 1 mV
// Accuracy is ± 20 mV
// Range is -999 ~ 999 mV
Variable *y4000Chloro = new YosemitechY4000_Chlorophyll(&y4000, "UUID", "customVarCode");  // Chlorophyll concentration in µg/L
//  Resolution is 0.1 µg/L / 0.1 RFU
//  Accuracy is ± 1 %
//  Range is 0 to 400 µg/L or 0 to 100 RFU
Variable *y4000BGA = new YosemitechY4000_BGA(&y4000, "UUID", "customVarCode");  // blue-green algae concentration in µg/L
//  Resolution is 0.01 µg/L / 0.01 RFU
//  Accuracy is ±  0.04ug/L PC
//  Range is 0 to 100 µg/L or 0 to 100 RFU
```
_____

### <a name="dOpto"></a>[Zebra-Tech D-Opto](http://www.zebra-tech.co.nz/d-opto-sensor/) Dissolved Oxygen Sensor

The Zebra-Tech D-Opto sensor communicates with the board using the [SDI-12 protocol](http://www.sdi-12.org/) (and the [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12)).  It require an 8-12V power supply, which can be turned off between measurements.  The connection between the logger and the Arduino board is made by way of a white interface module provided by Zebra-Tech. You will need a voltage booster or a separate power supply to give the D-Opto sufficient voltage to run.  We use [Pololu 9V Step-Up Voltage Regulators](https://www.pololu.com/product/2116).

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.  Optionally, you can include a number of distinct readings to average.  The data pin must be a pin that supports pin-change interrupts.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.

Keep in mind that SDI12 is a slow communication protocol (only 1200 baud) and _ALL interrupts are turned off during communication_.  This means that if you have any interrupt driven sensors (like a tipping bucket) attached with an SDI12 sensor, no interrupts (or tips) will be registered during SDI12 communication.

The main constructor for the sensor object is:

```cpp
#include <ZebraTechDOpto.h>
// Create and return the Zebra-Tech D-Optop sensor object
ZebraTechDOpto dopto(*DOptoDI12address, SDI12Power, SDI12Data, measurementsToAverage);
```

The three available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the dissolved oxygen percent, dissolved oxygen concentration, and temperature variable objects for the Y504 and return variable-type pointers to them
Variable *doptoTemp = new ZebraTechDOpto_Temp(&ctd, "UUID", "customVarCode");  // Temperature in °C
//  Resolution is 0.01°C
//  Accuracy is ± 0.1°C
Variable *doptoDOpct = new ZebraTechDOpto_DOpct(&ctd, "UUID", "customVarCode");  // Dissolved oxygen percent saturation
Variable *doptoDOmgl = new ZebraTechDOpto_DOmgL(&ctd, "UUID", "customVarCode");  // Dissolved oxygen concentration in ppm (mg/L)
//  Resolution is 0.01% / 0.001 PPM
//  Accuracy is 1% of reading or 0.02PPM, whichever is greater
```
_____

### <a name="Onboard"></a> Processor On-Board Sensors and MetaData

The processor can return the amount of RAM it has available and, for some boards, the battery voltage (EnviroDIY Mayfly, Sodaq Mbili, Ndogo, Autonomo, and One, Adafruit Feathers).  The version of the board is required as input (ie, for a EnviroDIY Mayfly: "v0.3" or "v0.4" or "v0.5").  Use a blank value (ie, "") for un-versioned boards.

The main constructor for the sensor object is:

```cpp
#include <ProcessorStats.h>
// Create and return the processor "sensor"
ProcessorStats mayfly(MFVersion, measurementsToAverage);
```

The two available variables are:  (UUID and customVarCode are optional; UUID must always be listed first.)

```cpp
// Create the battery voltage and free RAM variable objects for the Y504 and return variable-type pointers to them
Variable *mayflyBatt = new ProcessorStats_Batt(&mayfly, "UUID", "customVarCode");  // Current battery voltage in volts
//  Resolution is 0.005V (10 bit ADC)
//  Range is 0 to 5 V
Variable *mayflyRAM = new ProcessorStats_FreeRam(&mayfly, "UUID", "customVarCode");  // Amount of free RAM in bits
//  Resolution is 1 bit
//  Accuracy is 1 bit
//  Range is 0 to full RAM available on processor
```
_____


## <a name="help"></a>Help: Common problems and FAQ's

### <a name="parasites"></a>Power Draw over Data Lines

When deploying a logger out into the wild and depending on only battery or solar charging, getting the power draw from sensors to be as low as possible is crucial.  This library assumes that the main power/Vcc supply to each sensor can be turned on by setting its powerPin high and off by setting its powerPin low.  For most well-designed sensors, this should stop all power draw from the sensor.  Real sensors, unfortunately, aren't as well designed as one might hope and some sensors (and particularly RS485 adapters) can continue to suck power from by way of high or floating data pins.  For most sensors, this library attempts to set all data pins low when sending the sensors and then logger to sleep.  If you are still seeing "parasitic" power draw, here are some work-arounds you can try:

- For sensors (and adapters) drawing power over a serial line:
    - Write-out your entire loop function.  (Don't just use ```log()```.)
    - Add a ```SerialPortName.begin(BAUD);``` statement to the beginning of your loop, before ```sensorsPowerUp()```.
    - After ```sensorsPowerDown()``` add ```SerialPortName.end(BAUD);```.
    - After "ending" the serial communication, explicitly set your Rx and Tx pins low using ```digitalWrite(#, LOW);```.
- For sensors drawing power over I2C:
    - Many (most?) boards have external pull-up resistors on the hardware I2C/Wire pins which cannot be disconnected from the main power supply.  This means I2C parasitic power draw is best solved via hardware, not software.
    - Use a specially designed I2C isolator
    - Use a generic opto-isolator or other type of isolator on both the SCL and SDA lines
    - In this future, this library _may_ offer the option of using software I2C, which would allow you to use the same technique as is currently usable to stop serial parasitic draw.  Until such an update happens, however, hardware solutions are required.

_____

## <a name="SoftwareSerial"></a>Notes on Arduino Streams and Software Serial

In this library, the Arduino communicates with the computer for debugging, the modem for sending data, and some sensors (like the [MaxBotix MaxSonar](#MaxBotix)) via instances of Arduino TTL "[streams](https://www.arduino.cc/en/Reference/Stream)."  The streams can either be an instance of [serial](https://www.arduino.cc/en/Reference/Serial) (aka hardware serial), [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial), [the EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts), or any other stream type you desire.  The very commonly used build-in version of the software serial library for AVR processors uses interrupts that conflict with several other sub-libraries or this library and _cannot be used_!  I repeat:  **_You cannot use the built-in version of SoftwareSerial!_**  You simply cannot.  It will not work.  Period.  This is not a bug that will be fixed.

For stream communication, **hardware serial** should always be your first choice, if your processor has enough hardware serial ports.  Hardware serial ports are the most stable and have the best performance of any of the other streams.  Hardware serial ports are also the only option if you need to communicate with any device that uses even or odd parity, more than one stop bit, or does not use 8 data bits.  (That is, hardware serial ports are the only way to communicate with a device that doesn't use the 8N1 configuration.)  Again, _always use a hardware serial port for communication if possible!_

If the [proper pins](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) are available, **[AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial)** by Paul Stoffregen is also superior to SoftwareSerial, especially at slow baud rates.  Neither hardware serial nor AltSoftSerial require any modifications.  Because of the limited number of serial ports available on most boards, I suggest giving first priority (i.e. the first (or only) hardware serial port, "Serial") to your debugging stream going to your PC (if you intend to debug), second priority to the stream for the modem, and third priority to any sensors that require a stream for communication.  See the section on [Processor Compatibility](#compatibility) for more specific notes on what serial ports are available on the various supported processors.

Another possible serial port emulator is [NeoSWSerial](https://github.com/SlashDevin/NeoSWSerial).  While not as stable as AltSoftSerial, it supports using any pin with pin change interrupts for communication. To use NeoSWSerial, you must open the NeoSWSerial.h file and find and uncomment the line ```//#define NEOSWSERIAL_EXTERNAL_PCINT``` and then recompile the library.  There are instructions in the NeoSWSerial ReadMe on how to use EnableInterrupt to activate NeoSWSerial.  Note that NeoSWSerial is generally incompatible with the SDI-12 communication library on most 8MHz processors (including the EnviroDIY Mayfly).  The two libraries can be compiled together, but because they are in competition for a timer, they cannot be used together.  The way this (ModularSensors) uses the SDI-12 library resets the timer settings when ending communication, so you may be able to use the two libraries together if the communication is not simultaneous.  Please test your configuration before deploying it!

To use a hardware serial stream, you do not need to include any libraries or write any extra lines.  You can simply write in "Serial#" where ever you need a stream.  If you would like to give your hardware serial port an easy-to-remember alias, you can use code like this:

```cpp
HardwareSerial* streamName = &Serial;
```

To use AltSoftSerial:

```cpp
#include <AltSoftSerial.h>  // include the AltSoftSerial library
AltSoftSerial streamName;  // Create an instance of AltSoftSerial
```

To use the EnviroDIY modified version of SoftwareSerial:

```cpp
#include <SoftwareSerial_ExtInts.h>  // include the SoftwareSerial library
SoftwareSerial_ExtInts streamName(tx_pin, rx_pin);
```

After creating the stream instances, you must always remember to "begin" your stream within the main setup function.

```cpp
streamName.begin(BAUD_RATE);
```

Additionally, for the EnviroDIY modified version of SoftwareSerial, (or NeoSWSerial) you must enable the interrupts in your setup function:

```cpp
//  Allow enableInterrrupt to control the interrupts for software serial
enableInterrupt(rx_pin, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
```

Here are some helpful links for more information about the number of serial ports available on some of the different Arduino-style boards:

- For Arduino brand boards:  [https://www.arduino.cc/en/Reference/Serial](https://www.arduino.cc/en/Reference/Serial)
- For AtSAMD21 boards:  [https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview](https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview)

## <a name="compatibility"></a>Processor/Board Compatibility

#### AtMega1284p ([EnviroDIY Mayfly](https://envirodiy.org/mayfly/), Sodaq Mbili, Mighty 1284)
The Mayfly _is_ the test board for this library.  _Everything_ is designed to work with this processor.

- An external DS3231 or DS3232 RTC is required.
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 13 (MOSI) on a Mayfly/Mbili or pins 6 (MISO), 7 (SCK), and 5 (MOSI) on a Mighty 1284 or other AtMega1284p.  Chip select/slave select is pin 12 on a Mayfly and card detect can be set to pin 18 with solder jumper 10.  CS/SS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- This processor has two built-in hardware serial ports, Serial and Serial1
    - On most boards, Serial is connected to the FDTI chip for USB communication with the computer.  On both the Mayfly and the Mbili Serial1 is wired to the "Bee" sockets for communication with the modem.
- AltSoftSerial can be used on pins 5 (Tx) and 6 (Rx) on the Mayfly or Mbili.  Pin 4 cannot be used while using AltSoftSerial on the Mayfly or Mbili.
    - Unfortunately, the Rx and Tx pins are on different Grove plugs on both the Mayfly and the Mbili making AltSoftSerial somewhat inconvienent to use.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx) on the Mighty 1284 and other 1284p boards.  Pin 12 cannot be used while using AltSoftSerial on the Mighty 1284.
- Any digital pin can be used with SoftwareSerial_ExtInts or SDI-12.
___

#### AtSAMD21 (Arduino Zero, Adafruit Feather M0, Sodaq Autonomo)
Fully supported

- This processor has an internal real time clock (RTC) and does not require a DS3231 to be installed.  The built-in RTC is not as accurate as the DS3231, however, and should be synchronized more frequently to keep the time correct.  The processor clock will also reset if the system battery dies because unlike most external RTC's, there is no coin battery backing up the clock.  At this time, the AtSAMD21 is only supported using the internal clock, but support with a more accurate external RTC is planned.
- This processor has one hardware serial port, USBSerial, which can _only_ be used for USB communication with a computer
- Most variants have 2 other hardware serial ports (Serial on pins 30 (TX) and 31 (RX) and Serial1 on pins 0 (TX) and 1 (RX)) configured by default.
    - On an Arduino Zero Serial goes to the debug port.
    - On a Sodaq Autonomo Serial1 goes to the "Bee" port.
    - On an Adafruit Feather M0 only Serial1 is configured.
- Most variants have one SPI port configured by default (likely pins 22 (MISO), 23 (MOSI), and 24 (SCK)).  Chip select/slave select and card detect pins vary by board.
- Most variants have one I2C (Wire) interface configured by default (likely pins 20 (SDA) and 21 (SCL)).
- There are up to _6_ total "sercom" ports hard which can be configured for either hardware serial, SPI, or I2C (wire) communication on this processor.  See https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview for more instructions on how to configure these ports, if you need them.
- AltSoftSerial is not supported on the AtSAMD21.
- SoftwareSerial_ExtInts is not supported on the AtSAMD21.
- NeoSWSerial is not supported at all on the AtSAMD21.
- Any digital pin can be used with SDI-12.
- Because the USB controller is built into the processor, your USB serial connection will close as soon as the processor goes to sleep.  If you need to debug, I recommend using a serial port monitor like [Tera Term](https://ttssh2.osdn.jp/index.html.en) which will automatically renew its connection with the serial port when it connects and disconnects.  Otherwise, you will have to rely on lights on your alert pin or your modem to verify the processor is waking/sleeping properly.
- There is a completely weird bug that causes the code to crash if using input pin 1 on the TI ADS1115 (used for the Campbell OBS3+, Apogee SQ212, and raw external voltages).  I have no idea at all why this happens.  Pins 0, 2, and 3 all work fine.  Just don't use pin 1.  This only seems to apply to the SAMD21 boards.
___

#### AtMega2560 (Arduino Mega)
Should be fully functional, but untested.

- An external DS3231 or DS3232 RTC is required.
- There is a single SPI port on pins 50 (MISO), 52 (SCK), and 51 (MOSI).  Chip select/slave select is on pin 53.
- There is a single I2C (Wire) interface on pins 20 (SDA) and 21 (SCL).
- This processor has 4 built-in hardware serial ports Serial, which is connected to the FTDI chip for USB communication with the computer, Serial1 on pins 19 (RX) and 18 (TX), Serial2 on pins 17 (RX) and 16 (TX), and Serial3 on pins 15 (RX) and 14 (TX).
- If you still need more serial ports, AltSoftSerial can be used on pins 46 (Tx) and 48 (Rx).  Pins 44 and 45 cannot be used while using AltSoftSerial on the AtMega2560.
- Pins 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), and A15 (69) can be used with SoftwareSerial_ExtInts or SDI-12.
___

#### AtMega644p (Sanguino)
Should be fully functional, but untested.

- An external DS3231 or DS3232 RTC is required.
- This processor has two built-in hardware serial ports, Serial and Serial1.  On most boards, Serial is connected to the FDTI chip for USB communication with the computer.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- There is a single SPI port on pins 6 (MISO), 7 (SCK), and 5 (MOSI).  Chip select/slave select and card detect pins vary by board.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx).  Pin 12 cannot be used while using AltSoftSerial on the AtMega644p.
- Any digital pin can be used with SoftwareSerial_ExtInts or SDI-12.
___

#### AtMega328p (Arduino Uno, Duemilanove, LilyPad, Mini, Seeeduino Stalker, etc)
All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.  You will only be able to use a small number of sensors at one time and may not be able to log data.

- An external DS3231 or DS3232 RTC is required.
- There is a singe SPI ports on pins 12 (MISO), 13 (SCK), and 11 (MOSI).  Chip select/slave select is pin 10 on an Uno.  SS/CS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins A4 (SDA) and A5 (SCL).
- This processor only has a single hardware serial port, Serial, which is connected to the FTDI chip for USB communication with the computer.
- AltSoftSerial can be used on pins 9 (Tx) and 8 (Rx).  Pin 10 cannot be used while using AltSoftSerial on the AtMega328p.
- Any digital pin can be used with SoftwareSerial_ExtInts or SDI-12.
___

#### AtMega32u4 (Arduino Leonardo/Micro, Adafruit Flora/Feather, etc)
All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.  You will only be able to use a small number of sensors at one time and may not be able to log data.

- An external DS3231 or DS3232 RTC is required.
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 16 (MOSI).  Chip select/slave select and card detect pins vary by board.
- There is a single I2C (Wire) interface on pins 2 (SDA) and 3 (SCL).
- This processor has one hardware serial port, Serial, which can _only_ be used for USB communication with a computer
- There is one additional hardware serial port, Serial1, which can communicate with any serial device.
- AltSoftSerial can be used on pins 5 (Tx) and 13 (Rx).
- Only pins 8, 9, 10, 11, 14, 15, and 16 can be used with SoftwareSerial_ExtInts or SDI-12.  (And pins 14, 15, and 16 will be eliminated if you are using any SPI devices (like an SD card).)
- Because the USB controller is built into the processor, your USB serial connection will close as soon as the processor goes to sleep.  If you need to debug, I recommend using a serial port monitor like Tera Term which will automatically renew its connection with the serial port when it connects and disconnects.  Otherwise, you will have to rely on lights on your alert pin or your modem to verify the processor is waking/sleeping properly.
___

#### Unsupported Processors:

- **ESP8266/ESP32** - Supported _only_ as a communications module (modem) with the default AT command firmware, not supported as an independent controller
- **AtSAM3X (Arduino Due)** - Unsupported at this time due to clock and sleep issues.
    - There is one SPI port on pins 74 (MISO), 76 (MOSI), and 75 (SCK).  Pins 4, 10 and pin 52 can be used for CS/SS.
    - There are I2C (Wire) interfaces on pins 20 (SDA) and 21 (SCL) and 70 (SDA1) and 71 (SCL1).
    - This processor has one hardware serial port, USBSerial, which can _only_ be used for USB communication with a computer
    - There are three additional 3.3V TTL serial ports: Serial1 on pins 19 (RX) and 18 (TX); Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX). Pins 0 and 1 are also connected to the corresponding pins of the ATmega16U2 USB-to-TTL Serial chip, which is connected to the USB debug port.
    - AltSoftSerial is not directly supported on the AtSAM3X.
    - SoftwareSerial_ExtInts is not supported on the AtSAM3X.
    - Any digital pin can be used with SDI-12.
- **ATtiny** - Unsupported.  This chip has too little processing power and far too few pins and communication ports to use this library.
- **Teensy 2.x/3.x** - Unsupported
- **STM32** - Unsupported
- Anything else not listed above as being supported.


## <a name="contribute"></a>Contributing
Open an [issue](https://github.com/EnviroDIY/ModularSensors/issues) to suggest and discuss potential changes/additions.

For power contributors:

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

For those interested in creating wrapper functions for a new sensor, please contact us for guidance while we work on a guide. In brief, this library is built to fully take advantage of Objecting Oriented Programing (OOP) approaches. Each sensor is implemented as a subclass of the "Sensor" class contained in "SensorBase.h".  Each variable is separately implemented as a subclass of the "Variable" class contained in "VariableBase.h".  The variables are tied to the sensor using an "[Observer](https://en.wikipedia.org/wiki/Observer_pattern)" software pattern.


## <a name="license"></a>License
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

Hardware designs shared are released, unless otherwise indicated, under the [CERN Open Hardware License 1.2](http://www.ohwr.org/licenses/cern-ohl/v1.2) (CERN_OHL).

## <a name="acknowledgments"></a>Acknowledgments
[EnviroDIY](http://envirodiy.org/)™ is presented by the Stroud Water Research Center, with contributions from a community of enthusiasts sharing do-it-yourself ideas for environmental science and monitoring.

[Sara Damiano](https://github.com/SRGDamia1) is the primary developer of the EnviroDIY ModularSensors library, with input from many [other contributors](https://github.com/EnviroDIY/ModularSensors/graphs/contributors).

This project has benefited from the support from the following funders:

* William Penn Foundation
* US Environmental Protection Agency (EPA)
* National Science Foundation, awards [EAR-0724971](http://www.nsf.gov/awardsearch/showAward?AWD_ID=0724971), [EAR-1331856](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1331856), [ACI-1339834](http://www.nsf.gov/awardsearch/showAward?AWD_ID=1339834)
* Stroud Water Research Center endowment

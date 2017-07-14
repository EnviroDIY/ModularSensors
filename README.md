# ModularSensors

A "library" of sensors to give all sensors and variables a common interface of functions and returns and to make it very easy to iterate through and log data from many sensors and variables.  This library was written primarily for the [EnviroDIY Mayfly](https://envirodiy.org/mayfly/) but should be applicable to a variety of other Arduino-based boards as well.

Each sensor is implemented as a subclass of the "Sensor" class contained in "SensorBase.h".  Each variable is separately implemented as a subclass of the "Variable" class contained in "VariableBase.h".  The variables are tied to the sensor using an "[Observer](https://en.wikipedia.org/wiki/Observer_pattern)" software pattern.

To use a sensor and variable in your sketch, you must separately include xxx.h for each sensor you intend to use.  While this may force you to write many more include statements, it decreases the library RAM usage on your Arduino board.  Regardless of how many sensors you intend to use, however, you must install all of the dependent libraries on your _computer_ for the IDE to be able to compile the library.

#### Contents:
- [Physical Dependencies](#pdeps)
- [Library Dependencies](#ldeps)
- [Basic Sensor and Variable Functions](#Basic)
    - [Individual Sensors Code Examples](#individuals)
- [Grouped Sensor Functions](#Grouped)
    - [VariableArray Code Examples](#ArrayExamples)
    - [Tips for Ordering Sensors in Arrays](#ArrayTips)
- Logger Functions
    - [Basic Logger Functions](#Logger)
    - [Modem and Internet Functions](#Modem)
    - [EnviroDIY Logger Functions](#DIYlogger)
    - [Logger Code Examples](#LoggerExamples)
- Available Sensors
    - [MaxBotix MaxSonar](#MaxBotix)
    - [Campbell Scientific OBS-3+](#OBS3)
    - [Decagon Devices 5TM](#5TM)
    - [Decagon Devices CTD-10](#CTD)
    - [Decagon Devices ES-2](#ES2)
    - [Maxim DS18 Temperature Probes](#DS18)
    - [AOSong AM2315](#AM2315)
    - [Bosch BME280](#BME280)
    - [AOSong DHT](#DHT)
    - [Maxim DS3231 Real Time Clock](#DS3231)
    - [Processor Metadata Treated as Sensors](#Onboard)
- [Notes on Arduino Streams and Software Serial](#SoftwareSerial)
- [Processor/Board Compatibility](#compatibility)

## <a name="pdeps"></a>Physical Dependencies

This library is designed for remote sensing applications, that is, to log data from many physical sensors and to put the processor and all peripherals to sleep to conserver power between readings.  The most banal functions of the library require only an AVR or SAMD processor, but making real use of this library requires:

- A sufficiently powerful AVR or SAMD processor mounted on some sort of curcuit board.  (See [Processor/Board Compatibility](#compatibility) for more details on specific processors and boards.)
    - For all AVR processors, you must also have a [Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) high precision I2C real-time clock connected to your processor.
    - For SAMD boards, this library makes use of their on-board (though less accurate) real-time clock.
- A SD card reader attached to the processor via SPI.
- A modem-type unit to communicate remote data (See [Modem and Internet Functions](#Modem) for supported models.)
- A battery to power the system
- A solar charging curcuit
- Environmental sensors
- Protected water-proof enclosures and mountings for all of the above
- An OTG cable to connect serial output from the board to a cell phone (Not required, but very helpful for debugging.)

## <a name="ldeps"></a>Library Dependencies

In order to support multiple functions and sensors, there are quite a lot of sub-libraries that this library is dependent on.  _Even if you do not use all of the modules, you must have all of the dependencies installed for the library itself to properly compile._

- [EnableInterrupt](https://github.com/EnviroDIY/EnableInterrupt) - Administrates and handles pin change interrupts, allowing the logger to sleep and save battery.  This also controls the interrupts for the versions of SoftwareSerial and SDI-12 linked below that have been stripped of interrupt control.  Because we use this library, _you must always add the line ```#include <EnableInterrupt.h>``` to the top of your sketch._
- AVR sleep library - This is for low power sleeping for AVR processors. (This library is built in to the Arduino IDE.)
- [Sodaq](https://github.com/SodaqMoja/Sodaq_DS3231) or [EnviroDIY DS-3231](https://github.com/EnviroDIY/Sodaq_DS3231) - For real time clock control
- [RTCZero library](https://github.com/arduino-libraries/RTCZero) - This real time clock control and low power sleeping on SAMD processors. (This library may be built in to the Arduino IDE.)
- [SdFat library](https://github.com/greiman/SdFat) - This enables communication with the SD card.
- [EnviroDIY version of the TinyGSM library](https://github.com/EnviroDIY/TinyGSM) - This provides internet (TCP/IP) connectivity.
- [Adafruit ADS1X15 library](https://github.com/Adafruit/Adafruit_ADS1X15/) - For high-resolution analog to digital conversion.
- [EnviroDIY Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12/tree/ExtInts) - For control of SDI-12 based sensors.  This modified version is needed so there are no pin change interrupt conflicts with the SoftwareSerial library or the software pin change interrupt library used to wake the processor.
- [OneWire](https://github.com/PaulStoffregen/OneWire) - This enables communication with Maxim/Dallas OneWire devices.
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) - for communication with the DS18 line of Maxim/Dallas OneWire temperature probes.
- [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor)  
- [Adafruit AM2315 library](https://github.com/adafruit/Adafruit_AM2315) - for the AOSong AM2315 temperature and humidity sensor.
- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library) - for the Bosch BME280 environmental sensor.

## <a name="Basic"></a>Basic Senor and Variable Functions

### Functions Available for Each Sensor
- **Constructor** - Each sensor has a unique constructor, the exact format of which is dependent on the individual sensor.
- **getSensorName()** - This gets the name of the sensor and returns it as a string.
- **getSensorLocation()** - This returns the Arduino pin sending and recieving data or other sensor installation information as a string.  This is the location where the sensor is connected to the data logger, NOT the position of the sensor in the environment.
- **setup()** - This "sets up" the sensor - setting up serial ports, etc required for the given sensor.  This must always be called for each sensor within the "setup" loop of your Arduino program _before_ calling the variable setup.
- **getStatus()** - This returns the current status of the sensor as an interger, if the sensor has some way of giving it to you (most do not.)
- **printStatus()** - This returns the current status of the sensor as a readable String.
- **sleep()** - This puts the sensor to sleep, often by stopping the power.  Returns true.
- **wake()** - This wakes the sensor up and sends it power.  Returns true.
- **update()** - This updates the sensor values and returns true when finished.  For digital sensors with a single infomation return, this only needs to be called once for each sensor, even if there are multiple variable subclasses for the sensor.

### Functions for Each Variable
- **Constructor** - Every variable requires a pointer to its parent sensor as part of the constructor.
- **getVarName()** - This returns the variable's name using http://vocabulary.odm2.org/variablename/ as a String.
- **getVarUnit()** - This returns the variable's unit using http://vocabulary.odm2.org/units/ as a String.
- **getVarCode()** - This returns a String with a customized code for the variable, if one is given, and a default if not
- **setup()** - This "sets up" the variable - attaching it to its parent sensor.  This must always be called for each sensor within the "setup" loop of your Arduino program _after_ calling the sensor setup.
- **getValue()** - This returns the current value of the variable as a float.  You should call the update function before calling getValue.  As a backup, if the getValue function sees that the update function has not been called within the last 60 seconds, it will re-call it.
- **getValueString()** - This is identical to getValue, except that it returns a string with the proper precision available from the sensor.

### <a name="individuals"></a>Examples Using Individual Sensor and Variable Functions
To access and get values from a sensor, you must create an instance of the sensor class you are interested in using its constuctor.  Each variable has different parameters that you must specify; these are described below within the section for each sensor.  You must then create a new instance for each _variable_, and reference a pointer to the parent sensor in the constructor.  Many variables can (and should) call the same parent sensor.  The variables are specific to the individual sensor because each sensor collects data and returns data in a unique way.  The constructors are all best called outside of the "setup()" or "loop()" functions.  The setup functions are then called (sensor, then variables) in the main "setup()" function and the update() and getValues() are called in the loop().  A very simple program to get data from a Decagon CTD might be something like:

```cpp
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 10;  // The number of readings to average
const int SDI12Data = 7;  // The pin the CTD is attached to
const int SDI12Power = 22;  // The sensor power pin (use -1 if not applicable)
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, numberReadings);
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

The "[single_sensor](https://github.com/EnviroDIY/ModularSensors/tree/DS18B20/examples/single_sensor)" example in the examples folder shows the same functionality for a MaxBotix Ultrasonic Range Finder.

## <a name="Grouped"></a>Grouped Sensor Functions
Having a unified set of functions to access many sensors allows us to quickly poll through a list of sensors to get all results quickly.  To this end, "VariableArray.h" adds the class "VariableArray" with functions to use on an array of pointers to variable objects.

### Functions Available for a VariableArray Object:
- **init(int variableCount, Variable variableList[])** - This initializes the variable array.  This must be called in the setup() function.  Note that the objects in the variable list must be pointers, not the variable objects themselves.
- **getVariableCount()** - Simply returns the number of variables.
- **getSensorCount()** - Returns the number of independent sensors.  This will often be different from the number of variables because many sensors can return multiple variables.
- **setupSensors()** - This sets up all of the variables in the array and their respective sensors by running all of their setup() functions.  If a sensor doesn't respond to its setup command, the command is called 5 times in attempt to make a connection.  If all sensors are set up sucessfully, returns true.
- **sensorsSleep()** - This puts all sensors to sleep (ie, cuts power), skipping repeated sensors.  Returns true.
- **sensorsWake()** - This wakes all sensors (ie, gives power), skipping repeated sensors.  Returns true.
- **updateAllSensors()** - This updates all sensor values, skipping repeated sensors.  Returns true.  Does NOT return any values.
- **printSensorData(Stream stream)** - This prints current sensor values along with metadata to a stream (either hardware or software serial).  By default, it will print to the first Serial port.  Note that the input is a pointer to a stream instance so to use a hardware serial instance you must use an ampersand before the serial name (ie, &Serial1).
- **generateSensorDataCSV()** - This returns an Arduino String containing comma separated list of sensor values.  This string does _NOT_ contain a timestamp of any kind.

### <a name="ArrayExamples"></a>VariableArray Examples:

To use the VariableArray module, you must first create the array of pointers.  This should be done outside of the setup() or loop() functions.  Remember that you must create a new instance for each variable and each sensor.  All functions will be called on the variables in the order they appear in the list.  The functions for sensors will be called in the order that the last variable listed for that sensor appears.  The customVarCode is _always_ optional.

Following the example from above, with a Decagon CTD, you would create an array with the three CTD variables like this:

```cpp
// Create a new VariableArray object
VariableArray myVars;
// Create the array of variables named "variableList" using the pre-created variable objects
Variable *variableList[] = {*cond, *temp, *depth};
// Optionally, count the number of variables in the array (in this case, it's 3)
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
```

The asterix must be put in front of the variable name to indicate that it is a pointer to your variable object.  With many variables, it is easier to create the object and the pointer to it all at once in the variable array.  This can be done using the "new" keyword like so:

```cpp
// Create a new VariableArray object
VariableArray myVars;
// Create new variable objects in an array named "variableList" using the "new" keyword
Variable *variableList[] = {
    new Sensor1_Variable1(&parentSensor1, "customVarCode1"),
    new Sensor1_Variable2(&parentSensor1, "customVarCode2"),
    new Sensor2_Variable1(&parentSensor2, "customVarCode3"),
    ...
    new SensorX_VariableX(&parentSensorx, "customVarCode4")
};
// Optionally, count the number of variables in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
```

Once you have created the array of pointers, you can initialize the VariableArray module and setup all of the sensors at once in the setup function:

```cpp
// Initialize the sensor array;
myVars.init(variableCount, variableList);
// Set up all the sensors AND variables (both are done by this function)
myVars.setupSensors();
```

You can then get values or variable names for all of the sensors within the loop with calls like:

```cpp
// Update the sensor value(s)
myVars.updateAllSensors();
// Print the data to the screen
myVars.printSensorData();
```

### <a name="ArrayTips"></a>Tips for Ordering Sensors in Arrays:
If you are running sensors remotely on batteries and/or solar power, saving power and minimizing sensor-on time is a high priority.  To reduce the amount of time needed for sensor warm-up, it is best to look for readings first from the sensors that warm up the fastest and then to move on to the slower-booting sensors, allowing them to warm up while the faster sensors take readings.  This means that you should list those faster sensors first in your variable array and the slower sensors last.  Within the multisensor_print and other examples, the sensors are ordered this way, so can copy that order when creating your own logger program.


## <a name="Logger"></a>Basic Logger Functions
Our main reason to unify the output from many sensors and variables is to easily log the data to an SD card and to send it to any other live streaming data receiver, like the [EnviroDIY data portal](http://data.envirodiy.org/).  There are several modules available to use with the sensors to log data and stream data:  LoggerBase.h, LoggerEnviroDIY.h, and ModemSupport.h.  The classes Logger (in LoggerBase.h) is a sub-class of VariableArray and LoggerEnviroDIY (in LoggerEnviroDIY.h) is in-turn a sub-class of Logger.   They contain all of the functions available to a VariableArray as described above.  The Logger class adds the abilities to communicate with a DS3231 real time clock, to put the board into deep sleep between readings to conserver power, and to write the data from the sensors to a csv file on a connected SD card.  The ModemSupport module is essentially a wrapper for [TinyGSM](https://github.com/EnviroDIY/TinyGSM) which adds quick functions for turning modem on and off to save power and to synchronize the real-time clock with the [NIST Internet time service](https://www.nist.gov/pml/time-and-frequency-division/services/internet-time-service-its).  The LoggerEnviroDIY class uses ModemSupport.h to add the ability to properly format and send data to the [EnviroDIY data portal](http://data.envirodiy.org/).

### Functions Available for a Logger Object:
Timezone functions:
- **setTimeZone(int timeZone)** - Sets the timezone that you wish data to be logged in (in +/- hours from UTC).  _This must always be set!_
- **setTZOffset(int offset)** - This sets the offset between the built-in clock and the timezone the data should be logged in.  If your clock is set in UTC, then the TZOffset should be the same as the TimeZone.  For example, if you would like your clock to be set in UTC but your data should be output in Eastern Standard Time, both setTimeZone and setTZOffset should be called with -5.  On the other hand, if your clock is already set EST, you do not need to call the setTZOffset function (or can call it with 0).
A note about timezones:  It is possible to create multiple logger objects in your code if you want to log different sensors at different intervals, _but every logger object will always have the same timezone and timezone offset_.  If you attempt to call these functions more than once for different loggers, whatever value was called last will apply to every logger.

Setup and initialization functions:
- **init(int SDCardPin, int interruptPin, int variableCount, Sensor variableList[], float loggingIntervalMinutes, const char loggerID = 0)** - Initializes the logger object.  Must happen within the setup function.  Note that the variableList[], loggerID are all pointers.  The SDCardPin is the pin of the chip select/slave select for the SPI connection to the SD card.
- **setAlertPin(int ledPin)** - Optionally sets a pin to put out an alert that a measurement is being logged.  This is intended to be a pin with a LED on it so you can see the light come on when a measurement is being taken.

Functions to access the clock in proper format and time zone:
- **getNow()** - This gets the current epoch time (unix timestamp - number of seconds since Jan 1, 1970) and corrects it for the specified logger time zone offset.
- **formatDateTime_ISO8601(DateTime dt)** - Formats a DateTime object into an ISO8601 formatted Arduino String.
- **formatDateTime_ISO8601(uint32_t unixTime)** - Formats a unix timestamp into an ISO8601 formatted Arduino String.
- **checkInterval()** - This returns true if the _current_ time is an even iterval of the logging interval, otherwise false.  This uses getNow() to get the curernt time.
- **markTime()** - This sets static variables for the date/time - this is needed so that all data outputs (SD, EnviroDIY, serial printing, etc) print the same time for updating the sensors - even though the routines to update the sensors and to output the data may take several seconds.  It is not currently possible to output the instantaneous time an individual sensor was updated, just a single marked time.  By custom, this should be called before updating the sensors, not after.  If you do not call this function before saving or sending data, there will be no timestamps associated with your data.  This is called for you every time the checkInterval() function is run.
- **checkMarkedInterval()** - This returns true if the _marked_ time is an even iterval of the logging interval, otherwise false.  This uses the static time value set by markTime() to get the time.  It does not check the real-time-clock directly.


Functions for the sleep modes:
- **setupSleep()** - Sets up the processor sleep mode and the interrupts to wake the processor back up.  This should be called in the setup function.
- **systemSleep()** - Puts the system into deep sleep mode.  This should be called at the very end of the loop function.  Please keep in mind that this does NOT call the wake and sleep functions of the sensors themselves; you must call those separately.  (This separation is for timing reasons.)

Functions for logging data:
- **setFileName(fileName)** - This sets a specified file name for data to be saved as, if you want to decide on it in advance.  Note that you must include the file extention (ie., '.txt') in the file name.  If you do not call the setFileName function with a specific name, a csv file name will automatically be generated from the logger id and the current date.
- **getFileName()** - This returns the current filename as an Arduino String.
- **setupLogFile()** - This creates a file on the SD card and writes a header to it.  It also sets the "file created" time stamp.
- **logToSD(String rec)** - This writes a data line containing "rec" the the SD card and sets the "file modified" timestamp.  
- **generateFileHeader()** - This returns and Aruduino String with a comma separated list of headers for the csv.  The headers will be ordered based on the order variables are listed in the array fed to the init function.
- **generateSensorDataCSV()** - This returns an Arduino String containing the time and a comma separated list of sensor values.  The data will be ordered based on the order variables are listed in the array fed to the init function.

Functions for debugging sensors:
- **checkForDebugMode(int buttonPin, Stream \*stream = &Serial)** - This stops everything and waits for up to two seconds for a button to be pressed to enter allow the user to enter "debug" mode.  I suggest running this as the very last step of the setup function.
- **debugMode(Stream \*stream = &Serial)** - This is a "debugging" mode for the sensors.  It prints out all of the sensor details every 5 seconds for 25 records worth of data.
- Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.

Convience functions to do it all:
- **begin()** - Starts the logger.  Must be in the setup function.
- **log()** - Logs data, must be the entire content of the loop function.

### <a name="Modem"></a>Functions within ModemSupport/TinyGSM:
The "ModemSupport" bit of this library is essentially a wrapper for [TinyGSM](https://github.com/EnviroDIY/TinyGSM), to interface with the modem.  To make this work, you must add one of these lines _to the very top of your sketch_:
```cpp
// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for anything using a SIM800, SIM900, or varient thereof: Sodaq GPRSBees, Microduino GPRS chips, Adafruit Fona, etc
// #define TINY_GSM_MODEM_A6  // Select for A6 or A7 chips
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand XBee's, including WiFi or LTE-M1
// Create the modem object
loggerModem modem;
```
Any of the above modems types/chips should work, though only a SIM800 Sodaq GPRSBee, ESP8266, and Digi WiFiBee have been tested to date.  If you would prefer to use a library of your own for controlling your modem, omit the define statements.  In this case, you will lose access to the postDataEnviroDIY() and log() functions within the LoggerEnviroDIY object.

After defining your modem, set it up using one of these two commands, depending on whether you are using cellular or WiFi communication:
- **setupModem(Stream modemStream, int vcc33Pin, int status_CTS_pin, int onoff_DTR_pin, DTRSleepType sleepType, const char APN)** - Sets up the internet communcation with a cellular modem.  Note that the modemStream and APN should be pointers.  Use -1 for any pins that are not connected.
- **setupModem(Stream modemStream, int vcc33Pin, int status_CTS_pin, int onoff_DTR_pin, DTRSleepType sleepType, const char ssid, const char pwd)** - Sets up the internet communcation with a WiFi modem.  Note that the modemStream, ssid, and password should be pointers.  Use -1 for any pins that are not connected.
- The vcc33Pin is the pin that controls whether or not the modem itself is powered.  Use -1 if your modem is always recieving power from your logger board or if you want to control modem power independently.
- The status_CTS_pin is the pin that indicates whether the modem is turned on and it is clear to send data.  If you use -1, the modem is assumed to always be ready.
- The onoff_DTR_pin is the _pin_ used to put the modem to sleep or to wake it up.
- The DTRSleepType controls _how_ the modem is put to sleep between readings.  Use "held" if the DTR pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.  Use "pulsed" if the DTR pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.  Use "reverse" if the DTR pin is held LOW to keep the modem awake, as with all XBees.  Use "always_on" if you do not want the library to control the modem power and sleep.
- Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.

Once the modem has been set up, these functions are available:
- **on()** - Turns the modem on.  Returns true if connection is successful.
- **off()** - Turns the modem off and empties the send and receive buffer.  Returns true if connection is successful.
- **connectNetwork()** - Connects to the internet via WiFi or cellular network.  Returns true if connection is successful.
- **disconnectNetwork()** - Disconnects from the network, if applicable.
- **connect(const char host, uint16_t port)** - Makes a TCP connection to a host url and port.  (If you don't know the port, use "80".)  Returns 1 if successful.
- **stop()** - Breaks the TCP connection.
- **dumpBuffer(Stream stream, int timeDelay = 5, int timeout = 5000)** - Empties out the recieve buffer.  The flush() function does NOT empty the buffer, it only waits for sending to complete.
- **getNISTTime()** - Returns the current unix timestamp from NIST via the TIME protocol (rfc868).
- **syncDS3231()** - This synchronizes the DS3231 real time clock with the NIST provided timestamp.

The cellular modems themselves (SIM800, SIM900, A6, A7, and M590) can also be used as "sensors" which have the following variables:
```cpp
Modem_RSSI(&modem, "customVarCode");
Modem_SignalPercent(&modem, "customVarCode");
```
The modem does not behave as all the other sensors do, though.  The normal '''setup()''', '''wake()''', '''sleep()''', and '''update()''' functions for other sensors do not do anything with the modem.  Setup must be done with the '''setupModem(...)''' function; the modem will only go on and off with the '''on()''' and '''off()''' functions; and the '''update()''' functionality happens within the '''connectNetwork()''' function.


### <a name="DIYlogger"></a>Additional Functions Available for a LoggerEnviroDIY Object:
These three functions set up the required registration token, sampling feature UUID, and time series UUIDs for the EnviroDIY streaming data loader API.  **All three** functions must be called before calling any of the other EnviroDIYLogger functions.  All of these values can be obtained after registering at http://data.envirodiy.org/.  You must call these functions to be able to get proper JSON data for EnviroDIY, even without the modem support.
- **setToken(const char registrationToken)** - Sets the registration token to access the EnviroDIY streaming data loader API.  Note that the input is a pointer to the registrationToken.
- **setSamplingFeature(const char samplingFeature)** - Sets the GUID of the sampling feature.  Note that the input is a pointer to the samplingFeature.
- **setUUIDs(const char UUIDs)** - Sets the time series UUIDs.  Note that the input is an array of pointers.  The order of the UUIDs in this array **must match exactly** with the order of the coordinating variable in the variableList.

Because sending data to EnviroDIY depends on having some sort of modem or internet connection, there is a modem object created within the LoggerEnviroDIY Object.  To set up that modem object, you still need to call the functions listed in the ModemSupport section, but you need to add an extra "modem." before the function name to call the internal modem object.  You do not need to separately create the object.

Within the loop, these two functions will then format and send out data:
- **generateSensorDataJSON()** - Generates a properly formatted JSON string to go to the EnviroDIY streaming data loader API.
- **postDataEnviroDIY()** - Creates proper headers and sends data to the EnviroDIY data portal.  Depends on the modem support module.  Returns an HTML response code.

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
LoggerEnviroDIY EnviroDIYLogger;
```

_Within the setup function_, you must then initialize the logger and then run the logger setup.  For the EnviroDIY logger, you must also set up the communication.  (Please note that these are shown with default values.):

```cpp
// Set the time zone and offset from the RTC
logger.setTimeZone(timeZone);
logger.setTZOffset(offset);
// Initialize the logger;
logger.init(SDCardPin, interruptPin, variableCount, variableList, loggingIntervalMinutes, loggerID);
// OPTIONAL - specify a pin to give an alert when a measurement is taken
// This should generally be a pin with an LED
setAlertPin(int ledPin);
// Begin the logger;
logger.begin();
```

--OR--

```cpp
// Set the time zone and offset from the RTC
EnviroDIYLogger.setTimeZone(timeZone);
EnviroDIYLogger.setTZOffset(offset);
// Initialize the logger;
EnviroDIYLogger.init(SDCardPin, interruptPin, variableCount, variableList, loggingIntervalMinutes, loggerID);
// OPTIONAL - specify a pin to give an alert when a measurement is taken
// This should generally be a pin with an LED
setAlertPin(ledPin);
// Set up the communication with EnviroDIY
EnviroDIYLogger.setToken(registrationToken);
EnviroDIYLogger.setSamplingFeature(samplingFeature);
EnviroDIYLogger.setUUIDs(UUIDs[]);

// Set up the internal modem instance
EnviroDIYLogger.modem.setupModem(modemStream, vcc33Pin, status_CTS_pin, onoff_DTR_pin, sleepType, APN);

// Connect to the network
if (EnviroDIYLogger.modem.connectNetwork())
{
    // Synchronize the RTC
    EnviroDIYLogger.modem.syncDS3231();
}
// Disconnect from the network
EnviroDIYLogger.modem.disconnectNetwork();

// Begin the logger;
EnviroDIYLogger.begin();
```

_Within the main loop function_, all logging and sending of data can be done using a single program line.  Because the built-in log functions already handle sleeping and waking the board processor, **there cannot be nothing else within the loop function.**
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
- Wake up all your sensors with ```sensorsWake()```.
- Update all the sensors in your VariableArray together with ```updateAllSensors()```.
- Immediately after running ```updateAllSensors()```, put sensors to sleep to save power with ```sensorsSleep()```.
- After updating the sensors, then call any functions you want to send/print/save data.
- Finish by putting the logger back to sleep, if desired, with ```systemSleep()```.

The double_logger example program demonstrates using a custom loop function in order to log two different groups of sensors at different logging intervals.


## Available sensors

There are a number of sensors supported by this library.  Depending on the sensor, it may communicate with the Arduino board using as a serial peripheral interface (SPI), inter-integrated curcuit (I2C, also called "Wire"), or some type of universal synchronous/asynchronous receiver/transmitter (USART, almost always simply called "serial") (USART or serial includes transistor-transistor logic (TLL), RS232 (adapter needed), and RS485 (adapter needed) communication).  See the section on [Processor Compatibility](#compatibility) for more specific notes on which pins are available for each type of communication on the various supported processors.

Essentially all of the sensors can have their power supplies turned off between readings, but not all boards are able to switch output power on and off.  When the sensor constructor asks for the Arduino pin controlling power on/off, use -1 for any board which is not capable of switching the output power on and off.
_____

#### <a name="MaxBotix"></a>[MaxBotix MaxSonar](http://www.maxbotix.com/Ultrasonic_Sensors/High_Accuracy_Sensors.htm) - HRXL MaxSonar WR or WRS Series with TTL Outputs

The MaxBotix sensors communicate with the board using TTL from pin 5 on the sensor.  They require a 2.7V-5.5V power supply to pin 6 on the sensor (which can be turned off between measurements) and the level of the TLL returned by the sensor will match the power level it is supplied with.  Pin 7 of the sensor must be connected to ground and pin 4 can optionally be used to trigger the sensor.

The Arduino pin controlling power on/off, a stream instance for received data, and the Arduino pin controlling the trigger are required for the sensor constructor.  (Use -1 for the trigger pin if you do not have it connected.)  Please see the section "[Notes on Arduino Streams and Software Serial](#SoftwareSerial)" for more information about what streams can be used along with this library.

The main constuctor for the sensor object is:
```cpp
#include <MaxBotixSonar.h>
MaxBotixSonar sonar(SonarPower, sonarStream, SonarTrigger);
```

The single available variable is:  (customVarCode is optional)

```cpp
MaxBotixSonar_Range(&sonar, "customVarCode");
```

In addition to the constructors for the sensor and variable, you must remember to "begin" your stream instance within the main setup function.  The baud rate must be set to 9600 for all MaxBotix sensors.

```cpp
sonarStream.begin(9600);
```
_____

#### <a name="OBS3"></a>[Campbell Scientific OBS-3+](https://www.campbellsci.com/obs-3plus)

The OBS-3 sends out a simple analog signal between 0 and 2.5V.  To convert that to a high resolution digital signal, the sensor must be attached to a TI ADS1115 ADD converter (such as on the first four analog pins of the Mayfly).  The TI ADS1115 ADD communicates with the board via I2C and should have its address set as 0x48.  To use a different I2C address for the ADS1115, add the line `#define ADS1X15_ADDRESS (0x##)` to your sketch below the line `#include <CampbellOBS3.h>`.  The OBS-3 requires a 5-15V power supply, which can be turned off between measurements.  (It may actually run on power as low as 3.3V.)  The power supply is connected to the red wire, low range output comes from the blue wire, high range output comes from the white wire, and the black, greeen, and silver/unshielded wires should all be connected to ground.  The TI ADS1115 requires a 2-5.5V power supply.

The Arduino pin controlling power on/off, analog data pin _on the TI ADS1115_, and calibration values _in Volts_ for Ax^2 + Bx + C are required for the sensor constructor.  A custom variable code can be entered as a second argument in the variable constructors, and it is very strongly recommended that you use this otherwise it will be very difficult to determine which return is high and which is low range on the sensor.

Note that to access both the high and low range returns, two instances must be created, one at the low range return pin and one at the high pin.

The main constuctor for the sensor object is (called once each for high and low range):

```cpp
#include <CampbellOBS3.h>
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C);
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C);
```

The single available variable is (called once each for high and low range):

```cpp
CampbellOBS3_Turbidity(&osb3low, "customLowVarCode");
CampbellOBS3_Turbidity(&osb3high, "customHighVarCode");
```
_____

#### <a name="5TM"></a>[Decagon Devices 5TM](https://www.decagon.com/en/soils/volumetric-water-content-sensors/5tm-vwc-temp/) Soil Moisture and Temperature Sensor

Decagon sensors communicate with the board using the [SDI-12 protocol](http://www.sdi-12.org/) (and the [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12)).  They require a 3.5-12V power supply, which can be turned off between measurements.  While contrary to the manual, they will run with power as low as 3.3V.  On the 5TM with a stereo cable, the power is connected to the tip, data to the ring, and ground to the sleeve.  On the bare-wire version, the power is connected to the _white_ cable, data to _red_, and ground to the unshielded cable.

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, the Arduino pin sending and recieving data, and a number of distinct readings to average are required for the sensor constructor.  The data pin must be a pin that supports pin-change interrupts.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.

The main constuctor for the sensor object is:

```cpp
#include <Decagon5TM.h>
Decagon5TM fivetm(TMSDI12address, SDI12Power, SDI12Data, numberReadings);
```

The three available variables are:  (customVarCode is optional)

```cpp
Decagon5TM_Ea(&fivetm, "customVarCode");
Decagon5TM_Temp(&fivetm, "customVarCode");
Decagon5TM_VWC(&fivetm, "customVarCode");
```
_____

#### <a name="CTD"></a>[Decagon Devices CTD-5 or  CTD-10](https://www.decagon.com/en/hydrology/water-level-temperature-electrical-conductivity/ctd-10-sensor-electrical-conductivity-temperature-depth/) Electrical Conductivity, Temperature, and Depth Sensor

Decagon sensors communicate with the board using the [SDI-12 protocol](http://www.sdi-12.org/) (and the [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12)).  They require a 3.5-12V power supply, which can be turned off between measurements.  While contrary to the manual, they will run with power as low as 3.3V.  On the CTD with a stereo cable, the power is connected to the tip, data to the ring, and ground to the sleeve.  On the bare-wire version, the power is connected to the _white_ cable, data to _red_, and ground to both the black and unshielded cable.

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, the Arduino pin sending and recieving data, and a number of distinct readings to average are required for the sensor constructor.  The data pin must be a pin that supports pin-change interrupts.  For this particular sensor, taking ~6 readings seems to be ideal for reducing noise.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.

The main constuctor for the sensor object is:

```cpp
#include <DecagonCTD.h>
DecagonCTD ctd(CTDSDI12address, SDI12Power, SDI12Data, numberReadings);
```

The three available variables are:  (customVarCode is optional)

```cpp
DecagonCTD_Cond(&ctd, "customVarCode");
DecagonCTD_Temp(&ctd, "customVarCode");
DecagonCTD_Depth(&ctd, "customVarCode");
```
_____

#### <a name="ES2"></a>[Decagon Devices ES-2](http://www.decagon.com/en/hydrology/water-level-temperature-electrical-conductivity/es-2-electrical-conductivity-temperature/) Electrical Conductivity Sensor

Decagon sensors communicate with the board using the [SDI-12 protocol](http://www.sdi-12.org/) (and the [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12)).  They require a 3.5-12V power supply, which can be turned off between measurements.  While contrary to the manual, they will run with power as low as 3.3V.  On the ES-2 with a stereo cable, the power is connected to the tip, data to the ring, and ground to the sleeve.  On the bare-wire version, the power is connected to the _white_ cable, data to _red_, and ground to the unshielded cable.

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, the Arduino pin sending and recieving data, and a number of distinct readings to average are required for the sensor constructor.  The data pin must be a pin that supports pin-change interrupts.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.

The main constuctor for the sensor object is:

```cpp
#include <DecagonES2.h>
DecagonES2 es2(ES2SDI12address, SDI12Power, SDI12Data, numberReadings);
```

The two available variables are:  (customVarCode is optional)

```cpp
DecagonES2_Cond(&es2, "customVarCode");
DecagonES2_Temp(&es2, "customVarCode");
```
_____

#### <a name="DS18"></a>[Maxim DS18 Temperature Probes](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18S20.html)

The Maxim temperature probes communicate using the OneWire library, which can be used on any digital pin on any of the supported boards.  The same module should work with a [DS18B20](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html), [DS18S20](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18S20.html), [DS1822](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1822.html), [MAX31820](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/MAX31820.html), and the no-longer-sold [DS1820](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1820.html) sensor.  These sensors can be attached to a 3.0-5.5V power source or they can take "parasitic power" from the data line.  When using the more typical setup with power, ground, and data lines, a 4.7k resistor must be attached as a pullup between the data and power lines.

The OneWire hex address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and recieving data are required for the sensor constructor.  The hex address is an array of 8 hex values, for example:  {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 }.  To get the address of your sensor, plug a single sensor into your device and run the [oneWireSearch](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/oneWireSearch/oneWireSearch.ino) example or the [Single](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.pde) example provided within the Dallas Temperature library.  The sensor address is programmed at the factory and cannot be changed.

The main constuctor for the sensor object is:

```cpp
#include <MaximDS18.h>
MaximDS18 ds18(OneWireAddress, powerPin, dataPin);
```

_If and only if you have exactly one sensor attached on your OneWire pin or bus_, you can use this constructor to save yourself the trouble of finding the address:

```cpp
#include <MaximDS18.h>
MaximDS18 ds18(powerPin, dataPin);
```

The single available variable is:  (customVarCode is optional)

```cpp
MaximDS18_Temp(&ds18, "customVarCode");
```
_____

#### <a name="AM2315"></a>[AOSong AM2315](www.aosong.com/asp_bin/Products/en/AM2315.pdf) Encased I2C Temperature/Humidity Sensor

The AOSong AM2315 communicates with the board via I2C.  Because this sensor can have only one I2C address, it is only possible to connect one of these sensors to your system.  This sensor should be attached to a 3.3-5.5V power source and the power supply to the sensor can be stopped between measurements.

The only input needed for the sensor constructor is the Arduino pin controlling power on/off:

```cpp
#include <AOSongAM2315.h>
AOSongAM2315 am2315(I2CPower);
```

The two available variables are:  (customVarCode is optional)

```cpp
AOSongAM2315_Humidity(&am2315, "customVarCode");
AOSongAM2315_Temp(&am2315, "customVarCode");
```
_____

#### <a name="BME280"></a>[Bosch BME280](https://www.bosch-sensortec.com/bst/products/all_products/bme280) Integrated Environmental Sensor

Although this sensor has the option of either I2C or SPI communication, this library only supports I2C.  The I2C sensor address is assumed to be 0x76, though it can be changed to 0x77 in the constructor if necessary.  The sensor address is determined by how the sensor is soldered onto its breakout board.  To connect two of these sensors to your system, you must ensure they are soldered so as to have different I2C addresses.  No more than two can be attached.  This module is likely to also work with the [Bosch BMP280 Barometric Pressure Sensor](https://www.bosch-sensortec.com/bst/products/all_products/bmp280), though it has not been tested on it.  These sensors should be attached to a 1.7-3.6V power source and the power supply to the sensor can be stopped between measurements.

The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex is optional:

```cpp
#include <BoschBME280.h>
BoschBME280 bme280(I2CPower, i2cAddressHex);
```

The four available variables are:  (customVarCode is optional)

```cpp
BoschBME280_Temp(&bme280, "customVarCode");
BoschBME280_Humidity(&bme280, "customVarCode");
BoschBME280_Pressure(&bme280, "customVarCode");
BoschBME280_Altitude(&bme280, "customVarCode");
```
_____

#### <a name="DHT"></a>[AOSong DHT](http://www.aosong.com/en/products/index.asp) Digital-Output Relative Humidity & Temperature Sensor

This module will work with an AOSong DHT11, DHT21, AM2301, DHT22, or AM2302.  These sensors uses a non-standard 1-wire digital
signalling protocol.  They can be connected to any digital pin.  Please keep in mind that these sensors should not be updated more frequently than once ever 2 seconds.  These sensors should be attached to a 3.3-6V power source and the power supply to the sensor can be stopped between measurements.

The Arduino pin controlling power on/off, the Arduino pin receiving data, and the sensor type are required for the sensor constructor:

```cpp
#include <AOSongDHT.h>
AOSongDHT dht(DHTPower, DHTPin, dhtType);;
```

The three available variables are:  (customVarCode is optional)

```cpp
AOSongDHT_Humidity(&dht, "customVarCode");
AOSongDHT_Temp(&dht, "customVarCode");
AOSongDHT_HI(&dht, "customVarCode");  // Heat Index
```
_____

#### <a name="SQ212"></a>[Apogee SQ-212 Quantum Light Sensor ](https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/) Photosynthetically Active Radiation (PAR)
This library will work with the Apogee SQ-212 and SQ-212 analog quantum light sensors, and could be readily adapted to work with similar sensors (e.g. SQ-215 or SQ225) with by simply changing the calibration factors.  These sensors send out a simple analog signal.  To convert that to a high resolution digigal signal, the sensor must be attached to a TI ADS1115 ADD converter (such as on the first four analog pins of the Mayfly).  The TI ADS1115 ADD communicates with the board via I2C and should have its address set as 0x48.  To use a different I2C address for the ADS1115, add the line `#define ADS1X15_ADDRESS (0x##)` to your sketch below the line `#include <ApogeeSQ212.h>`.  The PAR sensors should be attached to a 5-24V power source and the power supply to the sensor can be stopped between measurements.  The TI ADS1115 requires a 2-5.5V power supply.

The Arduino pin controlling power on/off and the analog data pin _on the TI ADS1115_ are required for the sensor constructor:

```cpp
#include <ApogeeSQ212.h>
ApogeeSQ212 SQ212(SQ212Power, SQ212Data);
```

The one available variable is:  (customVarCode is optional)

```cpp
ApogeeSQ212_PAR(&SQ212, "customVarCode");  // Photosynthetically Active Radiation (PAR), in units of μmol m-2 s-1, or microeinsteinPerSquareMeterPerSecond
```
_____

#### <a name="DS3231"></a>Maxim DS3231 Real Time Clock]

As the I2C [Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) real time clock (RTC) is absolutely required for time-keeping on all AVR boards, this library also makes use of it for its on-board temperature sensor.  The DS3231 requires a 3.3V power supply.

There are no arguements for the constructor, as the RTC requires constant power and is connected via I2C, but due to a bug, you do have to input a "1" in the input:

```cpp
#include <OnboardSensors.h>
MaximDS3231 ds3231(1);
```

The only available variables is:  (customVarCode is optional)

```cpp
MaximDS3231_Temp(&ds3231, "customVarCode");
```
_____

#### <a name="Onboard"></a>Processor Metadata Treated as Sensors

The processor can return the amount of RAM it has available and, for some boards, the battery voltage (EnviroDIY Mayfly, Sodaq Mbili, Ndogo, Autonomo, and One, Adafruit Feathers).  The version of the board is required as input (ie, for a EnviroDIY Mayfly: "v0.3" or "v0.4" or "v0.5").  Use a blank value (ie, "") for un-versioned boards.

The main constuctor for the sensor object is:

```cpp
#include <OnboardSensors.h>
ProcessorMetadata mayfly(MFVersion);
```

The two available variables are:  (customVarCode is optional)

```cpp
ProcessorMetadata_Batt(&mayfly, "customVarCode");
ProcessorMetadata_FreeRam(&mayfly, "customVarCode");
```
_____

## <a name="SoftwareSerial"></a>Notes on Arduino Streams and Software Serial

In this library, the Arduino communicates with the computer for debugging, the modem for sending data, and some sensors (like the [MaxBotix MaxSonar](#MaxBotix)) via instances of Arduino TTL "[streams](https://www.arduino.cc/en/Reference/Stream)."  The streams can either be an instance of [serial](https://www.arduino.cc/en/Reference/Serial) (aka hardware serial), [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial), [the EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts), or any other stream type you desire.  The very commonly used build-in version of the software serial library for AVR processors uses interrupts that conflict with several other sub-libraries or this library and _cannot be used_!  I repeat:  _You cannot use the built in version of SoftwareSerial!_  You simply cannot.  It will not work.  Period.  This is not a bug that will be fixed.

For stream communication, hardware serial should always be your first choice, if your processor has enough hardware serial ports.  Hardware serial ports are the most stable and have the best performance of any of the other streams.  If the [proper pins](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) are available, [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial) by Paul Stoffregen is also superior to SoftwareSerial, especially at slow baud rates.  Neither hardware serial nor AltSoftSerial require any modifications.  Because of the limited number of serial ports available on most boards, I suggest giving first priority (ie the first (or only) hardware serial port, "Serial") to your debugging stream going to your PC (if you intend to debug), second priority to the stream for the modem, and third priority to any sensors that require a stream for communication.  See the section on [Processor Compatibility](#compatibility) for more specific notes on what serial ports are available on the various supported processors.

To use a hardware serial stream, you do not need to include any libraries:
```cpp
HardwareSerial* stream = &Serial;
```

To use AltSoftSerial:
```cpp
#include <AltSoftSerial.h>  // include the AltSoftSerial library
AltSoftSerial stream;  // Create an instance of AltSoftSerial
```

To use the EnviroDIY modified version of SoftwareSerial:
```cpp
#include <SoftwareSerial_ExtInts.h>  // include the SoftwareSerial library
SoftwareSerial_ExtInts stream(tx_pin, rx_pin);
```

After creating the stream instances, you must always remember to "begin" your stream within the main setup function.
```cpp
stream.begin(BAUD_RATE);
```

Additionally, for the EnviroDIY modified version of SoftwareSerial, you must enable the interrupts in your setup function:
```cpp
//  Allow enableInterrrupt to control the interrupts for software serial
enableInterrupt(stream, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
```

Here are some helpful links for more information about the number of serial ports available on some of the different Arduino-style boards:
- For Arduino brand boards:  https://www.arduino.cc/en/Reference/Serial
- For AtSAMD21 boards:  https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview

## <a name="compatibility"></a>Processor/Board Compatibility

AtMega1284p ([EnviroDIY Mayfly](https://envirodiy.org/mayfly/), Sodaq Mbili, Mighty 1284) - The Mayfly _is_ the test board for this library.  _Everything_ is designed to work with this processor.
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 13 (MOSI) on a Mayfly/Mbili or pins 6 (MISO), 7 (SCK), and 5 (MOSI) on a Mighty 1284 or other AtMega1284p.  Chip select/slave select is pin 12 on a Mayfly and card detect can be set to pin 18 with solder jumper 10.  CS/SS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- This processor has two built-in hardware serial ports, Serial and Serial1
    - On most boards, Serial is connected to the FDTI chip for USB communication with the computer.  On both the Mayfly and the Mbili Serial1 is wired to the "Bee" sockets for communication with the modem.
- To use AltSoftSerial with a Mayfly or Mbili you must add these lines to the top of your sketch:
```cpp
#define ALTSS_USE_TIMER1
#define INPUT_CAPTURE_PIN		6 // receive
#define OUTPUT_COMPARE_A_PIN		5 // transmit
#define OUTPUT_COMPARE_B_PIN	4 // unusable PWM
```
    - Unfortunately, the Rx and Tx pins are on different Grove plugs on both the Mayfly and the Mbili making AltSoftSerial rather inconvienent to use.  It can still be used with sensors like the MaxBotix that only require a recieve pin (and not send), but if you do this you must keep in mind that you cannot use pins 4 and 5 at all.
- To use AltSoftSerial with a Mighty 1284 or other AtMega1284p you must add these lines to the top of your sketch:
```cpp
#define ALTSS_USE_TIMER1
#define INPUT_CAPTURE_PIN		14 // receive
#define OUTPUT_COMPARE_A_PIN		13 // transmit
#define OUTPUT_COMPARE_B_PIN	12 // unusable PWM
```
- Any digital pin can be used with SoftwareSerial_ExtInts or SDI-12.
___

AtSAMD21 (Arduino Zero, Adafruit Feather M0, Sodaq Autonomo) - Not yet fully supported, but support is planned.
- This processor has an internal real time clock (RTC) and does not require a DS3231 to be installed.  The built-in RTC is not as accurate as the DS3231, however, and should be synchronized more frequently to keep the time correct.
- This processor has one hardware serial port, USBSerial, which can _only_ be used for USB communication with a computer
- Most variants have 2 other hardware serial ports (Serial on pins 30 (TX) and 31 (RX) and Serial1 on pins 0 (TX) and 1 (RX)) configured by default.
    - On an Arduino Zero Serial goes to the debug port.
    - On a Sodaq Autonomo Serial1 goes to the "Bee" port.
    - On an Adafruit Feather M0 only Serial1 is configured.
- Most variants have one SPI port configured by default (likely pins 22 (MISO), 23 (MOSI), and 24 (SCK)).  Chip select/slave select and card detect pins vary by board.
- Most variants have one I2C (Wire) interface configured by default (likely pins 20 (SDA) and 21 (SCL)).
- There are up to _6_ total "sercom" ports hard which can be configured for either hardware serial, SPI, or I2C (wire) communication on this processor.  See https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview for more instructions on how to configure these ports, if you need them.
- AltSoftSerial is not directly supported on the AtSAMD21, but with some effort, the timers could be configured to make it work.
- SoftwareSerial_ExtInts is not supported at all on the AtSAMD21.
- Any digital pin can be used with SDI-12.
___

AtMega2560 (Arduino Mega) - Should be fully functional, but untested.
- There is a single SPI port on pins 50 (MISO), 52 (SCK), and 51 (MOSI).  Chip select/slave select is on pin 53.
- There is a single I2C (Wire) interface on pins 20 (SDA) and 21 (SCL).
- This processor has 4 built-in hardware serial ports Serial, which is connected to the FTDI chip for USB communication with the computer, Serial1 on pins 19 (RX) and 18 (TX), Serial2 on pins 17 (RX) and 16 (TX), and Serial3 on pins 15 (RX) and 14 (TX).
- If you still need more serial ports, AltSoftSerial can be used on pins 46 (Tx) and 48 (Rx).  Pins 44 and 45 cannot be used while using AltSoftSerial on the AtMega2560.
- Pins 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), and A15 (69) can be used with SoftwareSerial_ExtInts or SDI-12.
___

AtMega644p (Sanguino) - Should be fully functional, but untested.
- This processor has two built-in hardware serial ports, Serial and Serial1.  On most boards, Serial is connected to the FDTI chip for USB communication with the computer.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- There is a single SPI port on pins 6 (MISO), 7 (SCK), and 5 (MOSI).  Chip select/slave select and card detect pins vary by board.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx).  Pin 12 cannot be used while using AltSoftSerial on the AtMega644p.
- Any digital pin can be used with SoftwareSerial_ExtInts or SDI-12.
___

AtMega328p (Arduino Uno, Duemilanove, LilyPad, Mini, Seeeduino Stalker, etc) - All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.  You will only be able to use a small number of sensors at one time and may not be able to log data.
- There is a singe SPI ports on pins 12 (MISO), 13 (SCK), and 11 (MOSI).  Chip select/slave select is pin 10 on an Uno.  SS/CS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins A4 (SDA) and A5 (SCL).
- This processor only has a single hardware serial port, Serial, which is connected to the FTDI chip for USB communication with the computer.
- AltSoftSerial can be used on pins 9 (Tx) and 8 (Rx).  Pin 10 cannot be used while using AltSoftSerial on the AtMega328p.
- Any digital pin can be used with SoftwareSerial_ExtInts or SDI-12.
___

AtMega32u4 (Arduino Leonardo/Micro, Adafruit Flora/Feather, etc) - All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.  You will only be able to use a small number of sensors at one time and may not be able to log data.
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 16 (MOSI).  Chip select/slave select and card detect pins vary by board.
- There is a single I2C (Wire) interface on pins 2 (SDA) and 3 (SCL).
- This processor has one hardware serial port, Serial, which can _only_ be used for USB communication with a computer
- There is one additional hardware serial port, Serial1, which can communicate with any serial device.
- AltSoftSerial can be used on pins 5 (Tx) and 13 (Rx).
- Only pins 8, 9, 10, 11, 14, 15, and 16 can be used with SoftwareSerial_ExtInts or SDI-12.  (And pins 14, 15, and 16 will be eliminated if you are using any SPI devices (like an SD card).)
___

Unsupported Processors:
- ESP8266/ESP32 - Supported only as a communications module (modem) with the default AT command firmware, not supported as an independent controller
- AtSAM3X (Arduino Due)
    - There is one SPI port on pins 74 (MISO), 76 (MOSI), and 75 (SCK).  Pins 4, 10 and pin 52 can be used for CS/SS.
    - There are I2C (Wire) interfaces on pins 20 (SDA) and 21 (SCL) and 70 (SDA1) and 71 (SCL1).
    - This processor has one hardware serial port, USBSerial, which can _only_ be used for USB communication with a computer
    - There are three additional 3.3V TTL serial ports: Serial1 on pins 19 (RX) and 18 (TX); Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX). Pins 0 and 1 are also connected to the corresponding pins of the ATmega16U2 USB-to-TTL Serial chip, which is connected to the USB debug port.
    - AltSoftSerial is not directly supported on the AtSAM3X.
    - SoftwareSerial_ExtInts is not supported on the AtSAM3X.
    - Any digital pin can be used with SDI-12.
- ATtiny - Unsupported
- Teensy 2.x/3.x - Unsupported
- STM32 - Unsupported
- Anything else not listed above as being supported.

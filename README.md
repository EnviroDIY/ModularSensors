# ModularSensors

A "library" of sensors to give all sensors and variables a common interface of functions and returns and to make it very easy to iterate through and log data from many sensors and variables.  This library was written primarily for the [EnviroDIY Mayfly](https://envirodiy.org/mayfly/) but should be applicable to other Arduino based boards as well.  To use the full functionality of this library, you do need an AVR board with a "large" amount of RAM (in Arduino terms).  The processor on an Arduino UNO or similar board is unlikely to be able to handle all of the logger functionality, though it will be able to access individual sensors.  An Arduino Mega, Sodaq Mbili, or other similar boards should be able to use the full library.  To date, however, the EnviroyDIY Mayfly is the only board that has been tested.

Each sensor is implemented as a subclass of the "Sensor" class contained in "SensorBase.h".  Each variable is separately implemented as a subclass of the "Variable" class contained in "VariableBase.h".  The variables are tied to the sensor using an "[Observer](https://en.wikipedia.org/wiki/Observer_pattern)" software pattern.

To use a sensor and variable in your sketch, you must separately include xxx.h for each sensor you intend to use.  While this may force you to write many more include statements, it makes the decreases the library RAM usage on your Arduino board.  Regardless of how many sensors you intend to use, however, you must install all of the dependent libraries on your _computer_ for the IDE to be able to compile the library.

#### Contents:
- [Library Dependencies](#deps)
- [Basic Senor and Variable Functions](#Basic)
    - [Individual Sensors Code Examples](#individuals)
- [Grouped Sensor Functions](#Grouped)
    - [VariableArray Code Examples](#ArrayExamples)
- Logger Functions
    - [Basic Logger Functions](#Logger)
    - [Modem and Internet Functions](#Modem)
    - [EnviroDIY Logger Functions](#DIYlogger)
    - [Logger Code Examples](#LoggerExamples)
- Available Sensors
    - [EnviroDIY Mayfly Onboard Sensors](#MayflyOnboard)
    - [MaxBotix MaxSonar](#MaxBotix)
    - [Campbell Scientific OBS-3+](#OBS3)
    - [Decagon Devices 5TM](#5TM)
    - [Decagon Devices CTD-10](#CTD)
    - [Decagon Devices ES-2](#ES2)
    - [Maxim DS18 Temperature Probes](#DS18)
    - [AOSong AM2315](#AM2315)
    - [Bosch BME280](#BME280)
    - [AOSong DHT](#DHT)


## <a name="deps"></a>Library Dependencies

In order to support multiple functions and sensors, there are quite a lot of dependencies for this library.  Even if you do not use all of the modules, you must have all of the dependencies installed for the library itself to properly compile.

- [Sodaq](https://github.com/SodaqMoja/Sodaq_DS3231) or [EnviroDIY DS-3231](https://github.com/EnviroDIY/Sodaq_DS3231) - For real time clock control
- [EnviroDIY modified version of Sodaq's pin change interrupt library](https://github.com/EnviroDIY/PcInt_PCINT0) - For waking the processor from clock alarms.  The modified version is needed so there are no pin change interrupt conflicts with the SDI-12 library or the SoftwareSerial library.
- AVR sleep library - This is for low power sleeping (This library is built in to the Arduino IDE.)
- [SdFat library](https://github.com/greiman/SdFat) - This enables communication with the SD card.
- [EnviroDIY version of the TinyGSM library](https://github.com/EnviroDIY/TinyGSM) - This provides internet (TCP/IP) connectivity.  
- [EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwareSerial_PCINT12/) - This modified version is needed so there are no pin change interrupt conflicts with the SDI-12 library or the software pin change interrupt library.
- [Adafruit ADS1X15 library](https://github.com/Adafruit/Adafruit_ADS1X15/) - For high-resolution analog to digital conversion.
- [EnviroDIY Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12/tree/mayfly) - For control of SDI-12 based sensors.  This modified version is needed so there are no pin change interrupt conflicts with the SoftwareSerial library or the software pin change interrupt library used to wake the processor.
- [OneWire](https://github.com/PaulStoffregen/OneWire) - This enables communication with Maxim/Dallas OneWire devices.
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) - for communication with the DS18 line of Maxim/Dallas OneWire temperature probes.
- [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor)  
- [Adafruit AM2315 library](https://github.com/adafruit/Adafruit_AM2315) - for the AOSong AM2315 temperature and humidity sensor.
- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library) - for the Bosch BME280 environmental sensor.

## <a name="Basic"></a>Basic Senor and Variable Functions

### Functions Available for Each Sensor
- **Constructor** - Each sensor has a unique constructor, the exact format of which is dependent on the indidual sensor.
- **getSensorName()** - This gets the name of the sensor and returns it as a string.
- **getSensorLocation()** - This returns the data pin or other sensor installation information as a string.  This is the location where the sensor is connected to the data logger, NOT the position of the sensor in the environment.
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
- **checkInterval()** - This returns true if the current time is an even iterval of the logging interval, otherwise false.  This uses getNow() to get the curernt time.
- **markTime()** - This sets variables for the date/time - this is needed so that all data outputs (SD, EnviroDIY, serial printing, etc) print the same time for updating the sensors - even though the routines to update the sensors and to output the data may take several seconds.  It is not currently possible to output the instantaneous time an individual sensor was updated, just a single marked time.  By custom, this should be called before updating the sensors, not after.  If you do not call this function before saving or sending data, there will be no timestamps associated with your data.  This is called for you every time the checkInterval() function is run.


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

Once the modem has been set up, these functions are available:
- **connectNetwork()** - Connects to the internet via WiFi or cellular network.  Returns true if connection is successful.
- **disconnectNetwork()** - Disconnects from the network, if applicable.
- **connect(const char host, uint16_t port)** - Makes a TCP connection to a host url and port.  (If you don't know the port, use "80".)  Returns 1 if successful.
- **stop()** - Breaks the TCP connection.
- **dumpBuffer(Stream *stream, int timeDelay = 5, int timeout = 5000)** - Empties out the recieve buffer.  Note since Arduino ~v1.0 that the flush() function does NOT empty the buffer.
- **getNISTTime()** - Returns the current unix timestamp from NIST via the TIME protocol  (rfc868).
- **syncDS3231()** - This synchronizes the DS3231 real time clock with the NIST provided timestamp. 


### <a name="DIYlogger"></a>Additional Functions Available for a LoggerEnviroDIY Object:
These three functions set up the required registration token, sampling feature uuid, and time series uuids for the EnviroDIY streaming data loader API.  **All three** functions must be called before calling any of the other EnviroDIYLogger functions.  All of these values can be obtained after registering at http://data.envirodiy.org/.  You must call these functions to be able to get proper JSON data for EnviroDIY, even without the modem support.
- **setToken(const char registrationToken)** - Sets the registration token to access the EnviroDIY streaming data loader API.  Note that the input is a pointer to the registrationToken.
- **setSamplingFeature(const char samplingFeature)** - Sets the GUID of the sampling feature.  Note that the input is a pointer to the samplingFeature.
- **setUUIDs(const char UUIDs[])** - Sets the time series UUIDs.  Note that the input is an array of pointers.  The order of the UUIDs in this array **must match exactly** with the order of the coordinating variable in the variableList.

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
- If you want to log on an even interval, use "if (checkInterval()" to verify the interval time.
- Call the markTime() function before printing/sending/saving any data that you want associate with a timestamp.
- Update all the sensors in your VariableArray together with updateAllSensors().
- Immediately after running updateAllSensors(), put sensors to sleep to save power with sensorsSleep().
- After updating the sensors, then call any functions you want to send/print/save data.
- Finish by putting the logger back to sleep, if desired, with systemSleep().


## Available sensors

#### <a name="MayflyOnboard"></a>Mayfly Onboard Sensors

The version of the Mayfly is required as input (ie, "v0.3" or "v0.4" or "v0.5").  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.

The main constuctor for the sensor object is:

```cpp
#include <MayflyOnboardSensors.h>
EnviroDIYMayfly mayfly(MFVersion);
```

The three available variables are:

```cpp
EnviroDIYMayfly_Temp(&mayfly, "customVarCode");
EnviroDIYMayfly_Batt(&mayfly, "customVarCode");
EnviroDIYMayfly_FreeRam(&mayfly, "customVarCode");
```

#### <a name="MaxBotix"></a>[MaxBotix MaxSonar](http://www.maxbotix.com/Ultrasonic_Sensors/High_Accuracy_Sensors.htm) - HRXL MaxSonar WR or WRS Series with TTL Outputs

The power/excite pin, digital data pin, and trigger are needed as input.  (Use -1 for the trigger if you do not have it connected.)  Because this library uses the [EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwareSerial_PCINT12/), the MaxBotix must be installed on a digital pin that depends on pin change interrupt vector 1 or 2.  On the Mayfly, the empty pins in this range are pins D10, D11, and D18.  (Changing the solder jumper options on the back of the board may eliminate D18 as a possibility.)  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.

The main constuctor for the sensor object is:

```cpp
#include <MaxBotixSonar.h>
MaxBotixSonar sonar(SonarPower, SonarData, SonarTrigger);
```

The single available variable is:

```cpp
MaxBotixSonar_Range(&sonar, "customVarCode");
```

#### <a name="OBS3"></a>[Campbell Scientific OBS-3+](https://www.campbellsci.com/obs-3plus)

The power pin, analog data pin, and calibration values _in Volts_ for Ax^2 + Bx + C are required as inputs and the sensor must be attached to a TI ADS1115 ADD converter (such as on the first four analog pins of the Mayfly).  A custom variable code can be entered as a second argument in the variable constructors, and it is very strongly recommended that you use this otherwise it will be very difficult to determine which return is high and which is low range on the sensor.

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

#### <a name="5TM"></a>[Decagon Devices 5TM](https://www.decagon.com/en/soils/volumetric-water-content-sensors/5tm-vwc-temp/) Soil Moisture and Temperature Sensor

The SDI-12 address of the sensor, the power pin, the data pin, and a number of distinct readings to average are required as inputs.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.  Because this library uses a modified version of the basic SDI-12 library, the 5TM (and all SDI-12 based sensors) must be installed on on of the digital pins that depends on pin change interrupt vector 3.  On the Mayfly, the empty pins in this range are pins D4, D5, D6, and D7.

The main constuctor for the sensor object is:

```cpp
#include <Decagon5TM.h>
Decagon5TM fivetm(TMSDI12address, SDI12Power, SDI12Data, numberReadings);
```

The three available variables are:

```cpp
Decagon5TM_Ea(&fivetm, "customVarCode");
Decagon5TM_Temp(&fivetm, "customVarCode");
Decagon5TM_VWC(&fivetm, "customVarCode");
```

#### <a name="CTD"></a>[Decagon Devices CTD-5 or  CTD-10](https://www.decagon.com/en/hydrology/water-level-temperature-electrical-conductivity/ctd-10-sensor-electrical-conductivity-temperature-depth/) Electrical Conductivity, Temperature, and Depth Sensor

The SDI-12 address of the sensor, the power pin, the data pin, and a number of distinct readings to average are required as inputs.  For this particular sensor, taking ~6 readings seems to be ideal for reducing noise.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.  Because this library uses a modified version of the basic SDI-12 library, the 5TM (and all SDI-12 based sensors) must be installed on on of the digital pins that depends on pin change interrupt vector 3.  On the Mayfly, the empty pins in this range are pins D4, D5, D6, and D7.

The main constuctor for the sensor object is:

```cpp
#include <DecagonCTD.h>
DecagonCTD ctd(CTDSDI12address, SDI12Power, SDI12Data, numberReadings);
```

The three available variables are:

```cpp
DecagonCTD_Cond(&ctd, "customVarCode");
DecagonCTD_Temp(&ctd, "customVarCode");
DecagonCTD_Depth(&ctd, "customVarCode");
```

#### <a name="ES2"></a>[Decagon Devices ES-2](http://www.decagon.com/en/hydrology/water-level-temperature-electrical-conductivity/es-2-electrical-conductivity-temperature/) Electrical Conductivity Sensor

The SDI-12 address of the sensor, the power pin, the data pin, and a number of distinct readings to average are required as inputs.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.  To find or change the SDI-12 address of your sensor, load and run example [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change) within the SDI-12 library.  Because this library uses a modified version of the basic SDI-12 library, the 5TM (and all SDI-12 based sensors) must be installed on on of the digital pins that depends on pin change interrupt vector 3.  On the Mayfly, the empty pins in this range are pins D4, D5, D6, and D7.

The main constuctor for the sensor object is:

```cpp
#include <DecagonES2.h>
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data, numberReadings);
```

The two available variables are:

```cpp
DecagonES2_Cond(&es2, "customVarCode");
DecagonES2_Temp(&es2, "customVarCode");
```


#### <a name="DS18"></a>[Maxim DS18 Temperature Probes](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18S20.html)

The same library should work with a [DS18B20](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html), [DS18S20](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18S20.html), [DS1822](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1822.html), and the no-longer-sold [DS1820](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS1820.html) sensor.  The OneWire hex address of the sensor, the power pin, and the data pin, are required as inputs.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.  The hex address is an array of 8 hex values, for example:  {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 }.  To get the address of your sensor, plug a single sensor into your device and run the [oneWireSearch](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/oneWireSearch/oneWireSearch.ino) example or the [Single](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.pde) example provided within the Dallas Temperature library.  The sensor address is programmed at the factory and cannot be changed.

The main constuctor for the sensor object is:

```cpp
#include <MaximDS18.h>
MaximDS18 ds18(OneWireAddress, powerPin, dataPin);
```

_If and only you have exactly one sensor attached on your OneWire pin or bus_, you can use this constructor to save yourself the trouble of finding the address:

```cpp
#include <MaximDS18.h>
MaximDS18 ds18(powerPin, dataPin);
```

The single available variable is:

```cpp
MaximDS18_Temp(&ds18, "customVarCode");
```

#### <a name="AM2315"></a>[AOSong AM2315](www.aosong.com/asp_bin/Products/en/AM2315.pdf) Encased I2C Temperature/Humidity Sensor

Because this is an I2C sensor, the only input needed is the power pin.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.  Because this sensor can have only one I2C address, it is only possible to connect one of these sensors to your system.

The main constuctor for the sensor object is:

```cpp
#include <AOSongAM2315.h>
AOSongAM2315 am2315(I2CPower);
```

The two available variables are:

```cpp
AOSongAM2315_Humidity(&am2315, "customVarCode");
AOSongAM2315_Temp(&am2315, "customVarCode");
```

#### <a name="BME280"></a>[Bosch BME280](https://www.bosch-sensortec.com/bst/products/all_products/bme280) Integrated Environmental Sensor

Although this sensor has the option of either I2C or SPI communication, this library only supports I2C.  The only input needed is the power pin; the I2C sensor address is assumed to be 0x76, though it can be changed to 0x77 in the constructor if necessary.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.  To connect two of these sensors to your system, you must ensure they are soldered so as to have different I2C addresses.  No more than two can be attached.  This module is likely to also work with the [Bosch BMP280 Barometric Pressure Sensor](https://www.bosch-sensortec.com/bst/products/all_products/bmp280), though it has not been tested on it.

The main constuctor for the sensor object is:

```cpp
#include <BoschBME280.h>
BoschBME280 bme280(I2CPower, i2cAddressHex);
```

The four available variables are:

```cpp
BoschBME280_Temp(&bme280, "customVarCode");
BoschBME280_Humidity(&bme280, "customVarCode");
BoschBME280_Pressure(&bme280, "customVarCode");
BoschBME280_Altitude(&bme280, "customVarCode");
```

#### <a name="DHT"></a>[AOSong DHT](http://www.aosong.com/en/products/index.asp) Digital-Output Relative Humidity & Temperature Sensor

This library will work with an AOSong DHT11, DHT21, AM2301, DHT22, or AM2302.  Please keep in mind that these sensors should not be updated more frequently than once ever 2 seconds.  The data pin, power pin, and sensor type are needed as inputs.  A custom variable code can _optionally_ be entered as a second argument in the variable constructors.

The main constuctor for the sensor object is:

```cpp
#include <AOSongDHT.h>
AOSongDHT dht(DHTPower, DHTPin, dhtType);;
```

The three available variables are:

```cpp
AOSongDHT_Humidity(&dht, "customVarCode");
AOSongDHT_Temp(&dht, "customVarCode");
AOSongDHT_HI(&dht, "customVarCode");  // Heat Index
```

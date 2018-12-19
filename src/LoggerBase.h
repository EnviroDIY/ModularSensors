/*
 *LoggerBase.h

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

// Header Guards
#ifndef LoggerBase_h
#define LoggerBase_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
//#include <Arduino.h>
#include "ModSensorDebugger.h"
#include "VariableArray.h"
#include "LoggerModem.h"

// Bring in the libraries to handle the processor sleep/standby modes
// The SAMD library can also the built-in clock on those modules
#if defined(ARDUINO_ARCH_SAMD)
  #include <RTCZero.h>
#elif defined __AVR__
  #include <avr/sleep.h>
  #include <avr/power.h>
#endif

// Bring in the library to communicate with an external high-precision real time clock
// This also implements a needed date/time class
#include <Sodaq_DS3231.h>
#define EPOCH_TIME_OFF 946684800
// This is 2000-jan-01 00:00:00 in "epoch" time
// Need this b/c the date/time class in Sodaq_DS3231 treats a 32-bit long
// timestamp as time from 2000-jan-01 00:00:00 instead of the standard (unix)
// epoch beginning 1970-jan-01 00:00:00.

#include <SdFat.h>  // To communicate with the SD card

// The largest number of variables from a single sensor
#define MAX_NUMBER_SENDERS 4


class dataSender;  // Forward declaration


// Defines the "Logger" Class
class Logger
{

public:
    // Constructor
    Logger(const char *loggerID, uint16_t loggingIntervalMinutes,
           int8_t SDCardPin, int8_t mcuWakePin,
           VariableArray *inputArray);
    // Destructor
    virtual ~Logger();

    // ===================================================================== //
    // Public functions to get and set basic logging paramters
    // ===================================================================== //

    // A pointer to the internal variable array instance
    const char * getLoggerID(){return _loggerID;}

    // Adds the sampling feature UUID
    void setSamplingFeatureUUID(const char *samplingFeatureUUID);
    const char * getSamplingFeatureUUID(){return _samplingFeatureUUID;}

    // Sets up a pin for an LED or other way of alerting that data is being logged
    void setAlertPin(int8_t ledPin);
    void alertOn();
    void alertOff();

    // Sets up a pin for an interrupt to enter testing mode
    void setTestingModePin(int8_t buttonPin);

protected:
    // Initialization variables
    const char *_loggerID;
    uint16_t _loggingIntervalMinutes;
    int8_t _SDCardPin;
    int8_t _mcuWakePin;
    int8_t _ledPin;
    int8_t _buttonPin;
    bool _areSensorsSetup;
    const char *_samplingFeatureUUID;

    // ===================================================================== //
    // Public functions to get information about the attached variable array
    // ===================================================================== //

public:
    // Returns the number of variables in the internal array
    uint8_t getArrayVarCount();

    // These are reflections of the variable returns
    // This gets the name of the parent sensor, if applicable
    String getParentSensorNameAtI(uint8_t position_i);
    // This gets the name and location of the parent sensor, if applicable
    String getParentSensorNameAndLocationAtI(uint8_t position_i);
    // This gets the variable's name using http://vocabulary.odm2.org/variablename/
    String getVarNameAtI(uint8_t position_i);
    // This gets the variable's unit using http://vocabulary.odm2.org/units/
    String getVarUnitAtI(uint8_t position_i);
    // This returns a customized code for the variable, if one is given, and a default if not
    String getVarCodeAtI(uint8_t position_i);
    // This returns the variable UUID, if one has been assigned
    String getVarUUIDAtI(uint8_t position_i);
    // This returns the current value of the variable as a string with the
    // correct number of significant figures
    String getValueStringAtI(uint8_t position_i);

protected:
    // A pointer to the internal variable array instance
    VariableArray *_internalArray;

    // ===================================================================== //
    // Public functions for internet and dataSenders
    // ===================================================================== //

public:
    // Adds a loggerModem objct to the logger
    // loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
    void attachModem(loggerModem& modem);

    // Takes advantage of the modem to synchronize the clock
    bool syncRTC();

    // These tie the variables to their parent sensor
    void registerDataSender(dataSender* sender);
    // Notifies attached variables of new values
    void sendDataToRemotes(void);

protected:
    // The internal modem instance
    loggerModem *_logModem;
    // NOTE:  The internal _logModem must be a POINTER not a reference because
    // it is possible for no modem to be attached (and thus the pointer could
    // be null).  It is not possible to have a null reference.

    // An array of all of the attached data senders
    dataSender *dataSenders[MAX_NUMBER_SENDERS];

    // ===================================================================== //
    // Public functions to access the clock in proper format and time zone
    // ===================================================================== //

public:
    // Sets the static timezone - this must be set
    static void setTimeZone(int8_t timeZone);
    static int8_t getTimeZone(void) { return Logger::_timeZone; }

    // This set the offset between the built-in clock and the time zone where
    // the data is being recorded.  If your RTC is set in UTC and your logging
    // timezone is EST, this should be -5.  If your RTC is set in EST and your
    // timezone is EST this does not need to be called.
    static void setTZOffset(int8_t offset);
    static int8_t getTZOffset(void) { return Logger::_offset; }

    // This gets the current epoch time (unix time, ie, the number of seconds
    // from January 1, 1970 00:00:00 UTC) and corrects it for the specified time zone
    #if defined(ARDUINO_ARCH_SAMD)
        static RTCZero zero_sleep_rtc;  // create the rtc object
    #endif

    static uint32_t getNowEpoch(void);
    static void setNowEpoch(uint32_t ts);

    static DateTime dtFromEpoch(uint32_t epochTime);

    // This converts a date-time object into a ISO8601 formatted string
    static String formatDateTime_ISO8601(DateTime& dt);

    // This converts an epoch time (unix time) into a ISO8601 formatted string
    static String formatDateTime_ISO8601(uint32_t epochTime);

    // This sets the real time clock to the given time
    bool setRTClock(uint32_t setTime);

    // This sets static variables for the date/time - this is needed so that all
    // data outputs (SD, EnviroDIY, serial printing, etc) print the same time
    // for updating the sensors - even though the routines to update the sensors
    // and to output the data may take several seconds.
    // It is not currently possible to output the instantaneous time an individual
    // sensor was updated, just a single marked time.  By custom, this should be
    // called before updating the sensors, not after.
    static void markTime(void);

    // This checks to see if the CURRENT time is an even interval of the logging rate
    // or we're in the first 15 minutes of logging
    bool checkInterval(void);

    // This checks to see if the MARKED time is an even interval of the logging rate
    // or we're in the first 15 minutes of logging
    bool checkMarkedInterval(void);

protected:
    // Static variables - identical for EVERY logger
    static int8_t _timeZone;
    static int8_t _offset;

    // ============================================================================
    //  Public Functions for sleeping the logger
    // ============================================================================

public:
    // Set up the Interrupt Service Request for waking
    // In this case, we're doing nothing, we just want the processor to wake
    // This must be a static function (which means it can only call other static funcions.)
    static void wakeISR(void);

    // Sets up the sleep mode
    void setupSleep(void);

    // Puts the system to sleep to conserve battery life.
    // This DOES NOT sleep or wake the sensors!!
    void systemSleep(void);

    // ===================================================================== //
    // Public functions for logging data to an SD card
    // ===================================================================== //

public:
    // This sets a file name, if you want to decide on it in advance
    void setFileName(const char *fileName);
    // Same as above, with a string (overload function)
    void setFileName(String& fileName);

    // This returns the current filename.  Must be run after setFileName.
    String getFileName(void){return _fileName;}

    // This prints a header onto a stream - this removes need to pass around
    // very long string objects which can crash the logger
    virtual void printFileHeader(Stream *stream);

    // This prints a comma separated list of volues of sensor data - including the
    // time -  out over an Arduino stream
    void printSensorDataCSV(Stream *stream);

    // These functions create a file on an SD card and set the created/modified/
    // accessed timestamps in that file.
    // The filename may either be the one automatically generated by the logger
    // id and the date, the one set by setFileName(String), or can be specified
    // in the function.
    // If asked to, these functions will also write a header to the file based
    // on the variable information from the variable array.
    // This can be used to force a logger to create a file with a secondary file name.
    bool createLogFile(String& filename, bool writeDefaultHeader = false);
    bool createLogFile(bool writeDefaultHeader = false);

    // These functions create a file on an SD card and set the modified/accessed
    // timestamps in that file.
    // The filename may either be the one automatically generated by the logger
    // id and the date, the one set by setFileName(String), or can be specified
    // in the function.
    // If the file does not already exist, the file will be created.
    // The line to be written to the file can either be specified or will be
    // a comma separated list of the current values of all variables in the
    // variable array.
    bool logToSD(String& filename, String& rec);
    bool logToSD(String& rec);
    bool logToSD(void);

protected:

    // The SD card and file
    SdFat sd;
    File logFile;
    String _fileName;

    // This checks if the SD card is available and ready
    // We run this check before every communication with the SD card to prevent
    // hanging.
    bool initializeSDCard(void);

    // This generates a file name from the logger id and the current date
    // NOTE:  This cannot be called until *after* the RTC is started
    void generateAutoFileName(void);

    // This sets a timestamp on a file
    void setFileTimestamp(File fileToStamp, uint8_t stampFlag);

    // This opens or creates a file, converting a string file name to a
    // character file name
    bool openFile(String& filename, bool createFile, bool writeDefaultHeader);


    // ===================================================================== //
    // Public functions for a "sensor testing" mode
    // ===================================================================== //

public:
    // This checks to see if you want to enter the sensor mode
    // This should be run as the very last step within the setup function
    // void checkForTestingMode(int8_t buttonPin);

    // A function if you'd prefer to enter testing based on an interrupt
    static void testingISR(void);

    // This defines what to do in the testing mode
    virtual void testingMode();


    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //

    // Setup the sensors and log files
    virtual void setupSensorsAndFile(void);

    // This does all of the setup that can't happen in the constructors
    // That is, things that require the actual processor/MCU to do something
    // rather than the compiler to do something.
    virtual void begin(bool skipSensorSetup = false);

    // This is a one-and-done to log data
    virtual void logData(void);
    void logDataAndSend(void);

    // Public variables
    // Time stamps - want to set them at a single time and carry them forward
    static uint32_t markedEpochTime;

    // These are flag fariables noting the current state (logging/testing)
    // NOTE:  if the logger isn't currently logging or testing or in the middle
    // of set-up, it's probably sleeping
    // Setting these as volatile because the flags can be changed in ISR's
    static volatile bool isLoggingNow;
    static volatile bool isTestingNow;
    static volatile bool startTesting;

};

#endif  // Header Guard

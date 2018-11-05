/*
 *LoggerBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

#include "LoggerBase.h"

// To prevent compiler/linker crashes with Enable interrupt
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include <EnableInterrupt.h>
// For all i2c communication, including with the real time clock
#include <Wire.h>


// Initialize the static timezone
int8_t Logger::_timeZone = 0;
// Initialize the static time adjustment
int8_t Logger::_offset = 0;
// Initialize the static timestamps
uint32_t Logger::markedEpochTime = 0;
// Initialize the testing/logging flags
volatile bool Logger::isLoggingNow = false;
volatile bool Logger::isTestingNow = false;
volatile bool Logger::startTesting = false;

// Initialize the RTC for the SAMD boards
#if defined(ARDUINO_ARCH_SAMD)
    RTCZero Logger::zero_sleep_rtc;
#endif


// Constructor
Logger::Logger(const char *loggerID, uint16_t loggingIntervalMinutes,
               int8_t SDCardPin, int8_t mcuWakePin,
               VariableArray *inputArray)
{
    // Set parameters from constructor
    _loggerID = loggerID;
    _loggingIntervalMinutes = loggingIntervalMinutes;
    _SDCardPin = SDCardPin;
    _mcuWakePin = mcuWakePin;
    _internalArray = inputArray;

    // Mark sensor set-up as not set up
    _areSensorsSetup = 0;

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Initialize with informational pins set void
    _ledPin = -1;
    _buttonPin = -1;

    // Initialize with no file name
    _fileName = "";
    _autoFileName = true;
}
// Destructor
Logger::~Logger(){}


// Sets the static timezone - this must be set
void Logger::setTimeZone(int8_t timeZone)
{
    _timeZone = timeZone;
    // Some helpful prints for debugging
    #ifdef STANDARD_SERIAL_OUTPUT
        const char* prtout1 = "Logger timezone is set to UTC";
        if (_timeZone == 0) PRINTOUT(prtout1);
        else if (_timeZone > 0) PRINTOUT(prtout1, '+', _timeZone);
        else PRINTOUT(prtout1, _timeZone);
    #endif
}


// This set the offset between the built-in clock and the time zone where
// the data is being recorded.  If your RTC is set in UTC and your logging
// timezone is EST, this should be -5.  If your RTC is set in EST and your
// timezone is EST this does not need to be called.
void Logger::setTZOffset(int8_t offset)
{
    _offset = offset;
    // Some helpful prints for debugging
    #ifdef STANDARD_SERIAL_OUTPUT
        const char* prtout1 = "RTC timezone is set to UTC";
        if ((_timeZone - _offset) == 0) PRINTOUT(prtout1);
        else if ((_timeZone - _offset) > 0) PRINTOUT(prtout1, '+', (_timeZone - _offset));
        else PRINTOUT(prtout1, (_timeZone - _offset));
    #endif
}


// Sets up a pin for an LED or other way of alerting that data is being logged
void Logger::setAlertPin(int8_t ledPin)
{
    _ledPin = ledPin;
    MS_DBG(F("Pin "), _ledPin, F(" set as LED alert pin"));
}


// Sets up a pin for an interrupt to enter testing mode
void Logger::setTestingModePin(int8_t buttonPin)
{
    _buttonPin = buttonPin;
    MS_DBG(F("Pin "), _buttonPin, F(" set as testing mode entry pin"));
}


// ===================================================================== //
// Public functions to access the clock in proper format and time zone
// ===================================================================== //

// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) and corrects it for the specified time zone
#if defined(ARDUINO_ARCH_SAMD)

    uint32_t Logger::getNowEpoch(void)
    {
      uint32_t currentEpochTime = zero_sleep_rtc.getEpoch();
      currentEpochTime += _offset*3600;
      return currentEpochTime;
    }
    void Logger::setNowEpoch(uint32_t ts){zero_sleep_rtc.setEpoch(ts);}

#else
    // Do not need to create the RTC object; it's created on library import
    uint32_t Logger::getNowEpoch(void)
    {
      uint32_t currentEpochTime = rtc.now().getEpoch();
      currentEpochTime += _offset*3600;
      return currentEpochTime;
    }
    void Logger::setNowEpoch(uint32_t ts){rtc.setEpoch(ts);}
#endif

// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) and corrects it for the specified time zone
DateTime Logger::dtFromEpoch(uint32_t epochTime)
{
    DateTime dt(epochTime - EPOCH_TIME_OFF);
    return dt;
}

// This converts a date-time object into a ISO8601 formatted string
String Logger::formatDateTime_ISO8601(DateTime& dt)
{
    // Set up an inital string
    String dateTimeStr;
    // Convert the DateTime object to a String
    dt.addToString(dateTimeStr);
    dateTimeStr.replace(" ", "T");
    String tzString = String(_timeZone);
    if (-24 <= _timeZone && _timeZone <= -10)
    {
        tzString += F(":00");
    }
    else if (-10 < _timeZone && _timeZone < 0)
    {
        tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");
    }
    else if (_timeZone == 0)
    {
        tzString = F("Z");
    }
    else if (0 < _timeZone && _timeZone < 10)
    {
        tzString = "+0" + tzString + F(":00");
    }
    else if (10 <= _timeZone && _timeZone <= 24)
    {
        tzString = "+" + tzString + F(":00");
    }
    dateTimeStr += tzString;
    return dateTimeStr;
}


// This converts an epoch time (unix time) into a ISO8601 formatted string
String Logger::formatDateTime_ISO8601(uint32_t epochTime)
{
    // Create a DateTime object from the epochTime
    DateTime dt = dtFromEpoch(epochTime);
    return formatDateTime_ISO8601(dt);
}


// This syncronizes the real time clock
bool Logger::syncRTClock(uint32_t nist)
{
    uint32_t start_millis = millis();

    // If the timestamp is zero, just exit
    if  (nist == 0)
    {
        PRINTOUT(F("Bad timestamp, skipping sync."));
        return false;
    }

    uint32_t nist_logTZ = nist + getTimeZone()*3600;
    uint32_t nist_rtcTZ = nist_logTZ - getTZOffset()*3600;
    MS_DBG(F("         Correct Time for Logger: "), nist_logTZ, F(" -> "), \
        formatDateTime_ISO8601(nist_logTZ));

    // See how long it took to get the time from NIST
    uint32_t sync_time = (millis() - start_millis)/1000;

    // Check the current RTC time
    uint32_t cur_logTZ = getNowEpoch();
    MS_DBG(F("            Time Returned by RTC: "), cur_logTZ, F(" -> "), \
        formatDateTime_ISO8601(cur_logTZ));
    MS_DBG(F("Offset: "), abs(nist_logTZ - cur_logTZ));

    // If the RTC and NIST disagree by more than 5 seconds, set the clock
    if ((abs(nist_logTZ - cur_logTZ) > 5) && (nist != 0))
    {
        setNowEpoch(nist_rtcTZ + sync_time/2);
        PRINTOUT(F("Clock synced!"));
        return true;
    }
    else
    {
        PRINTOUT(F("Clock already within 5 seconds of time."));
        return false;
    }
}


// This sets static variables for the date/time - this is needed so that all
// data outputs (SD, EnviroDIY, serial printing, etc) print the same time
// for updating the sensors - even though the routines to update the sensors
// and to output the data may take several seconds.
// It is not currently possible to output the instantaneous time an individual
// sensor was updated, just a single marked time.  By custom, this should be
// called before updating the sensors, not after.
void Logger::markTime(void)
{
  Logger::markedEpochTime = getNowEpoch();
}


// This checks to see if the CURRENT time is an even interval of the logging rate
// or we're in the first 15 minutes of logging
bool Logger::checkInterval(void)
{
    bool retval;
    uint32_t checkTime = getNowEpoch();
    MS_DBG(F("Current Unix Timestamp: "), checkTime);
    MS_DBG(F("Logging interval in seconds: "), (_loggingIntervalMinutes*60));
    MS_DBG(F("Mod of Logging Interval: "), checkTime % (_loggingIntervalMinutes*60));

    if (checkTime % (_loggingIntervalMinutes*60) == 0)
    {
        // Update the time variables with the current time
        markTime();
        MS_DBG(F("Time marked at (unix): "), markedEpochTime);
        MS_DBG(F("Time to log!"));
        retval = true;
    }
    else
    {
        MS_DBG(F("Not time yet."));
        retval = false;
    }
    return retval;
}


// This checks to see if the MARKED time is an even interval of the logging rate
// or we're in the first 15 minutes of logging
bool Logger::checkMarkedInterval(void)
{
    bool retval;
    MS_DBG(F("Marked Time: "), markedEpochTime);
    MS_DBG(F("Logging interval in seconds: "), (_loggingIntervalMinutes*60));
    MS_DBG(F("Mod of Logging Interval: "), markedEpochTime % (_loggingIntervalMinutes*60));

    if (markedEpochTime != 0 &&
        (markedEpochTime % (_loggingIntervalMinutes*60) == 0))
    {
        MS_DBG(F("Time to log!"));
        retval = true;
    }
    else
    {
        MS_DBG(F("Not time yet."));
        retval = false;
    }
    return retval;
}


// ============================================================================
//  Public Functions for sleeping the logger
// ============================================================================

// Set up the Interrupt Service Request for waking
// In this case, we're doing nothing, we just want the processor to wake
// This must be a static function (which means it can only call other static funcions.)
void Logger::wakeISR(void)
{
    // MS_DBG(F("Clock interrupt!"));
}

#if defined ARDUINO_ARCH_SAMD

    // Sets up the sleep mode
    void Logger::setupSleep(void)
    {
        // Alarms on the RTC built into the SAMD21 appear to be identical to those
        // in the DS3231.  See more notes below.
        // We're setting the alarm seconds to 59 and then seting it to go off
        // whenever the seconds match the 59.  I'm using 59 instead of 00
        // because there seems to be a bit of a wake-up delay
        zero_sleep_rtc.setAlarmSeconds(59);
        zero_sleep_rtc.enableAlarm(zero_sleep_rtc.MATCH_SS);
    }

    // Puts the system to sleep to conserve battery life.
    // This DOES NOT sleep or wake the sensors!!
    void Logger::systemSleep(void)
    {
        // Wait until the serial ports have finished transmitting
        // This does not clear their buffers, it just waits until they are finished
        // TODO:  Make sure can find all serial ports
        #if defined(STANDARD_SERIAL_OUTPUT)
            STANDARD_SERIAL_OUTPUT.flush();  // for debugging
        #endif
        #if defined(DEBUGGING_SERIAL_OUTPUT)
            DEBUGGING_SERIAL_OUTPUT.flush();  // for debugging
        #endif

        // This clears the interrrupt flag in status register of the clock
        // The next timed interrupt will not be sent until this is cleared
        // rtc.clearINTStatus();

        // Stop any I2C connections
        // This function actually disables the two-wire pin functionality and
        // turns off the internal pull-up resistors.
        Wire.end();

        // USB connection will end at sleep because it's a separate mode in the processor
        USBDevice.detach();  // Disable USB

        // Put the processor into sleep mode.
        zero_sleep_rtc.standbyMode();

        // ---------------------------------------------------------------------
        // -- The portion below this happens on wake up, after any wake ISR's --

        // Reattach the USB after waking
        USBDevice.attach();

        // Re-start any I2C connections
        Wire.begin();
    }

#elif defined __AVR__

    // Sets up the sleep mode
    void Logger::setupSleep(void)
    {
        // Set the pin attached to the RTC alarm to be in the right mode to listen to
        // an interrupt and attach the "Wake" ISR to it.
        pinMode(_mcuWakePin, INPUT_PULLUP);
        enableInterrupt(_mcuWakePin, wakeISR, CHANGE);

        // Unfortunately, because of the way the alarm on the DS3231 is set up, it
        // cannot interrupt on any frequencies other than every second, minute,
        // hour, day, or date.  We could set it to alarm hourly every 5 minutes past
        // the hour, but not every 5 minutes.  This is why we set the alarm for
        // every minute and use the checkInterval function.  This is a hardware
        // limitation of the DS3231; it is not due to the libraries or software.
        rtc.enableInterrupts(EveryMinute);

        // Set the sleep mode
        // In the avr/sleep.h file, the call names of these 5 sleep modes are:
        // SLEEP_MODE_IDLE         -the least power savings
        // SLEEP_MODE_ADC
        // SLEEP_MODE_PWR_SAVE
        // SLEEP_MODE_STANDBY
        // SLEEP_MODE_PWR_DOWN     -the most power savings
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    }

    // Puts the system to sleep to conserve battery life.
    // This DOES NOT sleep or wake the sensors!!
    void Logger::systemSleep(void)
    {
        // Wait until the serial ports have finished transmitting
        // This does not clear their buffers, it just waits until they are finished
        // TODO:  Make sure can find all serial ports
        #if defined(STANDARD_SERIAL_OUTPUT)
            STANDARD_SERIAL_OUTPUT.flush();  // for debugging
        #endif
        #if defined(DEBUGGING_SERIAL_OUTPUT)
            DEBUGGING_SERIAL_OUTPUT.flush();  // for debugging
        #endif

        // Make sure the RTC is still sending out interrupts
        rtc.enableInterrupts(EveryMinute);

        // Clear the last interrupt flag in the RTC status register
        // The next timed interrupt will not be sent until this is cleared
        rtc.clearINTStatus();

        // Make sure we're still set up to handle the clock interrupt
        pinMode(_mcuWakePin, INPUT_PULLUP);
        enableInterrupt(_mcuWakePin, wakeISR, CHANGE);

        // Stop any I2C connections
        // This function actually disables the two-wire pin functionality and
        // turns off the internal pull-up resistors.
        // It does NOT set the pin mode!
        Wire.end();
        // Now force the I2C pins to LOW
        // I2C devices have a nasty habit of stealing power from the SCL and SDA pins...
        // This will only work for the "main" I2C/TWI interface
        pinMode(SDA, OUTPUT);  // set output mode
        pinMode(SCL, OUTPUT);
        digitalWrite(SDA, LOW);  // Set the pins low
        digitalWrite(SCL, LOW);

        // Temporarily disables interrupts, so no mistakes are made when writing
        // to the processor registers
        noInterrupts();

        // Disable the processor ADC (must be disabled before it will power down)
        // ADCSRA = ADC Control and Status Register A
        // ADEN = ADC Enable
        ADCSRA &= ~_BV(ADEN);

        // turn off the brown-out detector, if possible
        // BODS = brown-out detector sleep
        // BODSE = brown-out detector sleep enable
        #if defined(BODS) && defined(BODSE)
            sleep_bod_disable();
        #endif

        // disable all power-reduction modules (ie, the processor module clocks)
        // NOTE:  This only shuts down the various clocks on the processor via
        // the power reduction register!  It does NOT actually disable the
        // modules themselves or set the pins to any particular state!  This
        // means that the I2C/Serial/Timer/etc pins will still be active and
        // powered unless they are turned off prior to calling this function.
        power_all_disable();

        // Set the sleep enable bit.
        sleep_enable();

        // Re-enables interrupts so we can wake up again
        interrupts();

        // Actually put the processor into sleep mode.
        // This must happen after the SE bit is set.
        sleep_cpu();

        // ---------------------------------------------------------------------
        // -- The portion below this happens on wake up, after any wake ISR's --

        // Temporarily disables interrupts, so no mistakes are made when writing
        // to the processor registers
        noInterrupts();

        // Re-enable all power modules (ie, the processor module clocks)
        // NOTE:  This only re-enables the various clocks on the processor!
        // The modules may need to be re-initialized after the clocks re-start.
        power_all_enable();

        // Clear the SE (sleep enable) bit.
        sleep_disable();

        // Re-enable the processor ADC
        ADCSRA |= _BV(ADEN);

        // Re-enables interrupts
        interrupts();

        // Re-start the I2C interface
        pinMode(SDA, INPUT_PULLUP);  // set as input with the pull-up on
        pinMode(SCL, INPUT_PULLUP);
        Wire.begin();

        // The logger will now start the next function after the systemSleep
        // function in either the loop or setup
    }
#endif

// ===================================================================== //
// Public functions for logging data to an SD card
// ===================================================================== //

// This sets a file name, if you want to decide on it in advance
void Logger::setFileName(String& fileName)
{
    _fileName = fileName;
    _autoFileName = false;
}
// Same as above, with a character array (overload function)
void Logger::setFileName(const char *fileName)
{
    String StrName = String(fileName);
    setFileName(StrName);
}


// This generates a file name from the logger id and the current date
// This will be used if the setFileName function is not called before
// the begin() function is called.
void Logger::generateAutoFileName(void)
{
    if (_autoFileName)
    {
        // Generate the file name from logger ID and date
        String fileName =  String(_loggerID);
        fileName +=  "_";
        fileName +=  formatDateTime_ISO8601(getNowEpoch()).substring(0, 10);
        fileName +=  ".csv";
        setFileName(fileName);
        _fileName = fileName;
    }
}


// This is a PRE-PROCESSOR MACRO to speed up generating header rows
// Again, THIS IS NOT A FUNCTION, it is a pre-processor macro
#define STREAM_CSV_ROW(firstCol, function) \
    stream->print("\""); \
    stream->print(firstCol); \
    stream->print("\","); \
    for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++) \
    { \
        stream->print("\""); \
        stream->print(function); \
        stream->print("\""); \
        if (i + 1 != _internalArray->getVariableCount()) \
        { \
            stream->print(","); \
        } \
    } \
    stream->println();

// This sends a file header out over an Arduino stream
void Logger::printFileHeader(Stream *stream)
{
    // Very first line of the header is the logger ID
    stream->print(F("Data Logger: "));
    stream->println(_loggerID);

    // Next we're going to print the current file name
    stream->print(F("Data Logger File: "));
    stream->println(_fileName);

    // Next line will be the parent sensor names
    STREAM_CSV_ROW(F("Sensor Name:"), _internalArray->arrayOfVars[i]->getParentSensorName())
    // Next comes the ODM2 variable name
    STREAM_CSV_ROW(F("Variable Name:"), _internalArray->arrayOfVars[i]->getVarName())
    // Next comes the ODM2 unit name
    STREAM_CSV_ROW(F("Result Unit:"), _internalArray->arrayOfVars[i]->getVarUnit())
    // Next comes the variable UUIDs
    STREAM_CSV_ROW(F("Result UUID:"), _internalArray->arrayOfVars[i]->getVarUUID())

    // We'll finish up the the custom variable codes
    String dtRowHeader = F("Date and Time in UTC");
    if (_timeZone > 0) dtRowHeader += '+' + _timeZone;
    else if (_timeZone < 0) dtRowHeader += _timeZone;
    STREAM_CSV_ROW(dtRowHeader, _internalArray->arrayOfVars[i]->getVarCode());
}


// This prints a comma separated list of volues of sensor data - including the
// time -  out over an Arduino stream
void Logger::printSensorDataCSV(Stream *stream)
{
    String csvString = "";
    dtFromEpoch(markedEpochTime).addToString(csvString);
    csvString += F(",");
    stream->print(csvString);
    for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++)
    {
        stream->print(_internalArray->arrayOfVars[i]->getValueString());
        if (i + 1 != _internalArray->getVariableCount())
        {
            stream->print(F(","));
        }
    }
    stream->println();
}

// Protected helper function - This checks if the SD card is available and ready
bool Logger::initializeSDCard(void)
{
    // Initialise the SD card
    if (!sd.begin(_SDCardPin, SPI_FULL_SPEED))
    {
        PRINTOUT(F("Error: SD card failed to initialize or is missing."));
        PRINTOUT(F("Data will not be saved!"));
        return false;
    }
    else  // skip everything else if there's no SD card, otherwise it might hang
    {
        PRINTOUT(F("Successfully connected to SD Card with card/slave select on pin "),
                 _SDCardPin);
        return true;
    }
}


// Protected helper function - This sets a timestamp on a file
void Logger::setFileTimestamp(File fileToStamp, uint8_t stampFlag)
{
    fileToStamp.timestamp(stampFlag, dtFromEpoch(getNowEpoch()).year(),
                                     dtFromEpoch(getNowEpoch()).month(),
                                     dtFromEpoch(getNowEpoch()).date(),
                                     dtFromEpoch(getNowEpoch()).hour(),
                                     dtFromEpoch(getNowEpoch()).minute(),
                                     dtFromEpoch(getNowEpoch()).second());
}


// Protected helper function - This opens or creates a file, converting a string
// file name to a character file name
bool Logger::openFile(String& filename, bool createFile, bool writeDefaultHeader)
{
    // Initialise the SD card
    // skip everything else if there's no SD card, otherwise it might hang
    if (!initializeSDCard()) return false;

    // Convert the string filename to a character file name for SdFat
    uint8_t fileNameLength = filename.length() + 1;
    char charFileName[fileNameLength];
    filename.toCharArray(charFileName, fileNameLength);

    // First attempt to open an already existing file (in write mode), so we
    // don't try to re-create something that's already there.
    // This should also prevent the header from being written over and over
    // in the file.
    if (logFile.open(charFileName, O_WRITE | O_AT_END))
    {
        MS_DBG(F("Opened existing file: "), filename);
        // Set access date time
        setFileTimestamp(logFile, T_ACCESS);
        return true;
    }
    else if (createFile)
    {
        // Create and then open the file in write mode
        if (logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END))
        {
            MS_DBG(F("Created new file: "), filename);
            // Set creation date time
            setFileTimestamp(logFile, T_CREATE);
            // Write out a header, if requested
            if (writeDefaultHeader)
            {
                // Add header information
                printFileHeader(&logFile);
                // Print out the header for debugging
                #if defined(DEBUGGING_SERIAL_OUTPUT)
                    MS_DBG(F("\n \\/---- File Header ----\\/ "));
                    printFileHeader(&DEBUGGING_SERIAL_OUTPUT);
                    MS_DBG('\n');
                #endif
                // Set write/modification date time
                setFileTimestamp(logFile, T_WRITE);
            }
            // Set access date time
            setFileTimestamp(logFile, T_ACCESS);
            return true;
        }
        // Return false if we couldn't create the file
        else
        {
            MS_DBG(F("Unable to create new file: "), filename);
            return false;
        }
    }
    // Return false if we couldn't access the file (and were not told to create it)
    else
    {
        MS_DBG(F("Unable to to write to file: "), filename);
        return false;
    }
}


// These functions create a file on the SD card with the given filename and
// set the proper timestamps to the file.
// The filename may either be the one set by setFileName(String)/setFileName(void)
// or can be specified in the function.
// If specified, it will also write a header to the file based on
// the sensors in the group.
// This can be used to force a logger to create a file with a secondary file name.
bool Logger::createLogFile(String& filename, bool writeDefaultHeader)
{
    // Attempt to create and open a file
    if (openFile(filename, true, writeDefaultHeader))
    {
        // Close the file to save it (only do this if we'd opened it)
        logFile.close();
        return true;
    }
    else return false;
}
bool Logger::createLogFile(bool writeDefaultHeader)
{
    if (_autoFileName) generateAutoFileName();
    return createLogFile(_fileName, writeDefaultHeader);
}


// These functions write a file on the SD card with the given filename and
// set the proper timestamps to the file.
// The filename may either be the one set by setFileName(String)/setFileName(void)
// or can be specified in the function.
// If the file does not already exist, the file will be created.
// This can be used to force a logger to write to a file with a secondary file name.
bool Logger::logToSD(String& filename, String& rec)
{
    // First attempt to open the file without creating a new one
    if (!openFile(filename, false, false))
    {
        // Next try to create the file, bail if we couldn't create it
        // This will not attempt to generate a new file name or add a header!
        if (!openFile(filename, true, false))
        {
            PRINTOUT(F("Unable to write to SD card!"));
            return false;
        }
    }

    // If we could successfully open or create the file, write the data to it
    logFile.println(rec);
    // Echo the line to the serial port
    PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/ "));
    PRINTOUT(rec);

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
    // Close the file to save it
    logFile.close();
    return true;
}
bool Logger::logToSD(String& rec)
{
    // Get a new file name if the name is blank
    if (_fileName == "") generateAutoFileName();
    return logToSD(_fileName, rec);
}
// NOTE:  This is structured differently than the version with a string input
// record.  This is to avoid the creation/passing of very long strings.
bool Logger::logToSD(void)
{
    // Get a new file name if the name is blank
    if (_fileName == "") generateAutoFileName();

    // First attempt to open the file without creating a new one
    if (!openFile(_fileName, false, false))
    {
        // Next try to create a new file, bail if we couldn't create it
        // Generate a filename with the current date, if the file name isn't set
        if (_autoFileName) generateAutoFileName();
        // Do add a default header to the new file!
        if (!openFile(_fileName, true, true))
        {
            PRINTOUT(F("Unable to write to SD card!"));
            return false;
        }

    }

    // Write the data
    printSensorDataCSV(&logFile);
    // Echo the line to the serial port
    #if defined(STANDARD_SERIAL_OUTPUT)
        PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/ "));
        printSensorDataCSV(&STANDARD_SERIAL_OUTPUT);
        PRINTOUT('\n');
    #endif

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
    // Close the file to save it
    logFile.close();
    return true;
}


// ===================================================================== //
// Public functions for a "sensor testing" mode
// ===================================================================== //

// This checks to see if you want to enter the sensor mode
// This should be run as the very last step within the setup function
/***
void Logger::checkForTestingMode(int8_t buttonPin)
{
    // Set the pin attached to some button to enter debug mode
    if (buttonPin >= 0) pinMode(buttonPin, INPUT_PULLUP);

    // Flash the LED to let user know it is now possible to enter debug mode
    for (uint8_t i = 0; i < 15; i++)
    {
        digitalWrite(_ledPin, HIGH);
        delay(50);
        digitalWrite(_ledPin, LOW);
        delay(50);
    }

    // Look for up to 5 seconds for a button press
    PRINTOUT(F("Push button NOW to enter sensor testing mode."));
    for (uint32_t start = millis(); millis() - start < 5000; )
    {
        if (digitalRead(buttonPin) == HIGH) testingMode();
    }
    PRINTOUT(F("------------------------------------------\n"));
    PRINTOUT(F("End of sensor testing mode."));
}
***/


// A static function if you'd prefer to enter testing based on an interrupt
void Logger::testingISR()
{
    // MS_DBG(F("Testing interrupt!"));
    if (!Logger::isTestingNow && !Logger::isLoggingNow)
    {
        Logger::startTesting = true;
        // MS_DBG(F("Testing flag has been set."));
    }
}


// This defines what to do in the testing mode
void Logger::testingMode()
{
    // Flag to notify that we're in testing mode
    Logger::isTestingNow = true;
    // Unset the startTesting flag
    Logger::startTesting = false;

    PRINTOUT(F("------------------------------------------"));
    PRINTOUT(F("Entering sensor testing mode"));
    delay(100);  // This seems to prevent crashes, no clue why ....

    // Power up all of the sensors
    _internalArray->sensorsPowerUp();

    // Wake up all of the sensors
    _internalArray->sensorsWake();

    // Update the sensors and print out data 25 times
    for (uint8_t i = 0; i < 25; i++)
    {
        PRINTOUT(F("------------------------------------------"));
        // Update the values from all attached sensors
        // NOTE:  NOT using complete update because we want everything left
        // on between iterations in testing mode.
        _internalArray->updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is "), formatDateTime_ISO8601(getNowEpoch()));
        PRINTOUT(F("    -----------------------"));
        // Print out the sensor data
        #if defined(STANDARD_SERIAL_OUTPUT)
            _internalArray->printSensorData(&STANDARD_SERIAL_OUTPUT);
        #endif
        PRINTOUT(F("    -----------------------"));
        delay(5000);
    }

    // Put sensors to sleep
    _internalArray->sensorsSleep();
    _internalArray->sensorsPowerDown();

    PRINTOUT(F("Exiting testing mode"));
    PRINTOUT(F("------------------------------------------"));

    // Unset testing mode flag
    Logger::isTestingNow = false;

    // Sleep
    if(_mcuWakePin >= 0){systemSleep();}
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This does all of the setup that can't happen in the constructors
// That is, things that require the actual processor/MCU to do something
// rather than the compiler to do something.
 void Logger::begin(bool skipSensorSetup)
{
    // Set up pins for the LED and button
    if (_ledPin >= 0)
    {
        pinMode(_ledPin, OUTPUT);
        digitalWrite(_ledPin, LOW);
    }
    if (_buttonPin >= 0) pinMode(_buttonPin, INPUT_PULLUP);

    #if defined ARDUINO_ARCH_SAMD
        zero_sleep_rtc.begin();
    #else
        // Set the pins for I2C
        pinMode(SDA, INPUT_PULLUP);
        pinMode(SCL, INPUT_PULLUP);
        Wire.begin();
        rtc.begin();
        delay(100);
    #endif

    // Print out the current time
    PRINTOUT(F("Current RTC time is: "), formatDateTime_ISO8601(getNowEpoch()));

    PRINTOUT(F("Setting up logger "), _loggerID, F(" to record at "),
             _loggingIntervalMinutes, F(" minute intervals."));

    PRINTOUT(F("This logger has a variable array with "),
             _internalArray->getVariableCount(), F(" variables, of which "),
             _internalArray->getVariableCount() - _internalArray->getCalculatedVariableCount(),
             F(" come from "),_internalArray->getSensorCount(), F(" sensors and "),
             _internalArray->getCalculatedVariableCount(), F(" are calculated."));

    if (!skipSensorSetup)
    {
         // Set up the sensors
         PRINTOUT(F("Setting up sensors..."));
         _internalArray->setupSensors();

        // Create the log file, adding the default header to it
        if (_autoFileName) generateAutoFileName();
        if (createLogFile(true)) PRINTOUT(F("Data will be saved as "), _fileName);
        else PRINTOUT(F("Unable to create a file to save data to!"));

        // Mark sensors as having been setup
        _areSensorsSetup = 1;
    }

    // Setup sleep mode
    if(_mcuWakePin >= 0){setupSleep();}

    // Set up the interrupt to be able to enter sensor testing mode
    // NOTE:  Entering testing mode before the sensors have been set-up may
    // give unexpected results.
    if (_buttonPin >= 0)
    {
        enableInterrupt(_buttonPin, Logger::testingISR, CHANGE);
        PRINTOUT(F("Push button on pin "), _buttonPin,
                 F(" at any time to enter sensor testing mode."));
    }

    // Make sure all sensors are powered down at the end
    // The should be, but just in case
    _internalArray->sensorsPowerDown();

    PRINTOUT(F("Logger setup finished!"));
    PRINTOUT(F("------------------------------------------\n"));

    // Sleep
    if(_mcuWakePin >= 0){systemSleep();}
}


// This is a one-and-done to log data
void Logger::logData(void)
{
    // If the number of intervals is negative, then the sensors and file on
    // the SD card haven't been setup and we want to set them up.
    // NOTE:  Unless it completed in less than one second, the sensor set-up
    // will take the place of logging for this interval!
    if (!_areSensorsSetup)
    {
        // Set up the sensors
        PRINTOUT(F("Sensors and data file had not been set up!  Setting them up now."));
        _internalArray->setupSensors();

       // Create the log file, adding the default header to it
       if (_autoFileName) generateAutoFileName();
       if (createLogFile(true)) PRINTOUT(F("Data will be saved as "), _fileName);
       else PRINTOUT(F("Unable to create a file to save data to!"));

       // Mark sensors as having been setup
       _areSensorsSetup = 1;
    }

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        if (_ledPin >= 0) digitalWrite(_ledPin, HIGH);

        // Do a complete sensor update
        MS_DBG(F("    Running a complete sensor update..."));
        _internalArray->completeUpdate();

        // Create a csv data record and save it to the log file
        logToSD();

        // Turn off the LED
        if (_ledPin >= 0) digitalWrite(_ledPin, LOW);
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Sleep
    if(_mcuWakePin >= 0){systemSleep();}
}

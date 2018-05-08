/*
 *LoggerBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

#include "LoggerBase.h"  // To communicate with the internet

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts


// Initialize the static timezone
int8_t Logger::_timeZone = 0;
// Initialize the static time adjustment
int8_t Logger::_offset = 0;
// Initialize the static timestamps
uint32_t Logger::markedEpochTime = 0;
DateTime Logger::markedDateTime = 0;
char Logger::markedISO8601Time[26];
// Initialize the testing/logging flags
bool Logger::_isLoggingNow = false;
bool Logger::_isTestingNow = false;
bool Logger::_startTesting = false;

// Initialize the RTC for the SAMD boards
#if defined(ARDUINO_ARCH_SAMD)
    RTCZero Logger::zero_sleep_rtc;
#endif


// Initialization - cannot do this in constructor arduino has issues creating
// instances of classes with non-empty constructors
void Logger::init(int8_t SDCardPin, int8_t mcuWakePin,
          uint8_t variableCount,
          Variable *variableList[],
          uint8_t loggingIntervalMinutes,
          const char *loggerID)
{
    // initialize the variable array
    VariableArray::init(variableCount, variableList);

    PRINTOUT(F("Initializing logger "), loggerID, F(" to record at "),
             loggingIntervalMinutes, F(" minute intervals ... "));

    _SDCardPin = SDCardPin;
    _mcuWakePin = mcuWakePin;
    _loggingIntervalMinutes = loggingIntervalMinutes;
    _loggingIntervalSeconds = round(_loggingIntervalMinutes*60);  // convert to even seconds
    _loggerID = loggerID;
    _autoFileName = false;
    _isFileNameSet = false;
    _numTimepointsLogged = 0;

    // Set sleep variable, if an interrupt pin is given
    if(_mcuWakePin != -1)
    {
        _sleep = true;
    }

    PRINTOUT(F("   ... Success!\n"));
};


// Sets the static timezone - this must be set
void Logger::setTimeZone(int8_t timeZone)
{
    _timeZone = timeZone;
    // Some helpful prints for debugging
    PRINTOUT(F("Logger timezone is "));
    if (_timeZone == 0) PRINTOUT(F("UTC\n"));
    else if (_timeZone > 0) PRINTOUT(F("UTC+"));
    else PRINTOUT(F("UTC"));
    if (_timeZone != 0) PRINTOUT(_timeZone, F("\n"));

}


// This set the offset between the built-in clock and the time zone where
// the data is being recorded.  If your RTC is set in UTC and your logging
// timezone is EST, this should be -5.  If your RTC is set in EST and your
// timezone is EST this does not need to be called.
void Logger::setTZOffset(int8_t offset)
{
    _offset = offset;
    // Some helpful prints for debugging
    PRINTOUT(F("RTC timezone is "));
    if ((_timeZone - _offset) == 0)
        PRINTOUT(F("UTC\n"));
    else if ((_timeZone - _offset) > 0)
        PRINTOUT(F("UTC+"));
    else PRINTOUT(F("UTC"));
    if ((_timeZone - _offset) != 0)
        PRINTOUT(_timeZone - _offset, F("\n"));
}


// Sets up a pin for an LED or other way of alerting that data is being logged
void Logger::setAlertPin(int8_t ledPin)
{
    _ledPin = ledPin;
    MS_DBG(F("Pin "), _ledPin, F(" set as LED alert pin\n"));
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
String Logger::formatDateTime_ISO8601(DateTime dt)
{
    // Set up an inital string
    String dateTimeStr;
    // Convert the DateTime object to a String
    dt.addToString(dateTimeStr);
    dateTimeStr.replace(F(" "), F("T"));
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
        PRINTOUT(F("Bad timestamp, skipping sync.\n"));
        return false;
    }

    uint32_t nist_logTZ = nist + getTimeZone()*3600;
    uint32_t nist_rtcTZ = nist_logTZ - getTZOffset()*3600;
    MS_DBG(F("         Correct Time for Logger: "), nist_logTZ, F(" -> "), \
        formatDateTime_ISO8601(nist_logTZ), F("\n"));

    // See how long it took to get the time from NIST
    int sync_time = (millis() - start_millis)/1000;

    // Check the current RTC time
    uint32_t cur_logTZ = getNowEpoch();
    MS_DBG(F("            Time Returned by RTC: "), cur_logTZ, F(" -> "), \
        formatDateTime_ISO8601(cur_logTZ), F("\n"));
    MS_DBG(F("Offset: "), abs(nist_logTZ - cur_logTZ), F("\n"));

    // If the RTC and NIST disagree by more than 5 seconds, set the clock
    if ((abs(nist_logTZ - cur_logTZ) > 5) && (nist != 0))
    {
        setNowEpoch(nist_rtcTZ + sync_time/2);
        PRINTOUT(F("Clock synced!\n"));
        return true;
    }
    else
    {
        PRINTOUT(F("Clock already within 5 seconds of time.\n"));
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
  Logger::markedDateTime = dtFromEpoch(markedEpochTime);
  formatDateTime_ISO8601(markedDateTime).toCharArray(markedISO8601Time, 26);
}


// This checks to see if the CURRENT time is an even interval of the logging rate
// or we're in the first 15 minutes of logging
bool Logger::checkInterval(void)
{
    bool retval;
    uint32_t checkTime = getNowEpoch();
    MS_DBG(F("Current Unix Timestamp: "), checkTime, F("\n"));
    MS_DBG(F("Logging interval in seconds: "), _loggingIntervalSeconds, F("\n"));
    MS_DBG(F("Mod of Logging Interval: "), checkTime % _loggingIntervalSeconds, F("\n"));
    MS_DBG(F("Number of Readings so far: "), _numTimepointsLogged, F("\n"));
    MS_DBG(F("Mod of 120: "), checkTime % 120, F("\n"));
    if ((checkTime % _loggingIntervalSeconds == 0 ) or
        (_numTimepointsLogged < 10 and checkTime % 120 == 0))
    {
        // Update the time variables with the current time
        markTime();
        MS_DBG(F("Time marked at (unix): "), markedEpochTime, F("\n"));
        MS_DBG(F("    year: "), markedDateTime.year(), F("\n"));
        MS_DBG(F("    month: "), markedDateTime.month(), F("\n"));
        MS_DBG(F("    date: "), markedDateTime.date(), F("\n"));
        MS_DBG(F("    hour: "), markedDateTime.hour(), F("\n"));
        MS_DBG(F("    minute: "), markedDateTime.minute(), F("\n"));
        MS_DBG(F("    second: "), markedDateTime.second(), F("\n"));
        MS_DBG(F("Time marked at [char]: "), markedISO8601Time, F("\n"));
        // Update the number of readings taken
        _numTimepointsLogged ++;
        MS_DBG(F("Time to log!\n"));
        retval = true;
    }
    else
    {
        MS_DBG(F("Not time yet.\n"));
        retval = false;
    }
    return retval;
}


// This checks to see if the MARKED time is an even interval of the logging rate
// or we're in the first 15 minutes of logging
bool Logger::checkMarkedInterval(void)
{
    bool retval;
    MS_DBG(F("Marked Time: "), markedEpochTime, F("\n"));
    MS_DBG(F("Logging interval in seconds: "), _loggingIntervalSeconds, F("\n"));
    MS_DBG(F("Mod of Logging Interval: "), markedEpochTime % _loggingIntervalSeconds, F("\n"));
    MS_DBG(F("Number of Readings so far: "), _numTimepointsLogged, F("\n"));
    MS_DBG(F("Mod of 120: "), markedEpochTime % 120, F("\n"));
    if (markedEpochTime != 0 &&
        ((markedEpochTime % _loggingIntervalSeconds == 0 ) or
        (_numTimepointsLogged < 10 and markedEpochTime % 120 == 0)))
    {
        // Update the number of readings taken
        _numTimepointsLogged ++;
        MS_DBG(F("Time to log!\n"));
        retval = true;
    }
    else
    {
        MS_DBG(F("Not time yet, back to sleep\n"));
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
void Logger::wakeISR(void){MS_DBG(F("Clock interrupt!\n"));}

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

        // USB connection will end at sleep because it's a separate mode in the processor
        USBDevice.detach();  // Disable USB

        // Put the processor into sleep mode.
        zero_sleep_rtc.standbyMode();

        // Reattach the USB after waking
        USBDevice.attach();
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
        // every minute and still need the timer function.  This is a hardware
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

        // This clears the interrrupt flag in status register of the clock
        // The next timed interrupt will not be sent until this is cleared
        rtc.clearINTStatus();

        // Temporarily disables interrupts, so no mistakes are made when writing
        // to the processor registers
        noInterrupts();
        // Disable the processor ADC
        ADCSRA &= ~_BV(ADEN);
        // turn off the brown-out detector, if possible
        #if defined(BODS) && defined(BODSE)
            sleep_bod_disable();
        #endif
        // Set the sleep enable bit.
        sleep_enable();
        // Re-enables interrupts so we can wake up again
        interrupts();

        // Actually put the processor into sleep mode.
        // This must happen after the SE bit is set.
        sleep_cpu();

        // This portion happens on the wake up..
        // Clear the SE (sleep enable) bit.
        sleep_disable();
        // Re-enable the processor ADC
        ADCSRA |= _BV(ADEN);
    }
#endif

// ===================================================================== //
// Public functions for logging data to an SD card
// ===================================================================== //

// This sets a file name, if you want to decide on it in advance
void Logger::setFileName(char *fileName)
{
    // Save the filename to the static String
    _fileName = fileName;
    _isFileNameSet = true;

    // Print out the file name
    PRINTOUT(F("Data will be saved as "), _fileName, '\n');
    if (!_autoFileName) PRINTOUT(F("\n"));
}
// Same as above, with a string (overload function)
void Logger::setFileName(String fileName)
{
    // Convert the string filename to a character file name
    uint8_t fileNameLength = fileName.length() + 1;
    char charFileName[fileNameLength];
    fileName.toCharArray(charFileName, fileNameLength);
    setFileName(charFileName);
}


// This generates a file name from the logger id and the current date
// This will be used if the setFileName function is not called before
// the begin() function is called.
void Logger::setFileName(void)
{
    _autoFileName = true;
    // Generate the file name from logger ID and date
    String fileName = "";
    if (_loggerID)
    {
        fileName +=  String(_loggerID);
        fileName +=  F("_");
    }
    fileName +=  formatDateTime_ISO8601(getNowEpoch()).substring(0, 10);
    fileName +=  F(".csv");
    setFileName(fileName);
}


// This creates a header for the logger file
String Logger::generateFileHeader(void)
{
    // Very first column of the header is the logger ID
    String logIDRowHeader = F("Data Logger: ");
    logIDRowHeader += String(_loggerID);

    // Create the header rows
    String dataHeader = "";
    // Next line will be the parent sensor names
    makeHeaderRowMacro(logIDRowHeader, _variableList[i]->parentSensor->getSensorName())
    // Next comes the ODM2 variable name
    makeHeaderRowMacro(logIDRowHeader, _variableList[i]->getVarName())
    // Next comes the ODM2 unit name
    makeHeaderRowMacro(logIDRowHeader, _variableList[i]->getVarUnit())

    // We'll finish up the the custom variable codes
    String dtRowHeader = F("Date and Time in UTC");
    dtRowHeader += _timeZone;
    makeHeaderRowMacro(dtRowHeader, _variableList[i]->getVarCode())

    // Return everything
    return dataHeader;
}


// This generates a comma separated list of volues of sensor data - including the time
String Logger::generateSensorDataCSV(void)
{
    String csvString = "";
    markedDateTime.addToString(csvString);
    csvString += F(",");
    csvString += VariableArray::generateSensorDataCSV();
    return csvString;
}


// This checks if the SD card is available and ready
bool Logger::initializeSDCard(uint8_t Pin)
{
    // Initialise the SD card
    if (!sd.begin(Pin, SPI_FULL_SPEED))
    {
        PRINTOUT(F("Error: SD card failed to initialize or is missing.\n"));
        PRINTOUT(F("Data will not be saved!\n"));
        return false;
    }
    else  // skip everything else if there's no SD card, otherwise it might hang
    {
        PRINTOUT(F("Successfully connected to SD Card with card/slave select on pin "));
        PRINTOUT(Pin, F("\n"));
        return true;
    }
}
bool Logger::initializeSDCard(void)
{
    return initializeSDCard(_SDCardPin);
}


// This sets a timestamp on a file
void Logger::setFileTimestame(SdFile fileToStamp, uint8_t stampFlag)
{
    fileToStamp.timestamp(stampFlag, dtFromEpoch(getNowEpoch()).year(),
                                dtFromEpoch(getNowEpoch()).month(),
                                dtFromEpoch(getNowEpoch()).date(),
                                dtFromEpoch(getNowEpoch()).hour(),
                                dtFromEpoch(getNowEpoch()).minute(),
                                dtFromEpoch(getNowEpoch()).second());
}


// This initializes a file on the SD card with the given filename and writes the given header to it
void Logger::setupLogFile(String filename, String header)
{
    // Initialise the SD card
    // skip everything else if there's no SD card, otherwise it might hang
    if (!initializeSDCard()) return;
    else
    {
        // Convert the string filename to a character file name for SdFat
        uint8_t fileNameLength = filename.length() + 1;
        char charFileName[fileNameLength];
        filename.toCharArray(charFileName, fileNameLength);

        // Open the file in write mode (and create it if it did not exist)
        logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END);
        // Set creation date time
        setFileTimestame(logFile, T_CREATE);
        // Set write/modification date time
        setFileTimestame(logFile, T_WRITE);
        // Set access date time
        setFileTimestame(logFile, T_ACCESS);
        PRINTOUT(F("File created!\n"));

        // Add header information
        logFile.print(header);
        MS_DBG(header, F("\n"));

        //Close the file to save it
        logFile.close();
    }
}
// This initializes a file on the SD card and writes a header to it
void Logger::setupLogFile(void)
{
    setupLogFile(_fileName, generateFileHeader());
}


// This writes a record to the SD card with the given filename
void Logger::logToSD(String rec, String filename)
{
    // Initialise the SD card
    // skip everything else if there's no SD card, otherwise it might hang
    if (!initializeSDCard()) return;
    else  // skip everything else if there's no SD card, otherwise it might hang
    {
        // Convert the string filename to a character file name for SdFat
        uint8_t fileNameLength = filename.length() + 1;
        char charFileName[fileNameLength];
        filename.toCharArray(charFileName, fileNameLength);

        // Check that the file exists, just in case someone yanked the SD card
        if (!logFile.open(charFileName, O_WRITE | O_AT_END))
        {
            PRINTOUT(F("SD Card File Lost!  Starting new file.\n"));
            setupLogFile(filename, "");
        }

        // Write the CSV data
        logFile.println(rec);
        // Echo the line to the serial port
        PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/ \n"));
        PRINTOUT(rec, F("\n"));

        // Set write/modification date time
        setFileTimestame(logFile, T_WRITE);
        // Set access date time
        setFileTimestame(logFile, T_ACCESS);

        // Close the file to save it
        logFile.close();
    }
}
// This writes a record to the SD card, using the logger's filename
void Logger::logToSD(String rec)
{
    logToSD(rec, _fileName);
}


// ===================================================================== //
// Public functions for a "sensor testing" mode
// ===================================================================== //

// This checks to see if you want to enter the sensor mode
// This should be run as the very last step within the setup function
void Logger::checkForTestingMode(int8_t buttonPin)
{
    // Set the pin attached to some button to enter debug mode
    if (buttonPin > 0) pinMode(buttonPin, INPUT_PULLUP);

    // Flash the LED to let user know it is now possible to enter debug mode
    for (uint8_t i = 0; i < 15; i++)
    {
        digitalWrite(_ledPin, HIGH);
        delay(50);
        digitalWrite(_ledPin, LOW);
        delay(50);
    }

    // Look for up to 5 seconds for a button press
    PRINTOUT(F("Push button NOW to enter sensor testing mode.\n"));
    for (uint32_t start = millis(); millis() - start < 5000; )
    {
        if (digitalRead(buttonPin) == HIGH) testingMode();
    }
    PRINTOUT(F("------------------------------------------\n\n"));
    PRINTOUT(F("End of sensor testing mode.\n"));
}

// A static function if you'd prefer to enter testing based on an interrupt
void Logger::testingISR()
{
    MS_DBG(F("Testing interrupt!\n"));
    if (!Logger::_isTestingNow && !Logger::_isLoggingNow)
    {
        Logger::_startTesting = true;
        MS_DBG(F("Testing flag has been set.\n"));
    }
}


    // This defines what to do in the testing mode
void Logger::testingMode()
{
    // Flag to notify that we're in testing mode
    Logger::_isTestingNow = true;
    // Unset the _startTesting flag
    Logger::_startTesting = false;

    PRINTOUT(F("------------------------------------------\n"));
    PRINTOUT(F("Entering sensor testing mode\n"));
    delay(100);  // This seems to prevent crashes, no clue why ....

    // Power up all of the sensors
    sensorsPowerUp();

    // Wake up all of the sensors
    sensorsWake();

    // Update the sensors and print out data 25 times
    for (uint8_t i = 0; i < 25; i++)
    {
        PRINTOUT(F("------------------------------------------\n"));
        // Update the values from all attached sensors
        updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is "));
        PRINTOUT(formatDateTime_ISO8601(getNowEpoch()), F("\n"));
        PRINTOUT(F("    -----------------------\n"));
        // Print out the sensor data
        #if defined(STANDARD_SERIAL_OUTPUT)
            printSensorData(&STANDARD_SERIAL_OUTPUT);
        #endif
        PRINTOUT(F("    -----------------------\n"));
        delay(5000);
    }

    // Put sensors to sleep
    sensorsSleep();
    sensorsPowerDown();

    // Unset testing mode flag
    Logger::_isTestingNow = false;
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This calls all of the setup functions - must be run AFTER init
 void Logger::begin(void)
{
    // Set up pins for the LED's
    if (_ledPin > 0) pinMode(_ledPin, OUTPUT);

    #if defined ARDUINO_ARCH_SAMD
        zero_sleep_rtc.begin();
    #else
        rtc.begin();
        delay(100);
    #endif

    // Print out the current time
    PRINTOUT(F("Current RTC time is: "));
    PRINTOUT(formatDateTime_ISO8601(getNowEpoch()), F("\n"));

    // Set up the sensors
    setupSensors();

    // Set the filename for the logger to save to, if it hasn't been done
    if(!_isFileNameSet){setFileName();}
    else if(_autoFileName){setFileName();}
    else setFileName(_fileName);  // This just for a nice print-out

    // Set up the log file
    setupLogFile();

    // Setup sleep mode
    if(_sleep){setupSleep();}

    PRINTOUT(F("Logger setup finished!\n"));
    PRINTOUT(F("------------------------------------------\n\n"));
}


// This is a one-and-done to log data
void Logger::log(void)
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::_isLoggingNow = true;

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------\n"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(_ledPin, HIGH);

        // Send power to all of the sensors
        MS_DBG(F("    Powering sensors...\n"));
        sensorsPowerUp();
        // Wake up all of the sensors
        MS_DBG(F("    Waking sensors...\n"));
        sensorsWake();
        // Update the values from all attached sensors
        MS_DBG(F("  Updating sensor values...\n"));
        updateAllSensors();
        // Put sensors to sleep
        MS_DBG(F("  Putting sensors back to sleep...\n"));
        sensorsSleep();
        // Cut sensor power
        MS_DBG(F("  Cutting sensor power...\n"));
        sensorsPowerDown();
        // Create a csv data record and save it to the log file
        logToSD(generateSensorDataCSV());

        // Turn off the LED
        digitalWrite(_ledPin, LOW);
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n\n"));

        // Unset flag
        Logger::_isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::_startTesting) testingMode();

    // Sleep
    if(_sleep){systemSleep();}
}

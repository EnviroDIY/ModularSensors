/*
 *LoggerBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

#ifndef LoggerBase_h
#define LoggerBase_h

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts
#include <Sodaq_DS3231.h>  // To communicate with the clock
#include <SdFat.h>  // To communicate with the SD card

#include "VariableArray.h"

// Defines the "Logger" Class
class Logger : public VariableArray
{
public:
    // Initialization - cannot do this in constructor arduino has issues creating
    // instances of classes with non-empty constructors
    void init(int SDCardPin, int interruptPin,
              int variableCount,
              Variable *variableList[],
              float loggingIntervalMinutes,
              const char *loggerID = 0)
    {
        PRINTOUT(F("Initializing variable array with "), variableCount, F(" variables..."));

        _SDCardPin = SDCardPin;
        _interruptPin = interruptPin;
        _variableCount = variableCount;
        _variableList = variableList;
        _loggingIntervalMinutes = loggingIntervalMinutes;
        _interruptRate = round(_loggingIntervalMinutes*60);  // convert to even seconds
        _loggerID = loggerID;
        _autoFileName = false;
        _isFileNameSet = false;
        _numReadings = 0;

        // Set sleep variable, if an interrupt pin is given
        if(_interruptPin != -1)
        {
            _sleep = true;
        }

        PRINTOUT(F("   ... Success!\n"));
    };

    // Sets the static timezone - this must be set
    static void setTimeZone(int timeZone)
    {
        _timeZone = timeZone;
        // Some helpful prints for debugging
        PRINTOUT(F("Logger timezone is "));
        if (_timeZone == 0) PRINTOUT(F("UTC\n"));
        else if (_timeZone > 0) PRINTOUT(F("UTC+"));
        else PRINTOUT(F("UTC"));
        if (_timeZone != 0) PRINTOUT(_timeZone, F("\n"));

    }
    static int getTimeZone(void) { return Logger::_timeZone; }

    // This set the offset between the built-in clock and the time zone where
    // the data is being recorded.  If your RTC is set in UTC and your logging
    // timezone is EST, this should be -5.  If your RTC is set in EST and your
    // timezone is EST this does not need to be called.
    static void setTZOffset(int offset)
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
    static int getTZOffset(void) { return Logger::_offset; }

    // Sets up a pin for an LED or other way of alerting that data is being logged
    void setAlertPin(int ledPin)
    {
        _ledPin = ledPin;
        DBGVA(F("Pin "), _ledPin, F(" set for alerts\n"));
    }


    // ===================================================================== //
    // Public functions to access the clock in proper format and time zone
    // ===================================================================== //
    // This gets the current epoch time (unix time) and corrects it for the specified time zone
    static uint32_t getNow(void)
    {
      uint32_t currentEpochTime = rtc.now().getEpoch();
      currentEpochTime += _offset*3600;
      return currentEpochTime;
    }

    // This converts a date-time object into a ISO8601 formatted string
    static String formatDateTime_ISO8601(DateTime dt)
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
    static String formatDateTime_ISO8601(uint32_t epochTime)
    {
        // Create a DateTime object from the epochTime
        DateTime dt(rtc.makeDateTime(epochTime));
        return formatDateTime_ISO8601(dt);
    }

    // This sets static variables for the date/time - this is needed so that all
    // data outputs (SD, EnviroDIY, serial printing, etc) print the same time
    // for updating the sensors - even though the routines to update the sensors
    // and to output the data may take several seconds.
    // It is not currently possible to output the instantaneous time an individual
    // sensor was updated, just a single marked time.  By custom, this should be
    // called before updating the sensors, not after.
    void markTime(void)
    {
      markedEpochTime = getNow();
      markedDateTime = rtc.makeDateTime(markedEpochTime);
      formatDateTime_ISO8601(markedDateTime).toCharArray(markedISO8601Time, 26);
    }

    // This checks to see if the CURRENT time is an even interval of the logging rate
    // or we're in the first 15 minutes of logging
    bool checkInterval(void)
    {
        bool retval;
        DBGVA(F("Current Time: "), getNow(), F("\n"));
        DBGVA(F("Mod of Logging Interval: "), getNow() % _interruptRate, F("\n"));
        DBGVA(F("Number of Readings so far: "), _numReadings, F("\n"));
        DBGVA(F("Mod of 120: "), getNow() % 120, F("\n"));
        if ((getNow() % _interruptRate == 0 ) or
            (_numReadings < 10 and getNow() % 120 == 0))
        {
            // Update the time variables with the current time
            markTime();
            // Update the number of readings taken
            _numReadings ++;
            DBGVA(F("Time to log!\n"));
            retval = true;
        }
        else
        {
            DBGVA(F("Not time yet, back to sleep\n"));
            retval = false;
        }
        return retval;
    }

    // This checks to see if the MARKED time is an even interval of the logging rate
    // or we're in the first 15 minutes of logging
    bool checkMarkedInterval(void)
    {
        bool retval;
        DBGVA(F("Marked Time: "), markedEpochTime, F("\n"));
        DBGVA(F("Mod of Logging Interval: "), markedEpochTime % _interruptRate, F("\n"));
        DBGVA(F("Number of Readings so far: "), _numReadings, F("\n"));
        DBGVA(F("Mod of 120: "), markedEpochTime % 120, F("\n"));
        if (markedEpochTime != 0 &&
            ((markedEpochTime % _interruptRate == 0 ) or
            (_numReadings < 10 and markedEpochTime % 120 == 0)))
        {
            // Update the number of readings taken
            _numReadings ++;
            DBGVA(F("Time to log!\n"));
            retval = true;
        }
        else
        {
            DBGVA(F("Not time yet, back to sleep\n"));
            retval = false;
        }
        return retval;
    }


    // ============================================================================
    //  Public Functions for sleeping the logger
    // ============================================================================
    #include <avr/sleep.h>  // To handle the processor sleep modes

    // Set up the Interrupt Service Request for waking
    // In this case, we're doing nothing, we just want the processor to wake
    // This must be a static function (which means it can only call other static funcions.)
    static void wakeISR(void){}

    // Sets up the sleep mode
    void setupSleep(void)
    {
        // Set the pin attached to the RTC alarm to be in the right mode to listen to
        // an interrupt and attach the "Wake" ISR to it.
        pinMode(_interruptPin, INPUT_PULLUP);
        enableInterrupt(_interruptPin, wakeISR, CHANGE);

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
    void systemSleep(void)
    {
        // Wait until the serial ports have finished transmitting
        // This does not clear their buffers, it just waits until they are finished
        // TODO:  Make sure can find all serial ports
        Serial.flush();
        // Serial1.flush();

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

        DBGVA(F("The clock interrupt woke me up!\n"));
        // Clear the SE (sleep enable) bit.
        sleep_disable();
        // Re-enable the processor ADC
        ADCSRA |= _BV(ADEN);
    }

    // ===================================================================== //
    // Public functions for logging data to an SD card
    // ===================================================================== //
    // This sets a file name, if you want to decide on it in advance
    void setFileName(char *fileName)
    {
        // Save the filename to the static String
        _fileName = fileName;
        _isFileNameSet = true;

        // Print out the file name for debugging
        PRINTOUT(F("Data will be saved as "), _fileName, F("..."));
        if (!_autoFileName) PRINTOUT(F("\n"));
    }
    // Same as above, with a string (overload function)
    void setFileName(String fileName)
    {
        // Convert the string filename to a character file name
        int fileNameLength = fileName.length() + 1;
        char charFileName[fileNameLength];
        fileName.toCharArray(charFileName, fileNameLength);
        setFileName(charFileName);
    }

    // This generates a file name from the logger id and the current date
    // This will be used if the setFileName function is not called before
    // the begin() function is called.
    void setFileName(void)
    {
        _autoFileName = true;
        // Generate the file name from logger ID and date
        String fileName = "";
        if (_loggerID)
        {
            fileName +=  String(_loggerID);
            fileName +=  F("_");
        }
        fileName +=  formatDateTime_ISO8601(getNow()).substring(0, 10);
        fileName +=  F(".csv");
        setFileName(fileName);
    }

    // This returns the current filename.  Must be run after setFileName.
    String getFileName(void){return _fileName;}

    // This is a PRE-PROCESSOR MACRO to speed up generating header rows
    // Again, THIS IS NOT A FUNCTION, it is a pre-processor macro
    #define makeHeaderRowMacro(firstCol, function) \
        dataHeader += F("\""); \
        dataHeader += firstCol; \
        dataHeader += F("\","); \
        for (uint8_t i = 0; i < _variableCount; i++) \
        { \
            dataHeader += F("\""); \
            dataHeader += function; \
            dataHeader += F("\""); \
            if (i + 1 != _variableCount) \
            { \
                dataHeader += F(","); \
            } \
        } \
        dataHeader += F("\r\n");

    // This creates a header for the logger file
    virtual String generateFileHeader(void)
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
    String generateSensorDataCSV(void)
    {
        String csvString = "";
        markedDateTime.addToString(csvString);
        csvString += F(",");
        csvString += VariableArray::generateSensorDataCSV();
        return csvString;
    }

    // This initializes a file on the SD card and writes a header to it
    void setupLogFile(void)
    {
        // Initialise the SD card
        if (!sd.begin(_SDCardPin, SPI_FULL_SPEED))
        {
            PRINTOUT(F("Error: SD card failed to initialize or is missing.\n"));
            PRINTOUT(F("Data will not be saved!.\n"));
        }
        else  // skip everything else if there's no SD card, otherwise it might hang
        {
            PRINTOUT(F("Successfully connected to SD Card with card/slave select on pin "));
            PRINTOUT(_SDCardPin, F("\n"));

            if(!_isFileNameSet){setFileName();}
            else if(_autoFileName){setFileName();}
            else setFileName(_fileName);  // This just for a nice print-out

            // Convert the string filename to a character file name for SdFat
            int fileNameLength = _fileName.length() + 1;
            char charFileName[fileNameLength];
            _fileName.toCharArray(charFileName, fileNameLength);

            // Open the file in write mode (and create it if it did not exist)
            logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END);
            // Set creation date time
            logFile.timestamp(T_CREATE, rtc.makeDateTime(getNow()).year(),
                                        rtc.makeDateTime(getNow()).month(),
                                        rtc.makeDateTime(getNow()).date(),
                                        rtc.makeDateTime(getNow()).hour(),
                                        rtc.makeDateTime(getNow()).minute(),
                                        rtc.makeDateTime(getNow()).second());
            // Set write/modification date time
            logFile.timestamp(T_WRITE, rtc.makeDateTime(getNow()).year(),
                                       rtc.makeDateTime(getNow()).month(),
                                       rtc.makeDateTime(getNow()).date(),
                                       rtc.makeDateTime(getNow()).hour(),
                                       rtc.makeDateTime(getNow()).minute(),
                                       rtc.makeDateTime(getNow()).second());
            // Set access  date time
            logFile.timestamp(T_ACCESS, rtc.makeDateTime(getNow()).year(),
                                        rtc.makeDateTime(getNow()).month(),
                                        rtc.makeDateTime(getNow()).date(),
                                        rtc.makeDateTime(getNow()).hour(),
                                        rtc.makeDateTime(getNow()).minute(),
                                        rtc.makeDateTime(getNow()).second());
            PRINTOUT(F("   ... File created!\n"));

            // Add header information
            logFile.print(generateFileHeader());
            DBGVA(generateFileHeader(), F("\n"));

            //Close the file to save it
            logFile.close();
        }
    }

    // This writes a record to the SD card
    void logToSD(String rec)
    {
        // Make sure the SD card is still initialized
        if (!sd.begin(_SDCardPin, SPI_FULL_SPEED))
        {
            PRINTOUT(F("Error: SD card failed to initialize or is missing.\n"));
            PRINTOUT(F("Data will not be saved!.\n"));
        }
        else  // skip everything else if there's no SD card, otherwise it might hang
        {
            // Convert the string filename to a character file name for SdFat
            int fileNameLength = _fileName.length() + 1;
            char charFileName[fileNameLength];
            _fileName.toCharArray(charFileName, fileNameLength);

            // Check that the file exists, just in case someone yanked the SD card
            if (!logFile.open(charFileName, O_WRITE | O_AT_END))
            {
                PRINTOUT(F("SD Card File Lost!  Starting new file.\n"));
                setupLogFile();
            }

            // Write the CSV data
            logFile.println(rec);
            // Echo the line to the serial port
            PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/ \n"));
            PRINTOUT(rec, F("\n"));

            // Set write/modification date time
            logFile.timestamp(T_WRITE, rtc.makeDateTime(getNow()).year(),
                                       rtc.makeDateTime(getNow()).month(),
                                       rtc.makeDateTime(getNow()).date(),
                                       rtc.makeDateTime(getNow()).hour(),
                                       rtc.makeDateTime(getNow()).minute(),
                                       rtc.makeDateTime(getNow()).second());
            // Set access  date time
            logFile.timestamp(T_ACCESS, rtc.makeDateTime(getNow()).year(),
                                        rtc.makeDateTime(getNow()).month(),
                                        rtc.makeDateTime(getNow()).date(),
                                        rtc.makeDateTime(getNow()).hour(),
                                        rtc.makeDateTime(getNow()).minute(),
                                        rtc.makeDateTime(getNow()).second());

            // Close the file to save it
            logFile.close();
        }
    }

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //
    // This calls all of the setup functions - must be run AFTER init
    virtual void begin(void)
    {
        // Print a start-up note to the first serial port
        PRINTOUT(F("Beginning logger "), _loggerID, F("\n"));

        // Start the Real Time Clock
        rtc.begin();
        delay(100);

        // Set up pins for the LED's
        pinMode(_ledPin, OUTPUT);

        // Set up the sensors
        setupSensors();

        // Set up the log file
        setupLogFile();

        // Print the current time
        PRINTOUT(F("Current RTC time is: "));
        PRINTOUT(formatDateTime_ISO8601(getNow()), F("\n"));

        // Setup sleep mode
        if(_sleep){setupSleep();}

        PRINTOUT(F("Logger setup finished!\n"));
        PRINTOUT(F("------------------------------------------\n\n"));
    }

    // This is a one-and-done to log data
    virtual void log(void)
    {
        // Check of the current time is an even interval of the logging interval
        if (checkInterval())
        {
            // Print a line to show new reading
            PRINTOUT(F("------------------------------------------\n"));
            // Turn on the LED to show we're taking a reading
            digitalWrite(_ledPin, HIGH);

            // Wake up all of the sensors
            // I'm not doing as part of sleep b/c it may take up to a second or
            // two for them all to wake which throws off the checkInterval()
            sensorsWake();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();

            // Create a csv data record and save it to the log file
            logToSD(generateSensorDataCSV());

            // Turn off the LED
            digitalWrite(_ledPin, LOW);
            // Print a line to show reading ended
            PRINTOUT(F("------------------------------------------\n\n"));
        }

        // Sleep
        if(_sleep){systemSleep();}
    }

    // Publie variables
    // Time stamps - want to set them at a single time and carry them forward
    static long markedEpochTime;



// ===================================================================== //
// Things that are private and protected below here
// ===================================================================== //
protected:

    // The SD card and file
    SdFat sd;
    SdFile logFile;
    String _fileName;

    // Static variables - identical for EVERY logger
    static int _timeZone;
    static int _offset;

    // Time stamps - want to set them at a single time and carry them forward
    static DateTime markedDateTime;
    static char markedISO8601Time[26];

    // Initialization variables
    int _SDCardPin;
    int _interruptPin;
    float _loggingIntervalMinutes;
    int _interruptRate;
    const char *_loggerID;
    bool _autoFileName;
    bool _isFileNameSet;
    uint8_t _numReadings;
    bool _sleep;
    int _ledPin;
};

// Initialize the static timezone
int Logger::_timeZone = 0;
// Initialize the static time adjustment
int Logger::_offset = 0;
// Initialize the static timestamps
long Logger::markedEpochTime = 0;
DateTime Logger::markedDateTime = 0;
char Logger::markedISO8601Time[26];

#endif

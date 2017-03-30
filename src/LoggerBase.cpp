/*
 *LoggerBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

#include <Sodaq_PcInt_PCINT0.h>  // To handle pin change interrupts for the clock
#include <avr/sleep.h>  // To handle the processor sleep modes
#include <SdFat.h>  // To communicate with the SD card
#include "LoggerBase.h"

// Initialize the SD card and file
SdFat sd;
SdFile logFile;

// Initialize the timer functions for the RTC
RTCTimer timer;

// Set up the static variables for the initilizer
bool LoggerBase::sleep = false;  // Set in the init function
int LoggerBase::_timeZone = 0;  // Set in the init function

// Initialization - cannot do this in constructor arduino has issues creating
// instances of classes with non-empty constructors
void LoggerBase::init(int timeZone, int SDCardPin, int interruptPin,
                      int sensorCount,
                      SensorBase *SENSOR_LIST[],
                      float loggingIntervalMinutes,
                      const char *loggerID/* = 0*/,
                      const char *samplingFeature/* = 0*/,
                      const char *UUIDs[]/* = 0*/)
{
    _timeZone = timeZone;
    _SDCardPin = SDCardPin;
    _interruptPin = interruptPin;
    _sensorCount = sensorCount;
    _sensorList = SENSOR_LIST;
    _loggingIntervalMinutes = loggingIntervalMinutes;
    _interruptRate = round(_loggingIntervalMinutes*60);  // convert to even seconds
    _loggerID = loggerID;
    _samplingFeature = samplingFeature;
    _UUIDs = UUIDs;
    _autoFileName = false;

    // Set sleep variable, if an interrupt pin is given
    if(_interruptPin != -1)
    {
        LoggerBase::sleep = true;
    }
};

// Sets up a pin for an LED or other way of alerting that data is being logged
void LoggerBase::setAlertPin(int ledPin)
{
    _ledPin = ledPin;
}


// ============================================================================
//  Functions for interfacing with the real time clock (RTC, DS3231).
// ============================================================================

// Helper function to get the current date/time from the RTC
// as a unix timestamp - and apply the correct time zone.
uint32_t LoggerBase::getNow(void)
{
  uint32_t currentEpochTime = rtc.now().getEpoch();
  currentEpochTime += _timeZone*3600;
  return currentEpochTime;
}


// This function converts a DateTime object into an ISO 8601 formated string
String LoggerBase::formatDateTime_ISO8601(DateTime dt, int timeZone)
{
    // Set up an inital string
    String dateTimeStr;
    // Convert the DateTime object to a String
    dt.addToString(dateTimeStr);
    dateTimeStr.replace(F(" "), F("T"));
    String tzString = String(timeZone);
    if (-24 <= timeZone && timeZone <= -10)
    {
        tzString += F(":00");
    }
    else if (-10 < timeZone && timeZone < 0)
    {
        tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");
    }
    else if (timeZone == 0)
    {
        tzString = F("Z");
    }
    else if (0 < timeZone && timeZone < 10)
    {
        tzString = "+0" + tzString + F(":00");
    }
    else if (10 <= timeZone && timeZone <= 24)
    {
        tzString = "+" + tzString + F(":00");
    }
    dateTimeStr += tzString;
    return dateTimeStr;
}

// This function converts an epochTime (unix time) into an ISO 8601 formated string
String LoggerBase::formatDateTime_ISO8601(uint32_t epochTime, int timeZone)
{
    // Create a DateTime object from the epochTime
    DateTime dt(rtc.makeDateTime(epochTime));
    return formatDateTime_ISO8601(dt, timeZone);
}

// This checks to see if the current time is an even interval of the logging rate
bool LoggerBase::checkInterval(void)
{
    bool retval;
    if (getNow() % _interruptRate == 0)
    {
        // Serial.println(F("Time to log!"));  // for Debugging
        retval = true;
    }
    else
    {
        // Serial.println(F("Not time yet, back to sleep"));  // for Debugging
        retval = false;
    }
    return retval;
}


// Declare the static variables to mark the time
long LoggerBase::markedEpochTime = 0;
DateTime LoggerBase::markedDateTime = 0;
char LoggerBase::markedISO8601Time[26] = "";
// This function sets all of the static variables to mark the time
void LoggerBase::markTime(void)
{
  LoggerBase::markedEpochTime = getNow();
  LoggerBase::markedDateTime = rtc.makeDateTime(LoggerBase::markedEpochTime);
  formatDateTime_ISO8601(LoggerBase::markedDateTime, _timeZone).toCharArray(LoggerBase::markedISO8601Time, 26);
}


// ============================================================================
// Functions for the timer - to do repeated events without using a delay function
// We want to use the timer instead of the delay because if using the delay
// nothing else can happen during the delay, whereas with a timer other processes
// can continue while the timer counts down.
// ============================================================================

// This sets up the function to be called by the timer with no return of its own.
// This structure is required by the timer library.
// See http://support.sodaq.com/sodaq-one/adding-a-timer-to-schedule-readings/
void LoggerBase::checkTime(uint32_t ts)
{
  // Serial.println(LoggerBase::markedISO8601Time); // For debugging
}

// Set-up the RTC Timer events
void LoggerBase::setupTimer(void)
{
    // Instruct the RTCTimer how to get the current time reading (ie, what function to use)
    // The units of the time returned by this function determine the units of the
    // period in the "every" function below.
    timer.setNowCallback(getNow);

    // This tells the timer how often (_interruptRate) it will repeat some function (checkTime)
    // The time units of the first input are the same as those returned by the
    // setNowCallback function above (in this case, seconds).  We are only
    // having the timer call a function to check the time instead of actually
    // taking a reading because we would rather first double check that we're
    // exactly on a current minute before taking the reading.
    timer.every(_interruptRate, checkTime);
}


// ============================================================================
//  Functions for sleeping the logger
// ============================================================================

// Set up the Interrupt Service Request for waking - in this case, doing nothing
void LoggerBase::wakeISR(void){}

// Sets up the sleep mode
void LoggerBase::setupSleep(void)
{
    // Set the pin attached to the RTC alarm to be in the right mode to listen to
    // an interrupt and attach the "Wake" ISR to it.
    pinMode(_interruptPin, INPUT_PULLUP);
    PcInt::attachInterrupt(_interruptPin, wakeISR);

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
void LoggerBase::systemSleep(void)
{
    // Serial.println(F("Putting system to sleep."));  // For debugging
    // This method handles any sensor specific sleep
    sensorsSleep();

    // Wait until the serial ports have finished transmitting
    // This does not clear their buffers, it just waits until they are finished
    // TODO:  Make sure can find all serial ports
    Serial.flush();
    Serial1.flush();

    // This clears the interrrupt flag in status register of the clock
    // The next timed interrupt will not be sent until this is cleared
    rtc.clearINTStatus();

    // Disable the processor ADC
    ADCSRA &= ~_BV(ADEN);
    // stop interrupts to ensure the BOD timed sequence executes as required
    cli();
    // turn off the brown-out detector
    byte mcucr1, mcucr2;
    mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);
    mcucr2 = mcucr1 & ~_BV(BODSE);
    MCUCR = mcucr1;
    MCUCR = mcucr2;
    // ensure interrupts enabled so we can wake up again
    sei();

    // Sleep time
    // Disables interrupts
    noInterrupts();
    // Prepare the processor for by setting the SE (sleep enable) bit.
    sleep_enable();
    // Re-enables interrupts
    interrupts();
    // Actually put the processor into sleep mode.
    // This must happen after the SE bit is set.
    sleep_cpu();

    // Serial.println(F("Waking up!"));  // For debugging
    // Clear the SE (sleep enable) bit.
    sleep_disable();
    // Re-enable the processor ADC
    ADCSRA |= _BV(ADEN);
    // This method handles any sensor specific wake-up
    sensorsWake();
}


// ============================================================================
//  Functions for logging data to an SD card
// ============================================================================

// Sets up the filename and saves it to the static char array
String LoggerBase::_fileName = "";
void LoggerBase::setFileName(char *fileName)
{
    // Save the filename to the static String
    LoggerBase::_fileName = fileName;

    // Print out the file name for debugging
    Serial.print(F("Data will be saved as "));  // for debugging
    Serial.println(LoggerBase::_fileName);  // for debugging
}

void LoggerBase::setFileName(void)
{
    _autoFileName = true;
    // Generate the file name from logger ID and date
    char fileName[strlen(_loggerID) + 16] = {};
    strcat(fileName,  _loggerID);
    strcat(fileName, "_");
    strncat(fileName, LoggerBase::markedISO8601Time, 10);
    strcat(fileName, ".csv");
    setFileName(fileName);
}

String LoggerBase::getFileName(void)
{
    return LoggerBase::_fileName;
}

// Initializes the SD card and prints a header to it
void LoggerBase::setupLogFile(void)
{
    // Initialise the SD card
    Serial.print(F("Connecting to SD Card with card/slave select on pin "));  // for debugging
    Serial.println(_SDCardPin);  // for debugging
    if (!sd.begin(_SDCardPin, SPI_FULL_SPEED))
    {
        Serial.println(F("Error: SD card failed to initialize or is missing."));
    }

    // Convert the string filename to a character file name for SdFat
    int fileNameLength = LoggerBase::_fileName.length() + 1;
    char charFileName[fileNameLength];
    LoggerBase::_fileName.toCharArray(charFileName, fileNameLength);

    // Open the file in write mode (and create it if it did not exist)
    logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END);
    // TODO: set creation date time

    // Add header information
    logFile.print(F("Data Logger: "));
    logFile.println(_loggerID);
    logFile.print(F("Sampling Feature UUID: "));
    logFile.println(_samplingFeature);

    String dataHeader = F("\"Timestamp\", ");
    for (uint8_t i = 0; i < _sensorCount; i++)
    {
        dataHeader += "\"" + String(_sensorList[i]->getSensorName());
        dataHeader += " " + String(_sensorList[i]->getVarName());
        dataHeader += " " + String(_sensorList[i]->getVarUnit());
        dataHeader += " (" + String(_UUIDs[i]) + ")\"";
        if (i + 1 != _sensorCount)
        {
            dataHeader += F(", ");
        }
    }

    // Serial.println(dataHeader);  // for debugging
    logFile.println(dataHeader);

    //Close the file to save it
    logFile.close();
}

String LoggerBase::generateSensorDataCSV(void)
{
    String csvString = String(LoggerBase::markedISO8601Time) + F(", ");

    for (uint8_t i = 0; i < _sensorCount; i++)
    {
        csvString += String(_sensorList[i]->getValue());
        if (i + 1 != _sensorCount)
        {
            csvString += F(", ");
        }
    }

    return csvString;
}

// Writes a string to a text file on the SD Card
// By default writes a comma-separated line
void LoggerBase::logToSD(String rec)
{
    // Make sure the SD card is still initialized
    if (!sd.begin(_SDCardPin, SPI_FULL_SPEED))
    {
        Serial.println(F("Error: SD card failed to initialize or is missing."));
    }

    // Convert the string filename to a character file name for SdFat
    int fileNameLength = LoggerBase::_fileName.length() + 1;
    char charFileName[fileNameLength];
    LoggerBase::_fileName.toCharArray(charFileName, fileNameLength);

    // Check that the file exists, just in case someone yanked the SD card
    if (!logFile.open(charFileName, O_WRITE | O_AT_END))
    {
        Serial.println(F("SD Card File Lost!  Starting new file."));  // for debugging
        if(_autoFileName){setFileName();}
        setupLogFile();
    }

    // Write the CSV data
    logFile.println(rec);
    // Echo the lind to the serial port
    Serial.println(F("\n \\/---- Line Saved to SD Card ----\\/ "));  // for debugging
    Serial.println(rec);  // for debugging

    // Close the file to save it
    logFile.close();
}


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================
void LoggerBase::begin(void)
{
    // Start the Real Time Clock
    rtc.begin();
    delay(100);

    // Set up pins for the LED's
    pinMode(_ledPin, OUTPUT);

    // Update the static time variables with the current time
    markTime();

    // Print a start-up note to the first serial port
    Serial.print(F("Current RTC time is: "));
    Serial.println(LoggerBase::markedISO8601Time);
    Serial.print(F("There are "));
    Serial.print(String(_sensorCount));
    Serial.println(F(" variables being recorded."));

    // Set up the sensors
    Serial.println(F("Setting up sensors."));
    setupSensors();

    // Set up the log file
    setFileName();
    setupLogFile();

    // Setup timer events
    setupTimer();

    // Setup sleep mode
    if(sleep){setupSleep();}

    Serial.println(F("Setup finished!"));
    Serial.println(F("------------------------------------------\n"));
}

void LoggerBase::log(void)
{
    // Update the timer
    // This runs the timer's "now" function [in our case getNow()] and then
    // checks all of the registered timer events to see if they should run
    timer.update();

    // Check of the current time is an even interval of the logging interval
    if (checkInterval())
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(_ledPin, HIGH);

        // Update the static time variables with the current time
        markTime();
        // Update the values from all attached sensors
        updateAllSensors();
        // Immediately put sensors to sleep to save power
        sensorsSleep();

        // Create a csv data record and save it to the log file
        logToSD(generateSensorDataCSV());

        // Turn off the LED
        digitalWrite(_ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    // Sleep
    if(sleep){systemSleep();}
}

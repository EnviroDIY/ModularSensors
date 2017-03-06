/*
 *LoggerBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the logging functions - ie, saving to an SD card.
*/

#include <RTCTimer.h>  // To handle timing on a schedule
#include <Sodaq_PcInt_PCINT0.h>  // To handle pin change interrupts for the clock
#include <avr/sleep.h>  // To handle the processor sleep modes
#include <SdFat.h>  // To communicate with the SD card
#include "LoggerBase.h"

// Initialize the SD card
SdFat SD;

// Initialize the timer functions for the RTC
RTCTimer timer;

// Set up the static variables for the current time and timer functions
static char currentTime[26] = "";
static long currentepochtime = 0;

// Constructor (should there be an init??)
Logger::Logger(int timeZone, int SDCardPin, SensorBase *SENSOR_LIST[],
               char *loggerID/* = 0*/,
               char *samplingFeature/* = 0*/,
               char *UUIDs[]/* = 0*/)
{
    _timeZone = timeZone;
    _SDCardPin = SDCardPin;
    _sensorList = SENSOR_LIST;
    _loggerID = loggerID;
    _sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);
    _samplingFeature = samplingFeature;
    _UUIDs = UUIDs;
};


// ============================================================================
//  Functions for interfacing with the real time clock (RTC, DS3231).
// ============================================================================

// Helper function to get the current date/time from the RTC
// as a unix timestamp - and apply the correct time zone.
uint32_t Logger::getNow(void)
{
  currentepochtime = rtc.now().getEpoch();
  currentepochtime += _timeZone*3600;
  return currentepochtime;
}

// This function returns the datetime from the realtime clock as an ISO 8601 formated string
String Logger::getDateTime_ISO8601(void)
{
    String dateTimeStr;
    //Create a DateTime object from the current time
    DateTime dt(rtc.makeDateTime(getNow()));
    //Convert it to a String
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


// ============================================================================
//  Functions for interfacing with sensors.
// ============================================================================

// Return the number of sensors, in case someone wants it
uint8_t Logger::countSensors(void)
{
    // Count the number of sensors
    return _sensorCount;
}

// This sets up the sensors, generally setting pin modes and the like
bool Logger::setupSensors(void)
{
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->setup();
    }

    return success;
}

bool Logger::sensorsSleep()
{
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->sleep();
    }

    return success;
}

bool Logger::sensorsWake()
{
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->wake();
    }

    return success;
}

// This function updates the values for any connected sensors.
bool Logger::updateAllSensors(void)
{
    // Get the clock time when we begin updating sensors
    getDateTime_ISO8601().toCharArray(currentTime, 26) ;

    bool success = true;
    for (uint8_t i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->update();
        // Prints for debugging
        Serial.print(F("--- Updated "));
        Serial.print(_sensorList[i]->getSensorName());
        Serial.print(F(" for "));
        Serial.print(_sensorList[i]->getVarName());

        // Check for and skip the updates of any identical sensors
        for (int j = i+1; j < _sensorCount; j++)
        {
            if (_sensorList[i]->getSensorName() == _sensorList[j]->getSensorName() &&
                _sensorList[i]->getSensorLocation() == _sensorList[j]->getSensorLocation())
            {
                // Prints for debugging
                Serial.print(F(" and "));
                Serial.print(_sensorList[i+1]->getVarName());
                i++;
            }
            else {break;}
        }
        Serial.println(F(" ---"));  // For Debugging
        // delay(250);  // A short delay before next sensor - is this necessary??
    }

    return success;
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
void Logger::checkTime(uint32_t ts)
{
  // Update the current date/time
  getNow();
}

// Set-up the RTC Timer events
void Logger::setupTimer(uint32_t period)
{
    // Instruct the RTCTimer how to get the current time reading (ie, what function to use)
    // The units of the time returned by this function determine the units of the
    // "every" function below.
    timer.setNowCallback(getNow);

    // This tells the timer how often (period) it will repeat some function (getNow)
    // The time units of the first input are the same as those returned by the
    // setNowCallback function above (in this case, seconds).  We are only
    // having the timer call a function to check the time instead of actually
    // taking a reading because we would rather first double check that we're
    // exactly on a current minute before taking the reading.
    timer.every(period, checkTime);
}

// Set up the Interrupt Service Request for waking - in this case, doing nothing
void Logger::wakeISR(void)
{
  //Leave this blank
}


// ============================================================================
//  Functions for sleeping the logger
// ============================================================================

// Sets up the sleep mode (used on device wake-up)
void Logger::setupSleep(int interruptPin, uint8_t periodicity /*= EveryMinute*/)
{
    // Attach the RTC alarm pin to a pin that can accept pin-change interrupts
    pinMode(interruptPin, INPUT_PULLUP);
    PcInt::attachInterrupt(interruptPin, wakeISR);

    // Put the RTC itself into in alarm mode - that is, allowing it to send interrupts
    // Essentially, we're telling the clock to send a signal over the alarm pin
    // (interrupt pin) at this rate.  For the general purpose of logging, I'm
    // defaulting this to EveryMinute
    rtc.enableInterrupts(periodicity);

    // Set the sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

// Puts the system to sleep to conserve battery life.
void Logger::systemSleep(void)
{
  // This method handles any sensor specific sleep setup
  sensorsSleep();

  // Wait until the serial ports have finished transmitting
  Serial.flush();
  Serial1.flush();

  // This clears the interrrupt flag in status register.
  // The next timed interrupt will not be sent until this is cleared
  rtc.clearINTStatus();

  // Disable ADC
  ADCSRA &= ~_BV(ADEN);

  // Sleep time
  noInterrupts();
  sleep_enable();
  interrupts();
  sleep_cpu();
  sleep_disable();

  // Enable ADC
  ADCSRA |= _BV(ADEN);

  // This method handles any sensor specific wake setup
  sensorsWake();
}


// ============================================================================
//  Functions for logging data to an SD card
// ============================================================================

// Initializes the SDcard and prints a header to it
void Logger::setupLogFile(void)
{
  // Initialise the SD card
  if (!SD.begin(_SDCardPin))
  {
    Serial.println(F("Error: SD card failed to initialise or is missing."));
  }

  _fileName = String(_loggerID) + F("_") + getDateTime_ISO8601().substring(0,10) + F(".txt");
  // Check if the file already exists
  bool oldFile = SD.exists(_fileName.c_str());

  // Open the file in write mode
  File logFile = SD.open(_fileName, FILE_WRITE);

  // Add header information if the file did not already exist
  if (!oldFile)
  {
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

    // Serial.println(dataHeader);
    logFile.println(dataHeader);
  }

  //Close the file to save it
  logFile.close();
}

String Logger::generateSensorDataCSV(void)
{
    String csvString = String(currentTime) + F(", ");

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
void Logger::logData(String rec /* = generateSensorDataCSV()*/)
{
  // Re-open the file
  File logFile = SD.open(_fileName, FILE_WRITE);

  // Write the CSV data
  logFile.println(rec);

  // Close the file to save it
  logFile.close();
}


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================
void Logger::Setup(int interruptPin /*= -1*/, uint8_t periodicity /*= EveryMinute*/)
{
    // Set up all the sensors
    setupSensors();

    // Set up the log file
    setupLogFile();

    // Figure out how often the timer function should check the clock based on
    // the alarm/interrupt periodicity of the real-time clock.
    uint32_t period = 0;
    switch(periodicity)
    {
        case EverySecond:  // If the RTC is sending an alarm every second
        period = 1;  // the timer will check the clock every second.
        break;

        case EveryMinute:  // If the RTC is sending an alarm every minute
        period = 15;  // the timer will check the clock every 15 seconds.
        break;

        case EveryHour:  // If the RTC is sending an alarm every hour
        period = 60*5;  // the timer will check the clock every 5 minutes.
        break;
    }
    // Setup timer events
    setupTimer(period);

    // Setup sleep mode, if an interrupt pin is given
    if(interruptPin != -1)
    {
        sleep = true;
        setupSleep(interruptPin, periodicity);
    }

    // Print a start-up note to the first serial port
    Serial.print(F("Current RTC time is: "));
    Serial.println(getDateTime_ISO8601());
    Serial.print(F("There are "));
    Serial.print(String(_sensorCount));
    Serial.println(F(" variables being recorded."));
    Serial.print(F("Data being saved to SD Card as "));
    Serial.println(_fileName);
}

void Logger::Log(int loggingIntervalMinutes)
{
    // Update the timer
    timer.update();

    // Check of the current time is an even interval of the logging interval
    if (currentepochtime % loggingIntervalMinutes*60 == 0)
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------\n"));  // for debugging

        // Update the values from all attached sensors
        updateAllSensors();

        //Save the data record to the log file
        logData(generateSensorDataCSV());
        Serial.println(generateSensorDataCSV());  // for debugging
    }

    delay(200);
    //Sleep
    if(sleep){systemSleep();}
}

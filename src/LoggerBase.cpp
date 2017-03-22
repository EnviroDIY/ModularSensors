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

// Initialize the SD card
SdFat SD;

// Initialize the timer functions for the RTC
RTCTimer timer;

// Set up the static variables for the current time and timer functions
char LoggerBase::currentTime[26] = "";
long LoggerBase::currentepochtime = 0;
int LoggerBase::_timeZone = 0;
bool LoggerBase::sleep = false;

// Initialization - cannot do this in constructor because it must happen
// within the setup and if using the constuctor cannot control when
// it happens
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
  currentepochtime = rtc.now().getEpoch();
  currentepochtime += _timeZone*3600;
  return currentepochtime;
}

// This function returns the datetime from the realtime clock as an ISO 8601 formated string
String LoggerBase::getDateTime_ISO8601(void)
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

// This function checks to see if it is the proper interval to log on.
bool LoggerBase::checkInterval(void)
{
    bool retval;
    if (currentepochtime % _interruptRate == 0)
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


// ============================================================================
//  Functions for interfacing with sensors.
// ============================================================================

// This sets up the sensors, generally setting pin modes and the like
bool LoggerBase::setupSensors(void)
{
    bool success = true;
    bool sensorSuccess = false;
    int setupTries = 0;
    for (int i = 0; i < _sensorCount; i++)
    {
        // Make 5 attempts before giving up
        while(setupTries < 5)
        {
            sensorSuccess = _sensorList[i]->setup();
            // Prints for debugging
            if(sensorSuccess)
            {
                Serial.print(F("--- Successfully set up "));
                Serial.print(_sensorList[i]->getSensorName());
                Serial.println(F(" ---"));
                break;
            }
            else
            {
                Serial.print(F("--- Setup for  "));
                Serial.print(_sensorList[i]->getSensorName());
                Serial.println(F(" failed! ---"));
                setupTries++;
            }
        }
        success &= sensorSuccess;

        // Check for and skip the setup of any identical sensors
        for (int j = i+1; j < _sensorCount; j++)
        {
            if (_sensorList[i]->getSensorName() == _sensorList[j]->getSensorName() &&
                _sensorList[i]->getSensorLocation() == _sensorList[j]->getSensorLocation())
            {i++;}
            else {break;}
        }
    }
    return success;
}

bool LoggerBase::sensorsSleep(void)
{
    // Serial.println(F("Putting sensors to sleep."));  // For debugging
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->sleep();
    }

    return success;
}

bool LoggerBase::sensorsWake(void)
{
    // Serial.println(F("Waking sensors."));  // For debugging
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->wake();
    }

    return success;
}

// This function updates the values for any connected sensors.
bool LoggerBase::updateAllSensors(void)
{
    // Get the clock time when we begin updating sensors
    getDateTime_ISO8601().toCharArray(currentTime, 26) ;

    bool success = true;
    for (uint8_t i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->update();
        // Prints for debugging
        // Serial.print(F("--- Updated "));  // For debugging
        // Serial.print(_sensorList[i]->getSensorName());  // For debugging
        // Serial.print(F(" for "));  // For debugging
        // Serial.print(_sensorList[i]->getVarName());  // For debugging

        // Check for and skip the updates of any identical sensors
        for (int j = i+1; j < _sensorCount; j++)
        {
            if (_sensorList[i]->getSensorName() == _sensorList[j]->getSensorName() &&
                _sensorList[i]->getSensorLocation() == _sensorList[j]->getSensorLocation())
            {
                // Prints for debugging
                // Serial.print(F(" and "));  // For debugging
                // Serial.print(_sensorList[i+1]->getVarName());  // For debugging
                i++;
            }
            else {break;}
        }
        // Serial.println(F(" ---"));  // For Debugging
        // delay(250);  // A short delay before next sensor - is this necessary??
    }

    return success;
}

// This function prints out the results for any connected sensors to a stream
void LoggerBase::printSensorData(Stream *stream)
{
    stream->print(F("Updated all sensors at "));
    stream->println(currentTime);
    for (int i = 0; i < _sensorCount; i++)
    {
        stream->print(_sensorList[i]->getSensorName());
        stream->print(F(" attached at "));
        stream->print(_sensorList[i]->getSensorLocation());
        stream->print(F(" has status "));
        stream->print(_sensorList[i]->getStatus());
        stream->print(F(" and reports "));
        stream->print(_sensorList[i]->getVarName());
        stream->print(F(" is "));
        stream->print(_sensorList[i]->getValue());
        stream->print(F(" "));
        stream->print(_sensorList[i]->getVarUnit());
        stream->println();
    }
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
  // Update the current date/time
  getNow();
  // Serial.println(getNow()); // For debugging
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

    // TODO:  Make this work!!
    // Figure out how frequently we want the interrupts to go off
    // based on the logging interval
    // uint8_t interruptSeconds;
    // uint8_t interruptMinutes;
    // uint8_t interruptHours;
    // int interruptRate = _interruptRate;  // convert to even seconds
    // interruptSeconds = interruptRate % 60;
    // interruptRate /= 60;  // now interruptRate is minutes
    // interruptMinutes = interruptRate % 60;
    // interruptRate /= 60;  // now interruptRate is hours
    // interruptHours = interruptRate;

    // Put the RTC itself into in alarm mode - that is, allowing it to send interrupts
    // Essentially, we're telling the clock to send a signal over the alarm pin
    // (interrupt pin) at this rate.
    // rtc.enableInterrupts(interruptHours, interruptMinutes, interruptSeconds);

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

// Sets up the filename
String LoggerBase::_fileName = "";
// Initializes the SD card and prints a header to it
void LoggerBase::setupLogFile(void)
{
    // Initialise the SD card
    Serial.print(F("Connecting to SD Card with card/slave select on pin "));  // for debugging
    Serial.println(_SDCardPin);  // for debugging
    if (!SD.begin(_SDCardPin))
    {
        Serial.println(F("Error: SD card failed to initialize or is missing."));
    }

    LoggerBase::_fileName = String(_loggerID) + F("_");
    LoggerBase::_fileName += getDateTime_ISO8601().substring(0,10) + F(".csv");
    Serial.print(F("Data being saved as "));  // for debugging
    Serial.println(LoggerBase::_fileName);  // for debugging

    // Open the file in write mode (and create it if it did not exist)
    File logFile = SD.open(LoggerBase::_fileName, FILE_WRITE);

    // Add header information
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
void LoggerBase::logToSD(String rec)
{
  // Re-open the file
  File logFile = SD.open(LoggerBase::_fileName, FILE_WRITE);

  // Write the CSV data
  Serial.println(F("\n \\/---- Line Saved to SD Card ----\\/ "));  // for debugging
  Serial.println(generateSensorDataCSV());  // for debugging
  logFile.println(rec);

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

    // Print a start-up note to the first serial port
    Serial.print(F("Current RTC time is: "));
    Serial.println(getDateTime_ISO8601());
    Serial.print(F("There are "));
    Serial.print(String(_sensorCount));
    Serial.println(F(" variables being recorded."));

    // Set up the sensors
    Serial.println(F("Setting up sensors."));
    setupSensors();

    // Set up the log file
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
    timer.update();

    // Check of the current time is an even interval of the logging interval
    if (checkInterval())
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(_ledPin, HIGH);

        // Update the values from all attached sensors
        updateAllSensors();
        // Immediately put sensors to sleep to save power
        sensorsSleep();

        //Save the data record to the log file
        logToSD(generateSensorDataCSV());

        // Turn off the LED
        digitalWrite(_ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    // Sleep
    if(sleep){systemSleep();}
}

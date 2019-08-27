/*
 *LoggerBase.h

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

#include "LoggerBase.h"
#include "dataPublisherBase.h"

// To prevent compiler/linker crashes with Enable interrupt
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include <EnableInterrupt.h>
// For all i2c communication, including with the real time clock
#include <Wire.h>


//Time Zone support in hours from UTC/GMT −10 to +14 https://en.wikipedia.org/wiki/Coordinated_Universal_Time
// Initialize the static timezone
int8_t Logger::_loggerTimeZone = 0;
// Initialize the static time adjustment
int8_t Logger::_loggerRTCOffset = 0;
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


// Constructors
Logger::Logger(const char *loggerID, uint16_t loggingIntervalMinutes,
               int8_t SDCardSSPin, int8_t mcuWakePin,
               VariableArray *inputArray)
{
    // Set parameters from constructor
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Set the initial pin values
    _SDCardPowerPin = -1;
    setSDCardSS(SDCardSSPin);
    setRTCWakePin(mcuWakePin);
    _ledPin = -1;
    _buttonPin = -1;

    // Initialize with no file name
    _fileName = "";

    // Start with no feature UUID
    _samplingFeatureUUID = NULL;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++)
    {
        dataPublishers[i] = NULL;
    }

    // MS_DBG(F("Logger object created"));
}
Logger::Logger(const char *loggerID, uint16_t loggingIntervalMinutes,
               VariableArray *inputArray)
{
    // Set parameters from constructor
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Set the initial pin values
    _SDCardPowerPin = -1;
    _SDCardSSPin = -1;
    _mcuWakePin = -1;
    _ledPin = -1;
    _buttonPin = -1;

    // Initialize with no file name
    _fileName = "";

    // Start with no feature UUID
    _samplingFeatureUUID = NULL;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++)
    {
        dataPublishers[i] = NULL;
    }

    // MS_DBG(F("Logger object created"));
}
Logger::Logger()
{
    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Set the initial pin values
    _SDCardPowerPin = -1;
    _SDCardSSPin = -1;
    _mcuWakePin = -1;
    _ledPin = -1;
    _buttonPin = -1;

    // Initialize with no file name
    _fileName = "";

    // Start with no feature UUID
    _samplingFeatureUUID = NULL;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++)
    {
        dataPublishers[i] = NULL;
    }

    // MS_DBG(F("Logger object created"));
}
// Destructor
Logger::~Logger(){}



// ===================================================================== //
// Public functions to get and set basic logging paramters
// ===================================================================== //

// Sets the logger ID
void Logger::setLoggerID(const char *loggerID)
{
    _loggerID = loggerID;
}

// Sets/Gets the logging interval
void Logger::setLoggingInterval(uint16_t loggingIntervalMinutes)
{
    _loggingIntervalMinutes = loggingIntervalMinutes;
}


// Adds the sampling feature UUID
void Logger::setSamplingFeatureUUID(const char *samplingFeatureUUID)
{
    _samplingFeatureUUID = samplingFeatureUUID;
}

// Sets up a pin controlling the power to the SD card
void Logger::setSDCardPwr(int8_t SDCardPowerPin)
{
    _SDCardPowerPin = SDCardPowerPin;
    if (_SDCardPowerPin >= 0)
    {
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, LOW);
    }
}
// NOTE:  Structure of power switching on SD card taken from:
// https://thecavepearlproject.org/2017/05/21/switching-off-sd-cards-for-low-power-data-logging/
void Logger::turnOnSDcard(bool waitToSettle)
{
    if (_SDCardPowerPin >= 0)
    {
        digitalWrite(_SDCardPowerPin, HIGH);
        if (waitToSettle)  // TODO:  figure out how long to wait
        {
            delay(6);
        }
    }
}
void Logger::turnOffSDcard(bool waitForHousekeeping)
{
    if (_SDCardPowerPin >= 0)
    {
        // TODO: set All SPI pins to INPUT?
        // TODO: set ALL SPI pins HIGH (~30k pullup)
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, LOW);
        if (waitForHousekeeping)  // TODO:  wait in lower power mode
        {
            // Specs say up to 1s for internal housekeeping after each write
            delay(1000);
        }
    }
}


// Sets up a pin for the slave select (chip select) of the SD card
void Logger::setSDCardSS(int8_t SDCardSSPin)
{
    _SDCardSSPin = SDCardSSPin;
    if (_SDCardSSPin >= 0)
    {
        pinMode(_SDCardSSPin, OUTPUT);
    }
}


// Sets both pins related to the SD card
void Logger::setSDCardPins(int8_t SDCardSSPin, int8_t SDCardPowerPin)
{
    setSDCardPwr(SDCardPowerPin);
    setSDCardSS(SDCardSSPin);
}


// Sets up the wake up pin for an RTC interrupt
void Logger::setRTCWakePin(int8_t mcuWakePin)
{
    _mcuWakePin = mcuWakePin;
    if (_mcuWakePin >= 0)
    {
        pinMode(_mcuWakePin, INPUT_PULLUP);
    }
}


// Sets up a pin for an LED or other way of alerting that data is being logged
void Logger::setAlertPin(int8_t ledPin)
{
    _ledPin = ledPin;
    if (_ledPin >= 0 )
    {
        pinMode(_ledPin, OUTPUT);
    }
}
void Logger::alertOn()
{
    if (_ledPin >= 0)
    {
        digitalWrite(_ledPin, HIGH);
    }
}
void Logger::alertOff()
{
    if (_ledPin >= 0)
    {
        digitalWrite(_ledPin, LOW);
    }
}


// Sets up a pin for an interrupt to enter testing mode
void Logger::setTestingModePin(int8_t buttonPin)
{
    _buttonPin = buttonPin;

    // Set up the interrupt to be able to enter sensor testing mode
    // NOTE:  Entering testing mode before the sensors have been set-up may
    // give unexpected results.
    if (_buttonPin >= 0)
    {
        pinMode(_buttonPin, INPUT_PULLUP);
        enableInterrupt(_buttonPin, Logger::testingISR, CHANGE);
    }
}


// Sets up the five pins of interest for the logger
void Logger::setLoggerPins(int8_t mcuWakePin,
                           int8_t SDCardSSPin,
                           int8_t SDCardPowerPin,
                           int8_t buttonPin,
                           int8_t ledPin)
{
    setRTCWakePin(mcuWakePin);
    setSDCardSS(SDCardSSPin);
    setSDCardPwr(SDCardPowerPin);
    setTestingModePin(buttonPin);
    setAlertPin(ledPin);
}



// ===================================================================== //
// Public functions to get information about the attached variable array
// ===================================================================== //

// Assigns the variable array object
void Logger::setVariableArray(VariableArray *inputArray)
{
    _internalArray = inputArray;
}


// Returns the number of variables in the internal array
uint8_t Logger::getArrayVarCount()
{
    return _internalArray->getVariableCount();
}


// This gets the name of the parent sensor, if applicable
String Logger::getParentSensorNameAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getParentSensorName();
}
// This gets the name and location of the parent sensor, if applicable
String Logger::getParentSensorNameAndLocationAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getParentSensorNameAndLocation();
}
// This gets the variable's name using http://vocabulary.odm2.org/variablename/
String Logger::getVarNameAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getVarName();
}
// This gets the variable's unit using http://vocabulary.odm2.org/units/
String Logger::getVarUnitAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getVarUnit();
}
// This returns a customized code for the variable, if one is given, and a default if not
String Logger::getVarCodeAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getVarCode();
}
// This returns the variable UUID, if one has been assigned
String Logger::getVarUUIDAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getVarUUID();
}
// This returns the current value of the variable as a string with the
// correct number of significant figures
String Logger::getValueStringAtI(uint8_t position_i)
{
    return _internalArray->arrayOfVars[position_i]->getValueString();
}



// ===================================================================== //
// Public functions for internet and dataPublishers
// ===================================================================== //

// Set up communications
// Adds a loggerModem objct to the logger
// loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
void Logger::attachModem(loggerModem& modem)
{
    _logModem = &modem;
}


// Takes advantage of the modem to synchronize the clock
bool Logger::syncRTC()
{
    bool success = false;
    if (_logModem != NULL)
    {
        // Synchronize the RTC with NIST
        PRINTOUT(F("Attempting to synchronize RTC with NIST"));
        PRINTOUT(F("This may take up to two minutes!"));
        // Connect to the network
        // The connectInternet function will also power the modem up and run
        // its setup function if necessary.
        if (_logModem->connectInternet(120000L))
        {
            success = setRTClock(_logModem->getNISTTime());
            // Disconnect from the network - ehh, why bother
            // _logModem->disconnectInternet();
        }
        // Turn off the modem
        _logModem->modemSleepPowerDown();
    }
    return success;
}


void Logger::registerDataPublisher(dataPublisher* publisher)
{
    // find the next empty spot in the publisher array
    uint8_t i = 0;
    for (; i < MAX_NUMBER_SENDERS; i++)
    {
        if (dataPublishers[i] == NULL) break;
    }

    // register the publisher there
    dataPublishers[i] = publisher;
}


void Logger::publishDataToRemotes(void)
{
    MS_DBG(F("Sending out remote data."));

    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++)
    {
        if (dataPublishers[i] != NULL)
        {
            PRINTOUT(F("\nSending data to"), dataPublishers[i]->getEndpoint());
            // dataPublishers[i]->publishData(_logModem->getClient());
            dataPublishers[i]->publishData();
            watchDogTimer.resetWatchDog();
        }
    }
}
void Logger::sendDataToRemotes(void) { publishDataToRemotes(); }



// ===================================================================== //
// Public functions to access the clock in proper format and time zone
// ===================================================================== //

// Sets the static timezone that the data will be logged in - this must be set
void Logger::setLoggerTimeZone(int8_t timeZone)
{
    _loggerTimeZone = timeZone;
    // Some helpful prints for debugging
    #ifdef STANDARD_SERIAL_OUTPUT
        const char* prtout1 = "Logger timezone is set to UTC";
        if (_loggerTimeZone == 0)
            PRINTOUT(prtout1);
        else if (_loggerTimeZone > 0)
            PRINTOUT(prtout1, '+', _loggerTimeZone);
        else
            PRINTOUT(prtout1, _loggerTimeZone);
    #endif
}
int8_t Logger::getLoggerTimeZone(void)
{
    return Logger::_loggerTimeZone;
}
// Duplicates for backwards compatibility
void Logger::setTimeZone(int8_t timeZone) { setLoggerTimeZone(timeZone); }
int8_t Logger::getTimeZone(void) { return getLoggerTimeZone(); }

// Sets the static timezone that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setRTCTimeZone(int8_t timeZone)
{
    _loggerRTCOffset = _loggerTimeZone - timeZone;
    // Some helpful prints for debugging
    #ifdef STANDARD_SERIAL_OUTPUT
        const char* prtout1 = "RTC timezone is set to UTC";
        if ((_loggerTimeZone - _loggerRTCOffset) == 0)
            PRINTOUT(prtout1);
        else if ((_loggerTimeZone - _loggerRTCOffset) > 0)
            PRINTOUT(prtout1, '+', (_loggerTimeZone - _loggerRTCOffset));
        else
            PRINTOUT(prtout1, (_loggerTimeZone - _loggerRTCOffset));
    #endif
}
int8_t Logger::getRTCTimeZone(void)
{
    return Logger::_loggerTimeZone - Logger::_loggerRTCOffset;
}


// This set the offset between the built-in clock and the time zone where
// the data is being recorded.  If your RTC is set in UTC and your logging
// timezone is EST, this should be -5.  If your RTC is set in EST and your
// timezone is EST this does not need to be called.
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setTZOffset(int8_t offset)
{
    _loggerRTCOffset = offset;
    // Some helpful prints for debugging
    if (_loggerRTCOffset == 0)
        PRINTOUT(F("RTC and Logger are set in the same timezone."));
    else if (_loggerRTCOffset < 0)
        PRINTOUT(F("RTC is set"), -1*_loggerRTCOffset, F("hours ahead of logging timezone"));
    else
        PRINTOUT(F("RTC is set"), _loggerRTCOffset, F("hours behind the logging timezone"));
}
int8_t Logger::getTZOffset(void)
{
    return Logger::_loggerRTCOffset;
}

// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) and corrects it for the specified time zone
#if defined MS_SAMD_DS3231 || not defined ARDUINO_ARCH_SAMD
uint32_t Logger::getNowEpochT0(void)
{
  uint32_t currentEpochTime = rtc.now().getEpoch();
  return currentEpochTime;
}

uint32_t Logger::getNowEpochTz(void)
{
  uint32_t currentEpochTime = rtc.now().getEpoch();
  currentEpochTime += ((uint32_t)_loggerRTCOffset)*3600;
  return currentEpochTime;
}
uint32_t Logger::getNowEpoch(void) {return getNowEpochTz();} //Depreciated in 0.23.4, left in for compatiblity 

void Logger::setNowEpochT0(uint32_t ts){rtc.setEpoch(ts);}
void Logger::setNowEpoch(uint32_t ts){rtc.setEpoch(ts);} //Depreciated in 0.23.4, left in for compatiblity 

#elif defined ARDUINO_ARCH_SAMD

uint32_t Logger::getNowEpochT0(void)
{
  return zero_sleep_rtc.getEpoch();
}

uint32_t Logger::getNowEpochTz(void)
{
  uint32_t currentEpochTime = zero_sleep_rtc.getEpoch();
  currentEpochTime += ((uint32_t)_loggerRTCOffset)*3600;
  return currentEpochTime;
}
uint32_t Logger::getNowEpoch(void) {return getNowEpochTz();} //Depreciated in 0.23.4, left in for compatiblity 

void Logger::setNowEpochT0(uint32_t ts){zero_sleep_rtc.setEpoch(ts);}
void Logger::setNowEpoch(uint32_t ts){zero_sleep_rtc.setEpoch(ts);} //Depreciated in 0.23.4, left in for compatiblity 

#endif

// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) 
DateTime Logger::dtFromEpochT0(uint32_t epochTime)
{
    DateTime dt(epochTime);
    return dt;
}
// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) and corrects it for the specified time zone
DateTime Logger::dtFromEpochTz(uint32_t epochTime)
{
    DateTime dt(epochTime - EPOCH_TIME_OFF);
    return dt;
}
DateTime Logger::dtFromEpoch(uint32_t epochTime) {return dtFromEpochTz(epochTime);} //Depreciated
// This converts a date-time object into a ISO8601 formatted string
// It assumes the supplied date/time is in the LOGGER's timezone and adds
// the LOGGER's offset as the time zone offset in the string.
String Logger::formatDateTime_ISO8601(DateTime& dt)
{
    // Set up an inital string
    String dateTimeStr;
    // Convert the DateTime object to a String
    dt.addToString(dateTimeStr);
    dateTimeStr.replace(" ", "T");
    String tzString = String(_loggerTimeZone);
    if (-24 <= _loggerTimeZone && _loggerTimeZone <= -10)
    {
        tzString += F(":00");
    }
    else if (-10 < _loggerTimeZone && _loggerTimeZone < 0)
    {
        tzString = tzString.substring(0,1) + '0' + tzString.substring(1,2) + F(":00");
    }
    else if (_loggerTimeZone == 0)
    {
        tzString = 'Z';
    }
    else if (0 < _loggerTimeZone && _loggerTimeZone < 10)
    {
        tzString = "+0" + tzString + F(":00");
    }
    else if (10 <= _loggerTimeZone && _loggerTimeZone <= 24)
    {
        tzString = "+" + tzString + F(":00");
    }
    dateTimeStr += tzString;
    return dateTimeStr;
}


// This converts an epoch time (unix time) into a ISO8601 formatted string
// It assumes the supplied date/time is in the LOGGER's timezone and adds
// the LOGGER's offset as the time zone offset in the string.
String Logger::formatDateTime_ISO8601(uint32_t epochTimeTz)
{
    // Create a DateTime object from the epochTime
    DateTime dtTz = dtFromEpoch(epochTimeTz);
    return formatDateTime_ISO8601(dtTz);
}


// This sets the real time clock to the given time
bool Logger::setRTClock(uint32_t UTCEpochSeconds)
{
    bool retVal=false;

    // If the timestamp is zero, just exit
    if  (UTCEpochSeconds == 0)
    {
        PRINTOUT(F("Bad timestamp 0, not setting clock."));
        return false;
    }

    // The "UTCEpochSeconds" is the number of seconds since Jan 1, 1970 in UTC
    // We're interested in the UTCEpochSeconds in the logger's and RTC's timezone
    // The RTC's timezone is a label and isn't used in calculations, only the offset is used to make it more readable
    // between the logger and the RTC.
    // Only works for ARM CC if long, AVR was uint32_t
    uint32_t nistTz_sec = UTCEpochSeconds+ ((int32_t)getTZOffset())*3600;
    MS_DBG(F("    NIST Time:"), UTCEpochSeconds, \
        F("->"), formatDateTime_ISO8601(nistTz_sec));

    // Check the current RTC time
    uint32_t cur_logT0_sec = getNowEpochT0();
    uint32_t cur_logTz_sec = getNowEpochTz();
    MS_DBG(F("    Current Epoch Time on RTC :"), cur_logT0_sec, F("->"), \
        formatDateTime_ISO8601(cur_logTz_sec));
    MS_DBG(F("    Offset between epoch NIST and RTC:"), abs(cur_logT0_sec - UTCEpochSeconds));

    // If the RTC and NIST disagree by more than 5 seconds, set the clock
    #define NIST_TIME_DIFF_SEC 5
    if (abs(cur_logT0_sec - UTCEpochSeconds) > NIST_TIME_DIFF_SEC )
    {
        setNowEpochT0(UTCEpochSeconds);
        PRINTOUT(F("Internal Clock set "),formatDateTime_ISO8601(nistTz_sec));
        retVal= true;
    }
    else
    {
        PRINTOUT(F("Internal Clock within "),NIST_TIME_DIFF_SEC,F("seconds of NIST."));
        //return false;
        retVal= true;  //RTC valid
    }
    #if defined ADAFRUIT_FEATHERWING_RTC_SD
    //Check the current ExtRtc time - 
    DateTime nowExt = rtcExtPhy.now(); //T0 UST/
    uint32_t nowExtEpoch_sec =nowExt.getEpoch();
    MS_DBG("         Time Returned by rtcExt:", nowExtEpoch_sec, \
        "->(T=", getTimeZone(),")", \
        formatDateTime_ISO8601(nowExtEpoch_sec));
    if (abs(nowExtEpoch_sec - UTCEpochSeconds) > NIST_TIME_DIFF_SEC)
    {
        rtcExtPhy.setTimeEpochT0(UTCEpochSeconds);
        MS_DBG("         rtcExt updated to UTS ",  UTCEpochSeconds,"->",formatDateTime_ISO8601(UTCEpochSeconds));
        retVal= true;
    }

    #endif //ADAFRUIT_FEATHERWING_RTC_SD
    return retVal;
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
    #if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
    uint32_t checkTime = getNowEpoch();
    MS_DBG(F("Current Unix Timestamp:"), checkTime, F("->"), \
        formatDateTime_ISO8601(checkTime));
    MS_DBG(F("Logging interval in seconds:"), (_loggingIntervalMinutes*60));
    MS_DBG(F("Mod of Logging Interval:"), checkTime % (_loggingIntervalMinutes*60));

    if (checkTime % (_loggingIntervalMinutes*60) == 0)
    {
        // Update the time variables with the current time
        markTime();
        MS_DBG(F("Time marked at (unix):"), Logger::markedEpochTime);
        MS_DBG(F("Time to log!"));
        retval = true;
    }
    else
    {
        MS_DBG(F("Not time yet."));
        retval = false;
    }
    #else  // ARDUINO_ARCH_SAMD
    //Assume that have slept for the right amount of time
    markTime();
    MS_DBG(F("Logging epoch time marked:"), Logger::markedEpochTime," ",Logger::formatDateTime_ISO8601(Logger::markedEpochTime));
    retval = true;
    #endif 
    return retval;
}


// This checks to see if the MARKED time is an even interval of the logging rate
// or we're in the first 15 minutes of logging
bool Logger::checkMarkedInterval(void)
{
    bool retval;
    MS_DBG(F("Marked Time:"), Logger::markedEpochTime,
           F("Logging interval in seconds:"), (_loggingIntervalMinutes*60),
           F("Mod of Logging Interval:"), Logger::markedEpochTime % (_loggingIntervalMinutes*60));

    if (Logger::markedEpochTime != 0 &&
        (Logger::markedEpochTime % (_loggingIntervalMinutes*60) == 0))
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
    // MS_DBG(F("\nClock interrupt!"));
}


// Puts the system to sleep to conserve battery life.
// This DOES NOT sleep or wake the sensors!!
static uint32_t wakeUpTime_secs;
void Logger::systemSleep(uint8_t sleep_min)
{

    #if defined MS_SAMD_DS3231 || not defined ARDUINO_ARCH_SAMD

    // Don't go to sleep unless there's a wake pin!
    if (_mcuWakePin < 0)
    {
        MS_DBG(F("Use a non-negative wake pin to request sleep!"));
        return;
    }
    // Unfortunately, because of the way the alarm on the DS3231 is set up, it
    // cannot interrupt on any frequencies other than every second, minute,
    // hour, day, or date.  We could set it to alarm hourly every 5 minutes past
    // the hour, but not every 5 minutes.  This is why we set the alarm for
    // every minute and use the checkInterval function.  This is a hardware
    // limitation of the DS3231; it is not due to the libraries or software.
    MS_DBG(F("Setting alarm on DS3231 RTC for every minute."));
    rtc.enableInterrupts(EveryMinute);

    // Clear the last interrupt flag in the RTC status register
    // The next timed interrupt will not be sent until this is cleared
    rtc.clearINTStatus();

    // Set up a pin to hear clock interrupt and attach the wake ISR to it
    pinMode(_mcuWakePin, INPUT_PULLUP);
    enableInterrupt(_mcuWakePin, wakeISR, CHANGE);

    #elif defined ARDUINO_ARCH_SAMD

    // Make sure interrupts are enabled for the clock
    NVIC_EnableIRQ(RTC_IRQn);  // enable RTC interrupt
    NVIC_SetPriority(RTC_IRQn, 0);  // highest priority

    // Alarms on the RTC built into the SAMD21 appear to be identical to those
    // in the DS3231.  See more notes below.
    // We're setting the alarm seconds to 59 and then seting it to go off
    // whenever the seconds match the 59.  I'm using 59 instead of 00
    // because there seems to be a bit of a wake-up delay
    uint16_t local_secs;
    uint32_t targetWakeup_secs;
    uint32_t timeNow_secs;
    uint32_t adjust_secs;
    if (0 == sleep_min) {
        local_secs = (_loggingIntervalMinutes*60);
    } else {
        local_secs = (sleep_min*60);
    }
    //zero_sleep_rtc.setAlarmSeconds(local_secs);
    timeNow_secs= getNowEpoch();
    targetWakeup_secs = timeNow_secs+local_secs;
    adjust_secs = targetWakeup_secs%60;
    targetWakeup_secs -= adjust_secs;
    MS_DBG("Setting alarm (",local_secs,"+",timeNow_secs,") on RTC @",targetWakeup_secs," ",formatDateTime_ISO8601(targetWakeup_secs),\
    " adj=",adjust_secs," fm now=",timeNow_secs ," Awake=",timeNow_secs-wakeUpTime_secs);
    zero_sleep_rtc.setAlarmEpoch(targetWakeup_secs);
 #define zsr zero_sleep_rtc
    MS_DBG("Alm:",zsr.getAlarmYear(),zsr.getAlarmMonth(),zsr.getAlarmDay(),"-",zsr.getAlarmHours(),":",zsr.getAlarmMinutes(),":",zsr.getAlarmSeconds());
    // Assume max is an hour - need to revisit
    zero_sleep_rtc.enableAlarm(zero_sleep_rtc.MATCH_MMSS);
    #endif

    // Send one last message before shutting down serial ports
    MS_DBG(F("Putting processor to sleep.  ZZzzz..."));

    // Wait until the serial ports have finished transmitting
    // This does not clear their buffers, it just waits until they are finished
    // TODO:  Make sure can find all serial ports
    #if defined(STANDARD_SERIAL_OUTPUT)
        STANDARD_SERIAL_OUTPUT.flush();  // for debugging
    #endif
    #if defined DEBUGGING_SERIAL_OUTPUT
        DEBUGGING_SERIAL_OUTPUT.flush();  // for debugging
    #endif

    // Stop any I2C connections
    // This function actually disables the two-wire pin functionality and
    // turns off the internal pull-up resistors.
    Wire.end();
    // Now force the I2C pins to LOW
    // I2C devices have a nasty habit of stealing power from the SCL and SDA pins...
    // This will only work for the "main" I2C/TWI interface
    #ifdef SDA
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, LOW);
    #endif
    #ifdef SCL
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, LOW);
    #endif

    #if defined ARDUINO_ARCH_SAMD

    // Disable the watch-dog timer
    watchDogTimer.disableWatchDog();

    // Sleep code from ArduinoLowPowerClass::sleep()
    bool restoreUSBDevice = false;
	// if (SERIAL_PORT_USBVIRTUAL)
    // {
	// 	USBDevice.standby();
	// }
    // else
    // {
        #ifndef USE_TINYUSB
		USBDevice.detach();
        #endif
		restoreUSBDevice = true;
	// }
	// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    // Now go to sleep
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__DSB();
    __WFI();

    #elif defined ARDUINO_ARCH_AVR

    // Set the sleep mode
    // In the avr/sleep.h file, the call names of these 5 sleep modes are:
    // SLEEP_MODE_IDLE         -the least power savings
    // SLEEP_MODE_ADC
    // SLEEP_MODE_PWR_SAVE
    // SLEEP_MODE_STANDBY
    // SLEEP_MODE_PWR_DOWN     -the most power savings
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Disable the watch-dog timer
    watchDogTimer.disableWatchDog();

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

    #endif
    // ---------------------------------------------------------------------


    // ---------------------------------------------------------------------
    // -- The portion below this happens on wake up, after any wake ISR's --

    #if defined ARDUINO_ARCH_SAMD
    // Reattach the USB after waking
	// Enable systick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	if (restoreUSBDevice)
    {
        #ifndef USE_TINYUSB
		USBDevice.attach();
        #endif
        uint32_t startTimer = millis();
        while (!SERIAL_PORT_USBVIRTUAL && ((millis()- startTimer) < 1000L)){}
    }
    #endif

    #if defined ARDUINO_ARCH_AVR

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

    #endif

    // Re-enable the watch-dog timer
    watchDogTimer.enableWatchDog();

    // Re-start the I2C interface
    #ifdef SDA
    pinMode(SDA, INPUT_PULLUP);  // set as input with the pull-up on
    #endif
    #ifdef SCL
    pinMode(SCL, INPUT_PULLUP);
    #endif
    Wire.begin();
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);

    #if defined MS_SAMD_DS3231 || not defined ARDUINO_ARCH_SAMD
    // Stop the clock from sending out any interrupts while we're awake.
    // There's no reason to waste thought on the clock interrupt if it
    // happens while the processor is awake and doing other things.
    rtc.disableInterrupts();
    // Detach the from the pin
    disableInterrupt(_mcuWakePin);

    #elif defined ARDUINO_ARCH_SAMD
    zero_sleep_rtc.disableAlarm();
    #endif

    // Wake-up message
    wakeUpTime_secs = getNowEpochTz();
    MS_DBG(F("\n\n\n... zzzZZ Processor awake @"),wakeUpTime_secs);

    // The logger will now start the next function after the systemSleep
    // function in either the loop or setup
}


// ===================================================================== //
// Public functions for logging data to an SD card
// ===================================================================== //

// This sets a file name, if you want to decide on it in advance
void Logger::setFileName(String& fileName)
{
    _fileName = fileName;
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
    // Generate the file name from logger ID and date
    String fileName =  String(_loggerID);
    fileName +=  "_";
    fileName +=  formatDateTime_ISO8601(getNowEpoch()).substring(0, 10);
    fileName +=  ".csv";
    setFileName(fileName);
    _fileName = fileName;
}


// This is a PRE-PROCESSOR MACRO to speed up generating header rows
// Again, THIS IS NOT A FUNCTION, it is a pre-processor macro
#define STREAM_CSV_ROW(firstCol, function) \
    stream->print("\""); \
    stream->print(firstCol); \
    stream->print("\","); \
    for (uint8_t i = 0; i < getArrayVarCount(); i++) \
    { \
        stream->print("\""); \
        stream->print(function); \
        stream->print("\""); \
        if (i + 1 != getArrayVarCount()) \
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

    // Adding the sampling feature UUID (only applies to EnviroDIY logger)
    if (strlen(_samplingFeatureUUID) > 1)
    {
        stream->print(F("Sampling Feature UUID: "));
        // stream->println(_samplingFeatureUUID);
        stream->print(_samplingFeatureUUID);
        stream->println(',');
    }

    // Next line will be the parent sensor names
    STREAM_CSV_ROW(F("Sensor Name:"), getParentSensorNameAtI(i))
    // Next comes the ODM2 variable name
    STREAM_CSV_ROW(F("Variable Name:"), getVarNameAtI(i))
    // Next comes the ODM2 unit name
    STREAM_CSV_ROW(F("Result Unit:"), getVarUnitAtI(i))
    // Next comes the variable UUIDs
    // We'll only add UUID's if we see a UUID for the first variable
    if (getVarUUIDAtI(0).length() > 1)
    {
        STREAM_CSV_ROW(F("Result UUID:"), getVarUUIDAtI(i))
    }

    // We'll finish up the the custom variable codes
    String dtRowHeader = F("Date and Time in UTC");
    if (_loggerTimeZone > 0) dtRowHeader += '+' + _loggerTimeZone;
    else if (_loggerTimeZone < 0) dtRowHeader += _loggerTimeZone;
    STREAM_CSV_ROW(dtRowHeader, getVarCodeAtI(i));
}


// This prints a comma separated list of volues of sensor data - including the
// time -  out over an Arduino stream
void Logger::printSensorDataCSV(Stream *stream)
{
    String csvString = "";
    dtFromEpoch(Logger::markedEpochTime).addToString(csvString);
    csvString += ',';
    stream->print(csvString);
    for (uint8_t i = 0; i < getArrayVarCount(); i++)
    {
        stream->print(getValueStringAtI(i));
        if (i + 1 != getArrayVarCount())
        {
            stream->print(',');
        }
    }
    stream->println();
}

// Protected helper function - This checks if the SD card is available and ready
bool Logger::initializeSDCard(void)
{
    // If we don't know the slave select of the sd card, we can't use it
    if (_SDCardSSPin < 0)
    {
        PRINTOUT(F("Slave/Chip select pin for SD card has not been set."));
        PRINTOUT(F("Data will not be saved!"));
        return false;
    }
    // Initialise the SD card
    if (!sd.begin(_SDCardSSPin, SPI_FULL_SPEED))
    {
        PRINTOUT(F("Error: SD card failed to initialize or is missing."));
        PRINTOUT(F("Data will not be saved!"));
        return false;
    }
    else  // skip everything else if there's no SD card, otherwise it might hang
    {
        MS_DBG(F("Successfully connected to SD Card with card/slave select on pin"),
                 _SDCardSSPin);
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
        MS_DBG(F("Opened existing file:"), filename);
        // Set access date time
        setFileTimestamp(logFile, T_ACCESS);
        return true;
    }
    else if (createFile)
    {
        // Create and then open the file in write mode
        if (logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END))
        {
            MS_DBG(F("Created new file:"), filename);
            // Set creation date time
            setFileTimestamp(logFile, T_CREATE);
            // Write out a header, if requested
            if (writeDefaultHeader)
            {
                // Add header information
                printFileHeader(&logFile);
                // Print out the header for debugging
                #if defined DEBUGGING_SERIAL_OUTPUT
                    MS_DBG(F("\n \\/---- File Header ----\\/"));
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
            MS_DBG(F("Unable to create new file:"), filename);
            return false;
        }
    }
    // Return false if we couldn't access the file (and were not told to create it)
    else
    {
        MS_DBG(F("Unable to to write to file:"), filename);
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
        // logFile.sync();
        logFile.close();
        PRINTOUT(F("Data will be saved as"), _fileName);
        return true;
    }
    else
    {
        PRINTOUT(F("Unable to create a file to save data to!"));
        return false;
    }
}
bool Logger::createLogFile(bool writeDefaultHeader)
{
    if (_fileName = "") generateAutoFileName();
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
    PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/"));
    PRINTOUT(rec);

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
    // Close the file to save it
    // logFile.sync();
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
        if (_fileName = "") generateAutoFileName();
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
        PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/"));
        printSensorDataCSV(&STANDARD_SERIAL_OUTPUT);
        PRINTOUT('\n');
    #endif

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
    // Close the file to save it
    // logFile.sync();
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

    // Power up the modem
    if (_logModem != NULL) _logModem->modemPowerUp();

    // Power up all of the sensors
    _internalArray->sensorsPowerUp();

    // Wake up all of the sensors
    _internalArray->sensorsWake();

    // Update the sensors and print out data 25 times
    for (uint8_t i = 0; i < 25; i++)
    {
        PRINTOUT(F("------------------------------------------"));
        watchDogTimer.resetWatchDog();
        // Update the values from all attached sensors
        // NOTE:  NOT using complete update because we want everything left
        // on between iterations in testing mode.
        _internalArray->updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is"), formatDateTime_ISO8601(getNowEpoch()));
        PRINTOUT(F("-----------------------"));
        // Print out the sensor data
        #if defined(STANDARD_SERIAL_OUTPUT)
            _internalArray->printSensorData(&STANDARD_SERIAL_OUTPUT);
        #endif
        PRINTOUT(F("-----------------------"));
        watchDogTimer.resetWatchDog();

        delay(5000);
    }

    // Put sensors to sleep
    _internalArray->sensorsSleep();
    _internalArray->sensorsPowerDown();

    // Turn the modem off
    _logModem->modemSleepPowerDown();

    PRINTOUT(F("Exiting testing mode"));
    PRINTOUT(F("------------------------------------------"));
    watchDogTimer.resetWatchDog();

    // Unset testing mode flag
    Logger::isTestingNow = false;

    // Sleep
    systemSleep();
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This does all of the setup that can't happen in the constructors
// That is, things that require the actual processor/MCU to do something
// rather than the compiler to do something.
void Logger::begin(const char *loggerID, uint16_t loggingIntervalMinutes,
                   VariableArray *inputArray)
{
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    begin(inputArray);
}
void Logger::begin(VariableArray *inputArray)
{
    setVariableArray(inputArray);
    begin();
}
void Logger::begin()
{
    MS_DBG(F("Logger ID is:"), _loggerID);
    MS_DBG(F("Logger is set to record at"),
           _loggingIntervalMinutes, F("minute intervals."));

    MS_DBG(F("Setting up a watch-dog timer to fire after 5 minutes of inactivity"));
    // watchDogTimer.setupWatchDog(((uint32_t)_loggingIntervalMinutes)*60*3);
    watchDogTimer.setupWatchDog((uint32_t)(5*60*3));
    // Enable the watchdog
    watchDogTimer.enableWatchDog();

    // Set pin modes for sd card power
    if (_SDCardPowerPin >= 0)
    {
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, LOW);
        MS_DBG(F("Pin"), _SDCardPowerPin, F("set as SD Card Power Pin"));
    }
    // Set pin modes for sd card slave select (aka chip select)
    if (_SDCardSSPin >= 0)
    {
        pinMode(_SDCardSSPin, OUTPUT);
        MS_DBG(F("Pin"), _SDCardSSPin, F("set as SD Card Slave/Chip Select"));
    }
    // Set pin mode for LED pin
    if (_ledPin >= 0 )
    {
        pinMode(_ledPin, OUTPUT);
        MS_DBG(F("Pin"), _ledPin, F("set as LED alert pin"));
    }
    if (_buttonPin >= 0)
    {
        pinMode(_buttonPin, INPUT_PULLUP);
        enableInterrupt(_buttonPin, Logger::testingISR, CHANGE);
        MS_DBG(F("Button on pin"), _buttonPin,
               F("can be used to enter sensor testing mode."));
    }

    #if defined ARDUINO_ARCH_SAMD
        MS_DBG(F("Beginning internal real time clock"));
        zero_sleep_rtc.begin();
    #endif
    watchDogTimer.resetWatchDog();

    // Set the pins for I2C
    MS_DBG(F("Setting I2C Pins to INPUT_PULLUP"));
    #ifdef SDA
    pinMode(SDA, INPUT_PULLUP);  // set as input with the pull-up on
    #endif
    #ifdef SCL
    pinMode(SCL, INPUT_PULLUP);
    #endif
    MS_DBG(F("Beginning wire (I2C)"));
    Wire.begin();
    watchDogTimer.resetWatchDog();

    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);

    #if defined MS_SAMD_DS3231 || not defined ARDUINO_ARCH_SAMD
        if (_mcuWakePin < 0)
        {
            MS_DBG(F("Logger mcu will not sleep between readings!"));
        }
        else
        {
            pinMode(_mcuWakePin, INPUT_PULLUP);
            MS_DBG(F("Pin"), _mcuWakePin, F("set as RTC wake up pin"));
        }
        MS_DBG(F("Beginning DS3231 real time clock"));
        rtc.begin();
    #endif
    #if defined ARDUINO_ARCH_SAMD
        /* Work in progress
         * Int RTCZero     class Time relative to 2000 TZ
         * Ext RTC_PCF8523 class Time relative to 2000 UTS/GMT/TZ0
         */

        //eg Apr 22 2019 16:46:09 in this TZ
        DateTime ccTimeTZ(__DATE__, __TIME__);
        //MS_DBG("now  ",ccTimeTZ.month(),"-",ccTimeTZ.date(),"-",ccTimeTZ.year2k(),"/",ccTimeTZ.year(), " ",ccTimeTZ.hour(),":",ccTimeTZ.minute(),":",ccTimeTZ.second());
        MS_DBG("Sw Build Time  ",ccTimeTZ.month(),"-",ccTimeTZ.date(),"-",ccTimeTZ.year(), " ",ccTimeTZ.hour(),":",ccTimeTZ.minute(),":",ccTimeTZ.second());

        #if defined ADAFRUIT_FEATHERWING_RTC_SD
        DateTime ccTime2k=ccTimeTZ.get()-(getTimeZone()*3600); //set to secs from UST/GMT Year 2000
        rtcExtPhy.begin();
        if (! rtcExtPhy.initialized())
        {
            MS_DBG("ExtRTC !init set to compile time ",__DATE__," ",__TIME__);
            rtcExtPhy.adjust(ccTime2k);
        } else {
            DateTime now = rtcExtPhy.now();
            if (now.getY2k_secs() < ccTime2k.getY2k_secs()) {
                MS_DBG("ExtRTC invalid   ",ccTime2k.month(),"-",ccTime2k.date(),"-",ccTime2k.year2k(),"/",ccTime2k.year(), " ",ccTime2k.hour(),":",ccTime2k.minute(),":",ccTime2k.second(), ". Set to compile time ",__DATE__," ",__TIME__);
                rtcExtPhy.adjust(ccTime2k);
            }
        }
        
    watchDogTimer.resetWatchDog();

        DateTime now = rtcExtPhy.now();
        MS_DBG("Set internal rtc from ext rtc ",now.year(),"-",now.month(),"-",now.date()," ",now.hour(),":",now.minute(),":",now.second());
        zero_sleep_rtc.setTime(now.hour(), now.minute(), now.second());
        zero_sleep_rtc.setDate(now.date(), now.month(), now.year2k());
        #define zr zero_sleep_rtc
        MS_DBG("Read internal rtc ",zr.getYear(),"-",zr.getMonth(),"-",zr.getDay()," ",zr.getHours(),":",zr.getMinutes(),":",zr.getSeconds());
        #else // no ADAFRUIT_FEATHERWING_RTC_SD
        // If Power-on Reset Rcause.Bit0 have specific processing
        #define zr zero_sleep_rtc
        if ( (0== zr.getYear()) && (1==zr.getMonth()) && (1==zr.getDay()) ) {
            MS_DBG("RTC.setDay to 2 for Power-On Reset case ");
            zr.setDay(2); // Allow for calcs for -11hrs
        }
        #endif // ADAFRUIT_FEATHERWING_RTC_SD
    #endif //ARDUINO_ARCH_SAMD

    MS_DBG(F("Current RTC time is:"), formatDateTime_ISO8601(getNowEpochTz()));
    // Reset the watchdog
    watchDogTimer.resetWatchDog();

    // Begin the internal array
    _internalArray->begin();
    PRINTOUT(F("This logger has a variable array with"),
         getArrayVarCount(), F("variables, of which"),
         getArrayVarCount() - _internalArray->getCalculatedVariableCount(),
         F("come from"), _internalArray->getSensorCount(), F("sensors and"),
         _internalArray->getCalculatedVariableCount(), F("are calculated."));

    if (_samplingFeatureUUID != NULL)
    {
        MS_DBG(F("Sampling feature UUID is:"), _samplingFeatureUUID);
    }

    PRINTOUT(F("Logger portion of setup finished."));
}


// This is a one-and-done to log data
void Logger::logData(void)
{
    // Reset the watchdog
    watchDogTimer.resetWatchDog();

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        watchDogTimer.resetWatchDog();

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        alertOn();
        // Power up the SD Card
        // TODO:  Decide how much delay is needed between turning on the card
        // and writing to it.  Could we turn it on just before writing?
        turnOnSDcard(false);

        // Do a complete sensor update
        MS_DBG(F("    Running a complete sensor update..."));
        watchDogTimer.resetWatchDog();
        _internalArray->completeUpdate();
        watchDogTimer.resetWatchDog();

        // Create a csv data record and save it to the log file
        logToSD();
        // Cut power from the SD card, waiting for housekeeping
        turnOffSDcard(true);

        // Turn off the LED
        alertOff();
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Sleep
    systemSleep();
}
// This is a one-and-done to log data
void Logger::logDataAndPublish(void)
{
    // Reset the watchdog
    watchDogTimer.resetWatchDog();

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        watchDogTimer.resetWatchDog();

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        alertOn();
        // Power up the SD Card
        // TODO:  Decide how much delay is needed between turning on the card
        // and writing to it.  Could we turn it on just before writing?
        turnOnSDcard(false);

        // Turn on the modem to let it start searching for the network
        if (_logModem != NULL) _logModem->modemPowerUp();

        // Do a complete update on the variable array.
        // This this includes powering all of the sensors, getting updated
        // values, and turing them back off.
        // NOTE:  The wake function for each sensor should force sensor setup
        // to run if the sensor was not previously set up.
        MS_DBG(F("Running a complete sensor update..."));
        watchDogTimer.resetWatchDog();
        _internalArray->completeUpdate();
        watchDogTimer.resetWatchDog();

        // Create a csv data record and save it to the log file
        logToSD();

        if (_logModem != NULL)
        {
            // Connect to the network
            MS_DBG(F("Connecting to the Internet..."));
            if (_logModem->connectInternet())
            {
                // Publish data to remotes
                watchDogTimer.resetWatchDog();
                publishDataToRemotes();
                watchDogTimer.resetWatchDog();

                // Sync the clock at midnight
                #define NIST_SYNC_DAY 86400
                #define NIST_SYNC_HR 3600
                #define NIST_SYNC_RATE NIST_SYNC_HR
#if 0
                syncTimeCheck_normalized = Logger::markedEpochTime/logIntvl_sec;
                syncTimeCheck_remainder = syncTimeCheck_normalized %(NIST_SYNC_RATE/logIntvl_sec);
                MS_DBG(F("SyncTimeCheck "),syncTimeCheck_remainder," Rate",logIntvl_sec," Time",Logger::markedEpochTime);
                if (Logger::markedEpochTime != 0 && syncTimeCheck_remainder == 0)
#endif
                //if (Logger::markedEpochTime != 0 && Logger::markedEpochTime % 86400 == 0)

                if (Logger::markedEpochTime != 0 && Logger::markedEpochTime % 86400 == 43200)
                // Sync the clock at noon
                {
                    MS_DBG(F("Running a daily clock sync..."));
                    setRTClock(_logModem->getNISTTime());
                    watchDogTimer.resetWatchDog();
                }

                // Disconnect from the network - ehh, why bother
                // MS_DBG(F("Disconnecting from the Internet..."));
                // _logModem->disconnectInternet();
            }
            else
            {
                MS_DBG(F("Could not connect to the internet!"));
                watchDogTimer.resetWatchDog();
            }
            // Turn the modem off
            _logModem->modemSleepPowerDown();
        }


        // TODO:  Do some sort of verification that minimum 1 sec has passed
        // for internal SD card housekeeping before cutting power
        // It seems very unlikely based on my testing that less than one second
        // would be taken up in publishing data to remotes
        // Cut power from the SD card - without additional housekeeping wait
        turnOffSDcard(false);

        // Turn off the LED
        alertOff();
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Call the processor sleep
    systemSleep();
}
#include "LoggerBaseExtCpp.h"
//End of LoggerBase.cpp


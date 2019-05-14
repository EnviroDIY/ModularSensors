/*
 *EspressifESP8266.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file SHOULD work for essentially any breakout of the Espressif ESP8266
 *wifi chip as long as the chip has been flashed with Espressif's AT command
 *firmware.
*/

// Included Dependencies
#include "EspressifESP8266.h"
#include "modems/LoggerModemMacros.h"


// Constructor
EspressifESP8266::EspressifESP8266(Stream* modemStream,
                                   int8_t powerPin, int8_t statusPin,
                                   int8_t modemResetPin, int8_t modemSleepRqPin,
                                   const char *ssid, const char *pwd,
                                   uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin,
                ESP8266_STATUS_TIME_MS, ESP8266_DISCONNECT_TIME_MS,
                ESP8266_WARM_UP_TIME_MS, ESP8266_ATRESPONSE_TIME_MS,
                ESP8266_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    _tinyModem(*modemStream)
{
    _ssid = ssid;
    _pwd = pwd;
    TinyGsmClient *tinyClient = new TinyGsmClient(_tinyModem);
    _tinyClient = tinyClient;
    _modemStream = modemStream;
}


MS_MODEM_DID_AT_RESPOND(EspressifESP8266);
MS_MODEM_IS_INTERNET_AVAILABLE(EspressifESP8266);
MS_MODEM_IS_MEASUREMENT_COMPLETE(EspressifESP8266);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(EspressifESP8266);
MS_MODEM_CONNECT_INTERNET(EspressifESP8266);
MS_MODEM_DISCONNECT_INTERNET(EspressifESP8266);
MS_MODEM_GET_NIST_TIME(EspressifESP8266);


// A helper function to wait for the esp to boot and immediately change some settings
// We'll use this in the wake function
bool EspressifESP8266::ESPwaitForBoot(void)
{
    // Wait for boot - finished when characters start coming
    // NOTE: After every "hard" reset (either power off or via RST-B), the ESP
    // sends out a boot log from the ROM on UART1 at 74880 baud.  We're not
    // going to worry about the odd baud rate since we're simply throwing the
    // characters away.
    delay(200);  // It will take at least this long
    uint32_t start = millis();
    bool success = false;
    while (!_modemStream->available() && millis() - start < 1000) {}
    if (_modemStream->available())
    {
        success = true;
        // Read the boot log to empty it from the serial buffer
        while (_modemStream->available())
        {
            _modemStream->read();
            delay(2);
        }
        // Have to make sure echo is off or all AT commands will be confused
        _tinyModem.sendAT(F("E0"));
        success &= _tinyModem.waitResponse() == 1;
        // re-run init to set mux and station mode
        success &= _tinyModem.init();
    }
    return success;
}


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool EspressifESP8266::modemWakeFxn(void)
{
    bool success = true;
    if (_powerPin >= 0)  // Turns on when power is applied
    {
        success &= ESPwaitForBoot();
        return success;
    }
    else if (_modemResetPin >= 0)
    {
        digitalWrite(_modemResetPin, LOW);
        delay(1);
        digitalWrite(_modemResetPin, HIGH);
        success &= ESPwaitForBoot();
        return success;
    }
    else if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        delay(1);
        digitalWrite(_modemSleepRqPin, HIGH);
        // Don't have to wait for a boot if using light sleep
        return true;
    }
    else
    {
        return true;
    }
}


bool EspressifESP8266::modemSleepFxn(void)
{
    // Use this if you have GPIO16 connected to the reset pin to wake from deep sleep
    // but no other MCU pin connected to the reset pin.
    // NOTE:  This will NOT work nicely with "testingMode"
    /*if (loggingInterval > 1)
    {
        uint32_t sleepSeconds = (((uint32_t)loggingInterval) * 60 * 1000) - 75000L;
        String sleepCommand = String(sleepSeconds);
        _tinyModem.sendAT(F("+GSLP="), sleepCommand);
        // Power down for 1 minute less than logging interval
        // Better:  Calculate length of loop and power down for logging interval - loop time
        return _tinyModem.waitResponse() == 1;
    }*/
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake from deep sleep
    if (_modemResetPin >= 0)
    {
        return _tinyModem.poweroff();
    }
    // Use this if you don't have access to the ESP8266's reset pin for deep sleep but you
    // do have access to another GPIO pin for light sleep.  This also sets up another
    // pin to view the sleep status.
    else if (_modemSleepRqPin >= 0 && _dataPin >= 0)
    {
        _tinyModem.sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), ',', _statusLevel);
        bool success = _tinyModem.waitResponse() == 1;
        _tinyModem.sendAT(F("+SLEEP=1"));
        success &= _tinyModem.waitResponse() == 1;
        return success;
    }
    // Light sleep without the status pin
    else if (_modemSleepRqPin >= 0 && _dataPin < 0)
    {
        _tinyModem.sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0"));
        bool success = _tinyModem.waitResponse() == 1;
        _tinyModem.sendAT(F("+SLEEP=1"));
        success &= _tinyModem.waitResponse() == 1;
        return success;
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


// Set up the light-sleep status pin, if applicable
bool EspressifESP8266::extraModemSetup(void)
{
    _modemName = _tinyModem.getModemName();
    // Slow down the baud rate for slow processors - and save the change to
    // the ESP's non-volatile memory so we don't have to do it every time
    // #if F_CPU == 8000000L
    // if (modemBaud > 57600)
    // {
    //     _modemStream->begin(modemBaud);
    //     _tinyModem.sendAT(F("+UART_DEF=9600,8,1,0,0"));
    //     _tinyModem.waitResponse();
    //     _modemStream->end();
    //     _modemStream->begin(9600);
    // }
    // #endif
    if (_powerPin < 0 && _modemResetPin < 0 && _modemSleepRqPin >= 0 && _dataPin >= 0)
    {
        _tinyModem.sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), ',', _statusLevel);
        _tinyModem.waitResponse();
    }
    return true;
}


bool EspressifESP8266::startSingleMeasurement(void)
{
    bool success = true;
    MS_DBG(F("Starting measurement on"), getSensorName());
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an attempt was made.
    _sensorStatus |= 0b00100000;

    // Check if the modem was successfully awoken (bit 4 set)
    // Only mark the measurement request time if it is
    if (bitRead(_sensorStatus, 4))
    {
        // check if the modem was successfully set up, run set up if not
        if (!bitRead(_sensorStatus, 0))
        {
            MS_DBG(getSensorName(), F("was never properly set up, attempting setup now!"));
            setup();
        }

        // For the wifi modems, the SSID and password need to be set before they
        // can join a network.
        success &= _tinyModem.networkConnect(_ssid, _pwd);

        // Mark the time that a measurement was requested
        _millisMeasurementRequested = millis();
        // Set the status bit for measurement start success (bit 6)
        _sensorStatus |= 0b01000000;
    }
    // Otherwise, make sure that the measurement start time and success bit (bit 6) are unset
    else
    {
        MS_DBG(getSensorNameAndLocation(),
               F("isn't awake/active!  A measurement cannot be started."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
        success = false;
    }
    return success;
}

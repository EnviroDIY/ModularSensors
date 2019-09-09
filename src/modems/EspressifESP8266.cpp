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
                                   uint8_t measurementsToAverage,
                                   int8_t espSleepRqPin, int8_t espStatusPin)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin, true,
                ESP8266_STATUS_TIME_MS, ESP8266_DISCONNECT_TIME_MS,
                ESP8266_WARM_UP_TIME_MS, ESP8266_ATRESPONSE_TIME_MS,
                ESP8266_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _ssid = ssid;
    _pwd = pwd;

    _espSleepRqPin = espSleepRqPin;
    _espStatusPin = espStatusPin;

    _modemStream = modemStream;
}


// Destructor
EspressifESP8266::~EspressifESP8266(){}


MS_MODEM_DID_AT_RESPOND(EspressifESP8266);
MS_MODEM_IS_INTERNET_AVAILABLE(EspressifESP8266);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(EspressifESP8266);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(EspressifESP8266);
MS_MODEM_GET_MODEM_BATTERY_NA(EspressifESP8266);
MS_MODEM_GET_MODEM_TEMPERATURE_NA(EspressifESP8266);
MS_MODEM_CONNECT_INTERNET(EspressifESP8266);
MS_MODEM_DISCONNECT_INTERNET(EspressifESP8266);


// A helper function to wait for the esp to boot and immediately change some settings
// We'll use this in the wake function
bool EspressifESP8266::ESPwaitForBoot(void)
{
    // Wait for boot - finished when characters start coming
    // NOTE: After every "hard" reset (either power off or via RST-B), the ESP
    // sends out a boot log from the ROM on UART1 at 74880 baud.  We're not
    // going to worry about the odd baud rate since we're simply throwing the
    // characters away.
    MS_DBG(F("Waiting for boot-up message from ESP8266"));
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
        gsmModem.sendAT(GF("E0"));
        gsmModem.waitResponse();  // Will return "ERROR" if echo wasn't on
        // re-run init to set mux and station mode
        success &= gsmModem.init();
        gsmClient.init(&gsmModem);
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
        digitalWrite(_modemSleepRqPin, HIGH);
        success &= ESPwaitForBoot();
        return success;
    }
    else if (_modemResetPin >= 0)
    {
        MS_DBG(F("Sending a reset pulse to pin"), _modemResetPin,
               F("to wake ESP8266 from deep sleep"));
        digitalWrite(_modemResetPin, LOW);
        delay(1);
        digitalWrite(_modemResetPin, HIGH);
        digitalWrite(_modemSleepRqPin, HIGH);
        success &= ESPwaitForBoot();
        return success;
    }
    else if (_modemSleepRqPin >= 0)
    {
        MS_DBG(F("Sending pin"), _modemSleepRqPin,
               F("LOW to wake ESP8266 from light sleep"));
        digitalWrite(_modemSleepRqPin, LOW);
        // Have to make sure echo is off or all AT commands will be confused
        gsmModem.sendAT(GF("E0"));
        success &= gsmModem.waitResponse() == 1;
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
        gsmModem.sendAT(GF("+GSLP="), sleepCommand);
        // Power down for 1 minute less than logging interval
        // Better:  Calculate length of loop and power down for logging interval - loop time
        return gsmModem.waitResponse() == 1;
    }*/
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake from deep sleep
    // We'll also put it in deep sleep before yanking power
    if (_modemResetPin >= 0 || _powerPin >= 0)
    {
        MS_DBG(F("Requesting deep sleep for ESP8266"));
        return gsmModem.poweroff();
    }
    // Use this if you don't have access to the ESP8266's reset pin for deep sleep but you
    // do have access to another GPIO pin for light sleep.  This also sets up another
    // pin to view the sleep status.
    // AT+WAKEUPGPIO=<enable>,<trigger_GPIO>,<trigger_level>[,<awake_GPIO>,<awake_level>]
    // <enable>
    //   1: ESP8266 can be woken up from light-sleep by GPIO.
    // <trigger_GPIO>
    //   Sets the GPIO to wake ESP8266 up; range of value: [0, 15].
    // <trigger_level>
    //   0: The GPIO wakes up ESP8266 on low level.
    // [<awake_GPIO>]
    //   Optional; this parameter is used to set a GPIO as a flag of ESP8266’s
    //   being awoken form Light-sleep; range of value: [0, 15].
    // [<awake_level>]
    //   Optional;
    //   0: The GPIO is set to be low level after the wakeup process.
    //   1: The GPIO is set to be high level after the wakeup process.
    // After being woken up by <trigger_GPIO> from Light-sleep, when the ESP8266
    // attempts to sleep again, it will check the status of the <trigger_GPIO>:
    // - if it is still in the wakeup status, the EP8266 will enter Modem-sleep mode instead;
    // - if it is NOT in the wakeup status, the ESP8266 will enter Light-sleep mode.
    else if (_modemSleepRqPin >= 0 && _dataPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, HIGH);
        MS_DBG(F("Requesting light sleep for ESP8266 with status indication"));
        gsmModem.sendAT(GF("+WAKEUPGPIO=1,"), String(_espSleepRqPin), F(",0,"),
                          String(_espStatusPin), ',', _statusLevel);
        bool success = gsmModem.waitResponse() == 1;
        gsmModem.sendAT(GF("+SLEEP=1"));
        success &= gsmModem.waitResponse() == 1;
        return success;
    }
    // Light sleep without the status pin
    else if (_modemSleepRqPin >= 0 && _dataPin < 0)
    {
        digitalWrite(_modemSleepRqPin, HIGH);
        MS_DBG(F("Requesting light sleep for ESP8266"));
        gsmModem.sendAT(GF("+WAKEUPGPIO=1,"), String(_espSleepRqPin), F(",0"));
        bool success = gsmModem.waitResponse() == 1;
        gsmModem.sendAT(GF("+SLEEP=1"));
        success &= gsmModem.waitResponse() == 1;
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
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, HIGH);
    }
    gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    // Slow down the baud rate for slow processors - and save the change to
    // the ESP's non-volatile memory so we don't have to do it every time
    // #if F_CPU == 8000000L
    // if (modemBaud > 57600)
    // {
    //     _modemSerial->begin(modemBaud);
    //     gsmModem.sendAT(GF("+UART_DEF=9600,8,1,0,0"));
    //     gsmModem.waitResponse();
    //     _modemSerial->end();
    //     _modemSerial->begin(9600);
    // }
    // #endif
    return true;
}


bool EspressifESP8266::startSingleMeasurement(void)
{
    // Sensor::startSingleMeasurement() checks that if it's awake/active and sets
    // the timestamp and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    bool success = true;
    MS_DBG(F("Starting measurement on"), getSensorName());
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an attempt was made.
    _sensorStatus |= 0b00100000;

    // The SSID and password need to be set before the ESP8266m can join a
    //network and get signal strength
    bool alreadyConnect = gsmModem.isNetworkConnected();
    if (!alreadyConnect) success &= gsmModem.networkConnect(_ssid, _pwd);

    if (success)
    {
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    }
    // Otherwise, make sure that the measurement start time and success bit (bit 6) are unset
    else
    {
        MS_DBG(getSensorNameAndLocation(), F("did not successfully start a measurement."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    return success;
}



// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
uint32_t EspressifESP8266::getNISTTime(void)
{
    // NOTE:  For the ESP, we're not checking for internet connection first

    /* Must ensure that we do not ping the daylight more than once every 4 seconds */
    /* NIST clearly specifies here that this is a requirement for all software */
    /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
    while (millis() < _lastNISTrequest + 4000) {}

    /* Make TCP connection */
    MS_DBG(F("\nConnecting to NIST daytime Server"));
    bool connectionMade = gsmClient.connect("time.nist.gov", 37, 15);

    /* Wait up to 5 seconds for a response */
    if (connectionMade)
    { \
        uint32_t start = millis();
        while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L){}

        if (gsmClient.available() >= 4)
        {
            MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
            byte response[4] = {0};
            gsmClient.read(response, 4);
            return parseNISTBytes(response);
        }
        else
        {
            MS_DBG(F("NIST Time server did not respond!"));
            return 0;
        }
    }
    else
    {
        MS_DBG(F("Unable to open TCP to NIST!"));
    }
    return 0;
}

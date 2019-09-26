/*
 *LoggerModem.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file wraps the tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
 *and adds in the power functions to turn the modem on and off.
*/

#include "LoggerModem.h"

// Sensor(const char *sensorName = "Unknown", const uint8_t numReturnedVars = 1,
//        uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0,
//        int8_t powerPin = -1, int8_t dataPin = -1, uint8_t measurementsToAverage = 1);
// Constructors
template <class Derived, typename modemType, typename modemClientType>
loggerModem<Derived, modemType, modemClientType>::loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                                                     int8_t modemResetPin, int8_t modemSleepRqPin, bool alwaysRunWake,
                                                     uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                                                     uint32_t wakeDelayTime_ms, uint32_t max_atresponse_time_ms)
{
    _modemName = "unspecified modem";

    _powerPin = powerPin;
    _statusPin = statusPin;
    _modemResetPin = modemResetPin;
    _modemSleepRqPin = modemSleepRqPin;
    _modemLEDPin = -1;
    _alwaysRunWake = alwaysRunWake;

    _statusLevel = statusLevel;
    _statusTime_ms = max_status_time_ms,
    _disconnetTime_ms = max_disconnetTime_ms;
    _wakeDelayTime_ms = wakeDelayTime_ms;
    _max_atresponse_time_ms = max_atresponse_time_ms;

    _millisPowerOn = 0;

    _lastNISTrequest = 0;
    _lastConnectionCheck = 0;
    _lastATCheck = 0;

    _priorSignalPercent = 0;
    _priorRSSI = 0;
    _priorModemTemp = 0;
    _priorBatteryState = 0;
    _priorBatteryPercent = 0;
    _priorBatteryVoltage = 0;
    // _priorActivationDuration = 0;
    // _priorPoweredDuration = 0;
}


// Destructor
template <class Derived, typename modemType, typename modemClientType>
loggerModem<Derived, modemType, modemClientType>::~loggerModem() {}


template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::setModemLED(int8_t modemLEDPin)
{
    _modemLEDPin = modemLEDPin;
    if (_modemLEDPin >= 0)
    {
        pinMode(_modemLEDPin, OUTPUT);
        digitalWrite(_modemLEDPin, LOW);
    }
};
template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::modemLEDOn(void)
{
    if (_modemLEDPin >= 0)
    {
        digitalWrite(_modemLEDPin, HIGH);
    }
}
template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::modemLEDOff(void)
{
    if (_modemLEDPin >= 0)
    {
        digitalWrite(_modemLEDPin, LOW);
    }
}

template <class Derived, typename modemType, typename modemClientType>
String loggerModem<Derived, modemType, modemClientType>::getModemName(void) { return _modemName; }

// NOTE: Unlike other setup functions, this will NOT turn the power to the modem
// on and off, it will simply check if the power has already been turned on and
// return quickly if not.  If the power had been on, it is left ON after set-up.
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::extraModemSetup(void)
{
    return true;
}
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::modemSetup(void)
{
    bool success = true;

    // Set-up pin modes
    if (_modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);
        // NOTE:  Not setting level of sleep request pin
        // extraModemSetup() should set the sleep request pin level if necessary
    }
    if (_modemResetPin >= 0)
    {
        pinMode(_modemResetPin, OUTPUT);
        digitalWrite(_modemResetPin, HIGH);
    }
    if (_modemLEDPin >= 0)
    {
        pinMode(_modemLEDPin, OUTPUT);
        digitalWrite(_modemLEDPin, LOW);
    }

    MS_DBG(F("Setting up the modem ..."));

    // Power up
    bool wasPowered = true;
    if (_millisPowerOn == 0)
    {
        modemPowerUp();
        wasPowered = false;
    }

    // Check if the modem was awake, wake it if not
    bool wasAwake = (_statusPin >= 0 && digitalRead(_statusPin) == _statusLevel);
    if (!wasAwake)
    {
        while (millis() - _millisPowerOn < _wakeDelayTime_ms) {}
        MS_DBG(F("Waking up the modem for setup ..."));
        success &= modemWake();
    }
    else
    {
        MS_DBG(F("Modem was already awake and should be ready for setup."));
    }

    _modemName = gsmModem.getModemName();

    if (success)
    {
        MS_DBG(F("Running modem's extra setup function ..."));
        success &= extraModemSetup();
        if (success)
        {
            MS_DBG(F("... Complete!  It's a"), getModemName());
        }
        else
        {
            MS_DBG(F("... Failed!  It's a"), getModemName());
        }
    }
    else
    {
        MS_DBG(F("... "), getModemName(), F("did not wake up and cannot be set up!"));
    }

    MS_DBG(_modemName, F("warms up in"), _wakeDelayTime_ms, F("ms, indicates status in"),
           _statusTime_ms, F("ms, is responsive to AT commands in less than"),
           _max_atresponse_time_ms, F("ms, and takes up to"), _disconnetTime_ms,
           F("ms to close connections and shut down."));

    // Put the modem to sleep after finishing setup
    // Only go to sleep if it had been asleep and is now awake
    bool isAwake = (_statusPin >= 0 && digitalRead(_statusPin) == _statusLevel);
    if ((!wasAwake && isAwake) || !wasPowered)
    {
        // Run the sleep function
        MS_DBG(F("Running given modem sleep function ..."));
        success &= modemSleepPowerDown();
    }
    else
    {
        MS_DBG(F("Leaving modem on after setup ..."));
    }

    return success;
}

template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        MS_DBG(F("Powering"), getModemName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Mark the power-on time, just in case it had not been marked
        if (_millisPowerOn == 0)
            _millisPowerOn = millis();
    }
}

template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::modemPowerDown(void)
{
    if (_powerPin >= 0)
    {
        // _priorPoweredDuration = ((float)(millis() - _millisPowerOn)) / 1000;
        // MS_DBG(F("Total modem power-on time (s):"), String(_priorPoweredDuration, 3));

        MS_DBG(F("Turning off power to"), getModemName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    else
    {
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Unset the power-on time
        // _millisPowerOn = 0;
    }
}

// The function to wake up the modem
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::modemWake(void)
{
    bool success = false;

    // Power up
    if (_millisPowerOn == 0)
        modemPowerUp();

    while (millis() - _millisPowerOn < _wakeDelayTime_ms)
    {
    }

    // Check the status pin and wake bits before running wake function
    // Don't want to accidently pulse an already on modem to off
    // NOTE:  It's possible that the status pin is on, but the modem is actually
    // mid-shutdown.  In that case, we'll mistakenly skip re-waking it.
    if (_statusPin >= 0 && digitalRead(_statusPin) == _statusLevel && !_alwaysRunWake)
    {
        MS_DBG(getModemName(), F("was already on!  (status pin"), _statusPin,
               F("level = "), _statusLevel, F(") Will not run wake function."));
    }
    else
    {
        // Run the input wake function
        MS_DBG(F("Running wake function for"), getModemName());
        success &= modemWakeFxn();
    }

    uint8_t resets = 0;
    while (!success && resets < 2)
    {
        // Check that the modem is responding to AT commands
        MS_START_DEBUG_TIMER;
        MS_DBG(F("\nWaiting for"), getModemName(), F("to respond to AT commands..."));
        success &= gsmModem.testAT(_max_atresponse_time_ms + 500);
        if (success)
        {
            MS_DBG(F("No response to AT commands!"));
        }
        else
        {
            MS_DBG(F("... AT OK after"), MS_PRINT_DEBUG_TIMER, F("milliseconds!"));
        }

        // Re-check the status pin
        if ((_statusPin >= 0 && _statusTime_ms == 0 &&
             digitalRead(_statusPin) != _statusLevel && !success) ||
            !success)
        {
            MS_DBG(getModemName(), F("doesn't appear to be responsive!"));
            if (_statusPin >= 0)
            {
                MS_DBG(F("Status pin"), _statusPin, F("on"), getModemName(), F("is"),
                       digitalRead(_statusPin), F("indicating it is off!"));
            }

            MS_DBG(F("Attempting a hard reset on the modem!"));
            if (!modemHardReset())
            {
                // Exit if we can't hard reset
                break;
            }
        }
    }

    // Re-run the modem init
    // This will turn off echo, which often turns itself back on after a reset/power loss
    // This also checks the SIM card state
    success &= gsmModem.init();
    gsmClient.init(&gsmModem);

    if (success)
    {
        modemLEDOn();
        MS_DBG(getModemName(), F("should be awake and ready to go."));
    }
    else
    {
        MS_DBG(getModemName(), F("failed to wake!"));
    }

    return success;
}

// Nicely put the modem to sleep and power down
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::modemSleepPowerDown(void)
{
    bool success = true;
    uint32_t start = millis();
    MS_DBG(F("Turning"), getModemName(), F("off."));

    // If there's a status pin available, check before running the sleep function
    // NOTE:  It's possible that the modem could still be in the process of turning
    // on and thus status pin isn't valid yet.  In that case, we wouldn't yet
    // know it's coming on and so we'd mistakenly assume it's already off and
    // not turn it back off.
    if (_statusPin >= 0 && digitalRead(_statusPin) != _statusLevel && !_alwaysRunWake)
    {
        MS_DBG(F("Status pin"), _statusPin, F("on"), getModemName(), F("is"),
               digitalRead(_statusPin),
               F("indicating it is already off!  Will not run sleep function."));
        // _priorActivationDuration = 0;
    }
    // If there's no status pin, check against the status bits
    else if (!_alwaysRunWake)
    {
        MS_DBG(getModemName(), F("was never sucessfully turned on.  Will not run sleep function."));
    }
    else
    {
        // Run the sleep function
        MS_DBG(F("Running given sleep function for"), getModemName());
        success &= modemSleepFxn();
        modemLEDOff();
    }

    // Now power down
    if (_powerPin >= 0)
    {
        // If there's a status pin available, wait until modem shows it's ready to be powered off
        // This allows the modem to shut down gracefully.
        if (_statusPin >= 0)
        {
            MS_DBG(F("Waiting up to"), _disconnetTime_ms,
                   F("milliseconds for graceful shutdown as indicated by pin"),
                   _statusPin, F("going"), !_statusLevel, F("..."));
            while (millis() - start < _disconnetTime_ms && digitalRead(_statusPin) == _statusLevel)
            {
            }
            if (digitalRead(_statusPin) == _statusLevel)
            {
                MS_DBG(F("... "), getModemName(), F("did not successfully shut down!"));
            }
            else
            {
                MS_DBG(F("... shutdown complete after"), millis() - start, F("ms."));
            }
        }
        else if (_disconnetTime_ms > 0)
        {
            MS_DBG(F("Waiting"), _disconnetTime_ms, F("ms for graceful shutdown."));
            while (millis() - start < _disconnetTime_ms)
            {
            }
        }

        // _priorPoweredDuration = ((float)(millis() - _millisPowerOn)) / 1000;
        // MS_DBG(F("Total modem power-on time (s):"), String(_priorPoweredDuration, 3));

        MS_DBG(F("Turning off power to"), getModemName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    else
    {
        // _priorPoweredDuration = (float)-9999;

        // If we're not going to power the modem down, there's no reason to hold
        // up the main processor while waiting for the modem to shut down.
        // It can just do its thing unwatched while the main processor sleeps.
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Unset the power-on time
        // _millisPowerOn = 0;
    }

    return success;
}

// Perform a hard/panic reset for when the modem is completely unresponsive
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::modemHardReset(void)
{
    if (_modemResetPin >= 0)
    {
        MS_DBG(F("Doing a hard reset on the modem!"));
        digitalWrite(_modemResetPin, LOW);
        delay(200);
        digitalWrite(_modemResetPin, HIGH);
        return true;
    }
    else
    {
        MS_DBG(F("No pin has been provided to reset the modem!"));
        return false;
    }
}


#if defined TINY_GSM_MODEM_HAS_GPRS
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::isInternetAvailable(void)
{
    return gsmModem.isGprsConnected();
}

template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::connectInternet(uint32_t maxConnectionTime)
{
    MS_START_DEBUG_TIMER
    MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,
           F("seconds for cellular network registration..."));
    if (gsmModem.waitForNetwork(maxConnectionTime))
    {
        MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds.  Connecting to GPRS..."));

#ifndef TINY_GSM_MODEM_XBEE
        gsmModem.gprsConnect(_apn, "", "");
#endif
        MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds."));
        return true;
    }
    else
    {
        MS_DBG(F("...GPRS connection failed."));
        return false;
    }
}

template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::disconnectInternet(void)
{
    MS_START_DEBUG_TIMER;
    gsmModem.gprsDisconnect();
    MS_DBG(F("Disconnected from cellular network after"), MS_PRINT_DEBUG_TIMER,
           F("milliseconds."));
}


#else
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::isInternetAvailable(void)
{
    return gsmModem.isNetworkConnected();
}

template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::connectInternet(uint32_t maxConnectionTime)
{
    MS_START_DEBUG_TIMER
    MS_DBG(F("\nAttempting to connect to WiFi network..."));
    if (!(gsmModem.isNetworkConnected()))
    {
        MS_DBG(F("Sending credentials..."));
        while (!gsmModem.networkConnect(_ssid, _pwd))
        {
        };
        MS_DBG(F("Waiting up to"), maxConnectionTime / 1000,
               F("seconds for connection"));
        if (!gsmModem.waitForNetwork(maxConnectionTime))
        {
            MS_DBG(F("... WiFi connection failed"));
            return false;
        }
    }
    MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER,
           F("milliseconds!"));
    return true;
}

template <class Derived, typename modemType, typename modemClientType>
void loggerModem<Derived, modemType, modemClientType>::disconnectInternet(void)
{
    MS_START_DEBUG_TIMER;
    gsmModem.networkDisconnect();
    MS_DBG(F("Disconnected from WiFi network after"), MS_PRINT_DEBUG_TIMER,
           F("milliseconds."));
}
#endif

// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
template <class Derived, typename modemType, typename modemClientType>
uint32_t loggerModem<Derived, modemType, modemClientType>::getNISTTime(void)
{
    // bail if not connected to the internet
    if (!isInternetAvailable())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    // Try up to 12 times to get a timestamp from NIST
    for (uint8_t i = 0; i < 12; i++)
    {
        // Must ensure that we do not ping the daylight server more than once every 4 seconds
        // NIST clearly specifies here that this is a requirement for all software
        // that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000)
        {
        }

        // Make TCP connection
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = gsmClient.connect("time.nist.gov", 37, 15);

        // Wait up to 5 seconds for a response
        if (connectionMade)
        {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L)
            {
            }

            if (gsmClient.available() >= 4)
            {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[4] = {0};
                gsmClient.read(response, 4);
                if (gsmClient.connected())
                    gsmClient.stop();
                return parseNISTBytes(response);
            }
            else
            {
                MS_DBG(F("NIST Time server did not respond!"));
                if (gsmClient.connected())
                    gsmClient.stop();
            }
        }
        else
        {
            MS_DBG(F("Unable to open TCP to NIST!"));
        }
    }
    return 0;
}


template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::getModemSignalQuality(int16_t &rssi, int16_t &percent)
{
    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    int16_t signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    // Convert signal quality to RSSI, if necessary
#if defined TINY_GSM_MODEM_XBEE || defined TINY_GSM_MODEM_ESP8266
    rssi = signalQual;
    MS_DBG(F("Raw signal is already in units of RSSI:"), rssi);
    percent = getPctFromRSSI(signalQual);
    MS_DBG(F("Signal percent calcuated from RSSI:"), percent);
#else
    rssi = getRSSIFromCSQ(signalQual);
    MS_DBG(F("RSSI Estimated from CSQ:"), rssi);
    percent = getPctFromCSQ(signalQual);
    MS_DBG(F("Signal percent calcuated from CSQ:"), percent);
#endif

    return true;
}

#ifdef MS_MODEM_HAS_BATTERY_DATA
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts)
{
    MS_DBG(F("Getting modem battery data:"));
    return gsmModem.getBattStats(chargeState, percent, milliVolts);
}

#else
template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts)
{
    MS_DBG(F("This modem doesn't return battery information!"));
    chargeState = 99;
    percent = -99;
    milliVolts = 9999;
    return false;
}
#endif

#ifdef MS_MODEM_HAS_BATTERY_DATA
// NOTE:  Most modems don't give this
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemChipTemperature(void)
{
    MS_DBG(F("Getting temperature:"));
    float temp = gsmModem.getTemperature();
    MS_DBG(F("Temperature:"), temp);

    return temp;
}

#else

// NOTE:  Could actually get temperature from the Digi chip by entering command mode
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemChipTemperature(void)
{
    MS_DBG(F("This modem doesn't return temperature!"));
    return (float)-9999;
}
#endif

template <class Derived, typename modemType, typename modemClientType>
bool loggerModem<Derived, modemType, modemClientType>::updateModemMetadata(void)
{
    bool success = true;

    // Initialize variable
    int16_t rssi = -9999;
    int16_t percent = -9999;
    uint8_t state = 99;
    int8_t bpercent = -99;
    uint16_t volt = 9999;

    success &= getModemSignalQuality(rssi, percent);
    MS_DBG(F("CURRENT RSSI:"), rssi);
    MS_DBG(F("CURRENT Percent signal strength:"), percent);
    _priorRSSI = rssi;
    _priorSignalPercent = percent;

    success &= getModemBatteryStats(state, bpercent, volt);
    MS_DBG(F("CURRENT Modem Battery Charge State:"), state);
    MS_DBG(F("CURRENT Modem Battery Charge Percentage:"), bpercent);
    MS_DBG(F("CURRENT Modem Battery Voltage:"), bpercent);
    if (state != 99)
        _priorBatteryState = (float)state;
    else
        _priorBatteryState = (float)-9999;

    if (bpercent != -99)
        _priorBatteryPercent = (float)bpercent;
    else
        _priorBatteryPercent = (float)-9999;

    if (volt != 9999)
        _priorBatteryVoltage = (float)volt;
    else
        _priorBatteryVoltage = (float)-9999;

    _priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem Chip Temperature:"), _priorModemTemp);

    return success;
}

template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemRSSI()
{
    float retVal = _priorRSSI;
    _priorRSSI = -9999;
    return retVal;
}
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemSignalPercent()
{
    float retVal = _priorSignalPercent;
    _priorSignalPercent = -9999;
    return retVal;
}
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemBatteryChargeState()
{
    float retVal = _priorBatteryState;
    _priorBatteryState = -9999;
    return retVal;
}
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemBatteryChargePercent()
{
    float retVal = _priorBatteryPercent;
    _priorBatteryPercent = -9999;
    return retVal;
}
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemBatteryVoltage()
{
    float retVal = _priorBatteryPercent;
    _priorBatteryPercent = -9999;
    return retVal;
}
template <class Derived, typename modemType, typename modemClientType>
float loggerModem<Derived, modemType, modemClientType>::getModemTemperature()
{
    float retVal = _priorModemTemp;
    _priorModemTemp = -9999;
    return retVal;
}
// template <class Derived, typename modemType, typename modemClientType>
// float loggerModem<Derived, modemType, modemClientType>::getModemActivationDuration()
// {
//     float retVal = _priorActivationDuration;
//     _priorActivationDuration = -9999;
//     return retVal;
// }
// template <class Derived, typename modemType, typename modemClientType>
// float loggerModem<Derived, modemType, modemClientType>::getModemPoweredDuration()
// {
//     float retVal = _priorPoweredDuration;
//     _priorPoweredDuration = -9999;
//     return retVal;
// }

// Helper to get approximate RSSI from CSQ (assuming no noise)
template <class Derived, typename modemType, typename modemClientType>
int16_t loggerModem<Derived, modemType, modemClientType>::getRSSIFromCSQ(int16_t csq)
{
    int16_t CSQs[33]  = {   0,    1,    2,    3,    4,    5,    6,   7,   8,   9,
                           10,   11,   12,   13,   14,   15,   16,  17,  18,  19,
                           20,   21,   22,   23,   24,   25,   26,  27,  28,  29,
                           30,   31,   99};
    int16_t RSSIs[33] = {-113, -111, -109, -107, -105, -103, -101, -99, -97, -95,
                          -93,  -91,  -89,  -87,  -85,  -83,  -81, -79, -77, -75,
                          -73,  -71,  -69,  -67,  -65,  -63,  -61, -59, -57, -55,
                          -53,  -51,    0};
    for (uint8_t i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return RSSIs[i];
    }
    return 0;
}

// Helper to get signal percent from CSQ
template <class Derived, typename modemType, typename modemClientType>
int16_t loggerModem<Derived, modemType, modemClientType>::getPctFromCSQ(int16_t csq)
{
    int16_t CSQs[33]  = {   0,    1,    2,    3,    4,    5,    6,   7,   8,   9,
                           10,   11,   12,   13,   14,   15,   16,  17,  18,  19,
                           20,   21,   22,   23,   24,   25,   26,  27,  28,  29,
                           30,   31,   99};
    int16_t PCTs[33] = {    0,    3,    6,   10,   13,   16,   19,  23,  26,  29,
                           32,   36,   39,   42,   45,   48,   52,  55,  58,  61,
                           65,   68,   71,   74,   78,   81,   84,  87,  90,  94,
                           97,  100,    0};
    for (uint8_t i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return PCTs[i];
    }
    return 0;
}

// Helper to get signal percent from RSSI
template <class Derived, typename modemType, typename modemClientType>
int16_t loggerModem<Derived, modemType, modemClientType>::getPctFromRSSI(int16_t rssi)
{
    int16_t pct = 1.6163 * rssi + 182.61;
    if (rssi == 0)
        pct = 0;
    if (rssi == (255 - 93))
        pct = 0; // This is a no-data-yet value from XBee
    return pct;
}



template <class Derived, typename modemType, typename modemClientType>
uint32_t loggerModem<Derived, modemType, modemClientType>::parseNISTBytes(byte nistBytes[4])
{
    // Response is returned as 32-bit number as soon as connection is made
    // Connection is then immediately closed, so there is no need to close it
    uint32_t secFrom1900 = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        MS_DBG(F("Response Byte"), i, ':', (char)nistBytes[i],
                   '=', nistBytes[i], '=', String(nistBytes[i], BIN));
        secFrom1900 += 0x000000FF & nistBytes[i];
        // MS_DBG(F("\nseconds from 1900 after byte:"),String(secFrom1900, BIN));
        if (i + 1 < 4)
        {
            secFrom1900 = secFrom1900 << 8;
        }
    }
    MS_DBG(F("Seconds from Jan 1, 1900 returned by NIST (UTC):"),
               secFrom1900, '=', String(secFrom1900, BIN));

    // Return the timestamp
    uint32_t unixTimeStamp = secFrom1900 - 2208988800;
    MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);
    // If before Jan 1, 2019 or after Jan 1, 2030, most likely an error
    if (unixTimeStamp < 1546300800)
    {
        return 0;
    }
    else if (unixTimeStamp > 1893456000)
    {
        return 0;
    }
    else
    {
        return unixTimeStamp;
    }
}


// Helper to set the timing for specific cellular chipsets based on their documentation
/***
NOTE:  These times are for raw cellular chips they do no necessarily
apply to assembled break-out boards or modules
**warmUpTime** = Length of time after power is applied to module before the
enable pin can be called to turn on the module or other wake fxn can be used.
If the module boots up as soon as power is applied, this value is 0.
**indicatorTime** = Length of time from the completion of wake up  request
until the modem status pin begins to show an "on" status.
**stabilizationTime** =  Length of time from the completion of wake up
function until UART port becomes available for AT commands.  This becomes
the MAXIMUM amount of time we will wait for a response.  Where I could
find a time listed for boot up in the documentation, I use that time.
Where I could not find it listed, I use 5 seconds.
**disconnetTime** - Approximate length of time for unit to gracefully
close sockets and disconnect from the network.  Most manufactures strongly
recommend allowing a graceful shut-down rather than a sudden power-off.
void loggerModem::setModemTiming(void)
{
    if (_modemName.indexOf(F("SARA-G3")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a u-blox SARA-G3"));
        _wakeDelayTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active
                              // Unspecified in documentation! Taking value from Lisa U2
        _max_atresponse_time_ms = 5000;  // Time until system and digital pins are operational (5 sec typical)
        // _on_pull_down_ms = 6;  // >5ms
        // _off_pull_down_ms = 1100;  // >1s
        _disconnetTime_ms = 15000;  // Power down time "can largely vary depending
        // on the application / network settings and the concurrent module
        // activities."  Vint/status pin should be monitored and power not withdrawn
        // until that pin reads low.  Giving 15sec here in case it is not monitored.
    }
    if (_modemName.indexOf(F("LISA-U2")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a u-blox LISA-U2"));
        _wakeDelayTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active <35ms
        _max_atresponse_time_ms = 3000;  // Time until system and digital pins are operational (3 sec typical)
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 400;  // power down (gracefully) takes ~400ms
    }
    if (_modemName.indexOf(F("Digi XBee® Cellular LTE Cat 1")) >= 0  ||
        _modemName.indexOf(F("Digi XBee3™ Cellular LTE CAT 1")) >= 0  ||
        _modemName.indexOf(F("Telit LE866")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Telit LE866"));
        _wakeDelayTime_ms = 0;  // Module turns on when power is applied
        _statusTime_ms = 50;  // Documentation does not specify how long between
        // power on and high reading on VAUX / PWRMON pin
        _max_atresponse_time_ms = 25000;  // Documentation says to wait up to 25 (!!) seconds.
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 10000L;  // Wait with 10s time-out for sleep
    }
    if (_modemName.indexOf(F("Neoway M590")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Neoway M590"));
        _wakeDelayTime_ms = 300;  // ON/OFF pin can be held low when power is applied
        // If the ON/OFF pin is not held low at time power is applied, wait at
        // least 300ms before dropping it low to turn the module on
        _statusTime_ms = 300;  // Time after end pulse until VCCIO becomes active
        _max_atresponse_time_ms = 300;  // Time until UART is active (300ms)
        // _on_pull_down_ms = 510;  // >300ms (>500ms recommended)
        // _off_pull_down_ms = 510;  // >300ms
        _disconnetTime_ms = 6000;  // power down (gracefully) takes ~5sec
    }
    if (_modemName.indexOf(F("Quectel BC95")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel BC95"));
        _wakeDelayTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _statusTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _max_atresponse_time_ms = 5000;  // ?? Time to UART availability not documented
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        // use AT+CPSMS command for LTE-M power saving - no other power save method
        _disconnetTime_ms = 0;  // N/A - If the reset pin is used as a status pin, it will not ever turn off
    }
    if (_modemName.indexOf(F("Quectel M95")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel M95"));
        _wakeDelayTime_ms = 30;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _max_atresponse_time_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 2000;  // until either status key goes on, or > 1.0 sec (~2s)
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf(F("Quectel MC60")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel MC60"));
        _wakeDelayTime_ms = 100;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _max_atresponse_time_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
}
***/
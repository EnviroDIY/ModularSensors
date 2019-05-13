/*
 *DigiXBee.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Included Dependencies
#include "DigiXBee.h"


// Constructor
DigiXBee::DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
            int8_t modemResetPin, int8_t modemSleepRqPin,
            TinyGsmModem *inModem, Client *inClient, const char *ssid, const char *pwd)
  : loggerModem(powerPin, statusPin, !useCTSStatus,
                modemResetPin, modemSleepRqPin,
                XBEE_STATUS_TIME_MS, XBEE_DISCONNECT_TIME_MS,
                XBEE_WARM_UP_TIME_MS, XBEE_ATRESPONSE_TIME_MS,
                inModem, inClient, ssid, pwd)
{}


bool DigiXBee::setup(void)
{
    bool success = Sensor::setup();  // this will set pin modes and the setup status bit

    // Initialize the modem
    MS_DBG(F("Setting up the XBee ..."));

    // Bail if there's no power
    if (!checkPowerOn())
    {
        MS_DBG(F("XBee doesn't have power and cannot be set-up!"));
        return false;
    }

    // Check if the modem was awake, wake it if not
    // NOTE:  We ar NOT powering up the modem!  Set up will NOT be successful
    // unless the modem is already powered external to this function.
    bool wasAwake = ( (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel)
                     || bitRead(_sensorStatus, 4) );
    if (!wasAwake)
    {
        waitForWarmUp();
        MS_DBG(F("Turning on the XBee ..."));
        success &= wake();
    }
    else MS_DBG(F("XBee was already awake."));

    if (success)
    {
        // The begin() generally starts with a 5 second testAT(), that should
        // be enough time to allow any modem to be ready to respond
        MS_DBG(F("Running XBee's begin function ..."));
        success &= _tinyModem->begin();
        _modemName = _tinyModem->getModemName();
        success &= extraXBeeSetup();
        if (success) MS_DBG(F("... Complete!  It's a"), getSensorName());
        else MS_DBG(F("... Failed!  It's a"), getSensorName());
    }
    else MS_DBG(F("... "), getSensorName(), F("did not wake up and cannot be set up!"));

    MS_DBG(_modemName, F("warms up in"), _warmUpTime_ms, F("ms, indicates status in"),
           _statusTime_ms, F("ms, is responsive to AT commands in less than"),
           _stabilizationTime_ms, F("ms, and takes up to"), _disconnetTime_ms,
           F("ms to close connections and shut down."));

    // XBee saves all configurations to flash, so we can set them here
    _tinyModem->networkConnect(_ssid, _pwd);

    // Print out some warnings if things seem to be improperly formatted
    if (_tinyModem->hasWifi() && _ssid == NULL)
        MS_DBG(F("WARNING:  Wifi modem with no SSID given!"));
     if (_tinyModem->hasGPRS() && _apn == NULL)
         MS_DBG(F("WARNING:  Cellular modem with no APN given!"));

    // Set the status bit marking that the modem has been set up (bit 0)
    // Only set the bit if setup was successful!
    if (success) _sensorStatus |= 0b00000001;
    // Otherwise, set the status error bit (bit 7)
    else _sensorStatus |= 0b10000000;

    // Put the modem to sleep after finishing setup
    // Only go to sleep if it had been asleep and is now awake
    bool isAwake = ( (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel)
                    || bitRead(_sensorStatus, 4) );
    if (!wasAwake && isAwake)
    {
        // Run the sleep function
        MS_DBG(F("Running given modem sleep function ..."));
        success &= _modemSleepFxn();
    }
    else MS_DBG(F("Leaving modem on after setup ..."));
    // Do NOT power down at the end, because this fxn cannot have powered the
    // modem up.

    return success;
}


bool DigiXBee::wake(void)
{
    // Sensor::wake() checks if the power pin is on, setup has been successful,
    // and sets the wake timestamp and status bits.  If it returns false,
    // there's no reason to go on.
    if (!Sensor::wake()) return false;
    // NOTE:  This is the ONLY place _millisSensorActivated is set!
    // NOTE:  This is the ONLY place bit 4 is set!

    bool success = true;

    // Check the status pin and wake bits before running wake function
    // Don't want to accidently pulse an already on modem to off
    if ( bitRead(_sensorStatus, 4))
    {
        MS_DBG(getSensorName(), F("has already been woken up!  Will not run wake function."));
    }
    // NOTE:  It's possible that the status pin is on, but the modem is actually
    // mid-shutdown.  In that case, we'll mistakenly skip re-waking it.
    else if (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel)
    {
        MS_DBG(getSensorName(), F("was already on!  (status pin level = "),
               _statusLevel, F(") Will not run wake function."));
    }
    else
    {
        // Run the input wake function
        MS_DBG(F("Running wake function for"), getSensorName());
        success &= XBeeWake();
    }

    // Re-check the status pin
    // Only works if the status pin comes on immediately
    if (_dataPin > 0 && _statusTime_ms == 0 && digitalRead(_dataPin) != _statusLevel)
    {
        MS_DBG(F("Status pin level on"), getSensorName(), F("is"),
                   digitalRead(_dataPin), F("indicating it is off!"));
        success = false;
    }

    if (success)
    {
        if (_modemLEDPin >= 0)
        {
            digitalWrite(_modemLEDPin, HIGH);
        }
        MS_DBG(getSensorName(), F("should be awake."));
    }
    else
    {
        MS_DBG(getSensorName(), F("failed to wake!"));
        // Make sure the activation time is zero and the wake success bit (bit 4) is unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
    }

    return success;
}


bool DigiXBee::modemSleepPowerDown(void)
{
    bool success = true;
    uint32_t start = millis();
    MS_DBG(F("Turning"), getSensorName(), F("off."));

    // If there's a status pin available, check before running the sleep function
    // NOTE:  It's possible that the modem could still be in the process of turning
    // on and thus status pin isn't valid yet.  In that case, we wouldn't yet
    // know it's coming on and so we'd mistakenly assume it's already off and
    // not turn it back off.
    if (_dataPin >= 0 && digitalRead(_dataPin) != _statusLevel)
        MS_DBG(getSensorName(), F("appears to have already been off.  Will not run sleep function."));
    // If there's no status pin, check against the status bits
    else if (_dataPin < 0 && !bitRead(_sensorStatus, 4))
        MS_DBG(getSensorName(), F("was never sucessfully turned on.  Will not run sleep function."));
    else
    {
        // Run the sleep function
        MS_DBG(F("Running given sleep function for"), getSensorName());
        success &= XBeeSleep();

        if (_modemLEDPin >= 0)
        {
            digitalWrite(_modemLEDPin, LOW);
        }
    }

    // Unset the activation time
    _millisSensorActivated = 0;
    // Unset the measurement request time
    _millisMeasurementRequested = 0;
    // Unset the status bits for sensor activation (bits 3 & 4) and measurement
    // request (bits 5 & 6)
    _sensorStatus &= 0b10000111;

    // Now power down
    if (_powerPin >= 0)
    {
        // If there's a status pin available, wait until modem shows it's ready to be powered off
        // This allows the modem to shut down gracefully.
        if (_dataPin >= 0)
        {
            MS_DBG(F("Waiting up to"), _disconnetTime_ms, F("milliseconds for graceful shutdown..."));
            while (millis() - start < _disconnetTime_ms && digitalRead(_dataPin) == _statusLevel){}
            if (digitalRead(_dataPin) == _statusLevel)
                MS_DBG(F("... "), getSensorName(), F("did not successfully shut down!"));
            else MS_DBG(F("... shutdown complete after"), millis() - start, F("ms."));
        }
        else if (_disconnetTime_ms > 0)
        {
            MS_DBG(F("Waiting"), _disconnetTime_ms, F("ms for graceful shutdown."));
            while (millis() - start < _disconnetTime_ms){}
        }

        MS_DBG(F("Turning off power to"), getSensorName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
    }
    else
    {
        // If we're not going to power the modem down, there's no reason to hold
        // up the  main processor while waiting for the modem to shut down.
        // It can just do its thing unwatched while the main processor sleeps.
        MS_DBG(F("Power to"), getSensorName(), F("is not controlled by this library."));
        // Unset the power-on time and bits even if we didn't do anything.
        // This prevents the wake from happening on modems with no power pin
        // unless modemPowerUp() is called.
        _millisPowerOn = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
    }

    return success;
}


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool DigiXBee::XBeeWake(void)
{
    if (_modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(_modemSleepRqPin, HIGH);
        return true;
    }
    else
    {
        return true;
    }
}


bool DigiXBee::XBeeSleep(void)
{
    if (_powerPin >= 0)  // Turns on when power is applied
        return true;
    else if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        digitalWrite(_modemLEDPin, HIGH);  // Because the XBee doesn't have any lights
        return true;
    }
    else
    {
        return true;
    }
}


bool DigiXBee::extraXBeeSetup(void)
{
    _tinyModem->init();  // initialize
    if (_tinyModem->commandMode())
    {
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        _tinyModem->sendAT(F("D8"),1);
        _tinyModem->waitResponse();
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        _tinyModem->sendAT(F("D9"),1);
        _tinyModem->waitResponse();
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        _tinyModem->sendAT(F("D7"),1);
        _tinyModem->waitResponse();
        // Put the XBee in pin sleep mode
        _tinyModem->sendAT(F("SM"),1);
        _tinyModem->waitResponse();
        // Disassociate from network for lowest power deep sleep
        _tinyModem->sendAT(F("SO"),200);
        _tinyModem->waitResponse();
        // Write changes to flash and apply them
        _tinyModem->writeChanges();
        // Exit command mode
        _tinyModem->exitCommand();
    }
}

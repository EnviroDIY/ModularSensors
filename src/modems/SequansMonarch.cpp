/*
 *SequansMonarch.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Botletics and other modules based on the SIMCOM BG96.
*/

// Included Dependencies
#include "SequansMonarch.h"
#include "LoggerModemMacros.h"

// Constructor
SequansMonarch::SequansMonarch(Stream* modemStream,
                               int8_t powerPin, int8_t statusPin,
                               int8_t modemResetPin, int8_t modemSleepRqPin,
                               const char* apn)
    : loggerModem(powerPin, statusPin, VZM20Q_STATUS_LEVEL,
                  modemResetPin, VZM20Q_RESET_LEVEL, VZM20Q_RESET_PULSE_MS,
                  modemSleepRqPin, VZM20Q_WAKE_LEVEL, VZM20Q_WAKE_PULSE_MS,
                  VZM20Q_STATUS_TIME_MS, VZM20Q_DISCONNECT_TIME_MS,
                  VZM20Q_WARM_UP_TIME_MS, VZM20Q_ATRESPONSE_TIME_MS),
#ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      gsmClient(gsmModem)
{
    _apn = apn;
}

// Destructor
SequansMonarch::~SequansMonarch() {}

MS_MODEM_WAKE(SequansMonarch);

MS_MODEM_CONNECT_INTERNET(SequansMonarch);
MS_MODEM_DISCONNECT_INTERNET(SequansMonarch);
MS_MODEM_IS_INTERNET_AVAILABLE(SequansMonarch);

MS_MODEM_GET_NIST_TIME(SequansMonarch);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(SequansMonarch);
MS_MODEM_GET_MODEM_BATTERY_DATA(SequansMonarch);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(SequansMonarch);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SequansMonarch::modemWakeFxn(void)
{
    // Module turns on when power is applied
    // No pulsing required in this case
    if (_powerPin >= 0)
    {
        MS_DBG(F("Waiting for modem start-up message"));
        return gsmModem.waitResponse(5000L, GF("+SYSSTART")) == 1;
    }
    else if (_modemResetPin >= 0)
    {
        modemHardReset();
        MS_DBG(F("Waiting for modem start-up message"));
        return gsmModem.waitResponse(5000L, GF("+SYSSTART")) == 1;
    }
    if (_modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        MS_DBG(F("Setting pin"), _modemSleepRqPin, _wakeLevel ? F("HIGH") : F("LOW"),
               F("to bring"), _modemName, F("out of power save mode"));
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        return true;
    }
    else
    {
        return true;
    }
}


bool SequansMonarch::modemSleepFxn(void)
{
    if (_powerPin >= 0 || _modemResetPin >= 0)  // will go on with power on
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        // The only way to wake from this is via a hard reset
        MS_DBG(F("Asking Sequans Monarch to power down"));
        return gsmModem.poweroff();
    }
    else if (_modemSleepRqPin >= 0)  // RTS for power save mode
    {
        MS_DBG(F("Setting pin"), _modemSleepRqPin, !_wakeLevel ? F("HIGH") : F("LOW"),
               F("to enable"), _modemName, F("to enter power save mode"));
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        return true;
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool SequansMonarch::extraModemSetup(void)
{
    bool success = gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    // Enable power save mode if we're not going to cut power or use reset
    if (!(_powerPin >= 0) && !(_modemResetPin >= 0) && _modemSleepRqPin >= 0)
    {
        MS_DBG("Enabling power save mode tracking area update [PSM TAU] timers");
        // Requested Periodic TAU (Time in between Tracking Area Updates) = 101 00001 = 5min increments * 1
        // Requested Active Time (Time connected before entering Power Save Mode) = 000 00101 = 2s increments * 5
        gsmModem.sendAT(GF("+CPSMS=1,,,\"10100001\",\"00000101\""));
        success &= gsmModem.waitResponse();
    }
    // If we are going to turn power it on and off or use the reset, turn on auto-connect and auto-internet so
    // the module will immediately start trying to connect on wake.  Unlike most others, the VZN20Q turns on
    // with the cellular radio disabled by default and will not turn on or search for the network without
    // enabling the radio.
    if (_powerPin >= 0 || _modemResetPin >= 0)
    {
        // Enable and force auto-connect - boot with CFUN=1 and attempt to register on network
        gsmModem.sendAT(GF("+SQNAUTOCONNECT=1,1"));
        success &= gsmModem.waitResponse();
        // Enable auto internet -automatically try to connect internet PDN provisioned into /etc/config/sqnmm
        // after each attach to the network
        gsmModem.sendAT(GF("+SQNAUTOINTERNET=1"));
        success &= gsmModem.waitResponse();
    }

    return success;
}

/*
 *ublox2and3G.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all u-blox 3G/2G modules.  This is an extra level
 *on top of the DigiXBee3GBypass and the Sodaq UBee U201 needed so that
 *TinyGSMublox.h is only included once.  Because that h file contains the
 *definitions of functions instead of mearly their declarations, including
 *it more than once causes a violation of the one-definition rule and
 *possible undefined behavior (ie, bad things).
*/

// Included Dependencies
#include "ublox2and3G.h"
#include "modems/LoggerModemMacros.h"


// Constructor
ublox2and3G::ublox2and3G(Stream *modemStream,
                         int8_t powerPin, int8_t statusPin, bool statusLevel,
                         int8_t modemResetPin, int8_t modemSleepRqPin, bool alwaysRunWake,
                         uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                         uint32_t max_warmUpTime_ms, uint32_t max_atresponse_time_ms,
                         uint32_t max_signalQuality_time_ms,
                         const char *apn,
                         uint8_t measurementsToAverage = 1)
    : loggerModem(powerPin, statusPin, statusLevel,
                  modemResetPin, modemSleepRqPin, alwaysRunWake,
                  max_status_time_ms, max_disconnetTime_ms,
                  max_warmUpTime_ms, max_atresponse_time_ms,
                  max_signalQuality_time_ms,
                  measurementsToAverage),
#ifdef MS_UBLOX2AND3G_DEBUG_DEEP
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
ublox2and3G::~ublox2and3G() {}

MS_MODEM_DID_AT_RESPOND(ublox2and3G);
MS_MODEM_IS_INTERNET_AVAILABLE(ublox2and3G);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(ublox2and3G);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(ublox2and3G);
MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(ublox2and3G);
MS_MODEM_GET_MODEM_TEMPERATURE_NA(ublox2and3G);
MS_MODEM_CONNECT_INTERNET(ublox2and3G);
MS_MODEM_DISCONNECT_INTERNET(ublox2and3G);
MS_MODEM_GET_NIST_TIME(ublox2and3G);
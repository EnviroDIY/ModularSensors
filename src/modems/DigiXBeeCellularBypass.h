/*
 *DigiXBeeCellularBypass.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's BASED ON UBLOX CHIPS in bypass mode
*/

// Header Guards
#ifndef DigiXBeeCellularBypass_h
#define DigiXBeeCellularBypass_h

// Debugging Statement
// #define MS_DIGIXBEECELLULARBYPASS_DEBUG

#ifdef MS_DIGIXBEECELLULARBYPASS_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_UBLOX

// Included Dependencies
#include "DigiXBee.h"
#include "TinyGsmClient.h"


class DigiXBeeCellularBypass : public DigiXBee
{

public:
    // Constructors
    DigiXBeeCellularBypass(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *apn,
                           uint8_t measurementsToAverage = 1);


    // The a measurement is "complete" when the modem is registered on the network.
    // For a cellular modem, this actually sets the GPRS bearer/APN!!
    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool didATRespond(void) override;
    bool extraModemSetup(void) override;

    TinyGsm _tinyModem;

 private:
    const char *_apn;

};


#endif

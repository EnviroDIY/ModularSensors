/*
 *Sodaq2GBeeR6.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq 2GBee revisions 6 and higher - these are based on
 *the SIMCOM SIM800h.
 */

// Header Guards
#ifndef Sodaq2GBeeR6_h
#define Sodaq2GBeeR6_h

// Debugging Statement
// #define MS_SODAQ2GBEER6_DEBUG
// #define MS_SODAQ2GBEER6_DEBUG_DEEP

#ifdef MS_SODAQ2GBEER6_DEBUG
#define MS_DEBUGGING_STD "Sodaq2GBeeR6"
#endif

// NOTE:  The GPRSBee R6+ has the PWR_KEY tied to the input voltage, so there is
// no warm-up time needed
#define S2GBR6_WARM_UP_TIME_MS 0

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "SIMComSIM800.h"

#ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class Sodaq2GBeeR6 : public SIMComSIM800 {
 public:
    // Constructor/Destructor
    // NOTE:  The Sodaq GPRSBee doesn't expose the SIM800's reset pin
    // NOTE:  The power pin of the SIM800 is wired to the XBee's DTR
    // pin, the actualy PWR_KEY is not exposed, there is no way to
    // request sleep.  The normal Vin pin of the Bee socket (pin 1)
    // is used for voltage reference only.
    // The order of the pins in the constructor is different from
    // other modems for backwards compatibility and because the
    // Sodaq documentation is confusing
    Sodaq2GBeeR6(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                 const char* apn);
    Sodaq2GBeeR6(Stream* modemStream, int8_t vRefPin, int8_t statusPin,
                 int8_t powerPin, const char* apn);
    ~Sodaq2GBeeR6();

    void setVRefPin(int8_t vRefPin);

 protected:
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;

 private:
    int8_t _vRefPin;
};

#endif  // Header Guard

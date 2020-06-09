/**
 * @file Sodaq2GBeeR6.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Sodaq2GBeeR6 subclass of the SIMComSIM800 class and is
 * specific to the Sodaq 2GBee revisions 6 and higher based on the SIMCOM
 * SIM800h.
 */

// Header Guards
#ifndef SRC_MODEMS_SODAQ2GBEER6_H_
#define SRC_MODEMS_SODAQ2GBEER6_H_

// Debugging Statement
// #define MS_SODAQ2GBEER6_DEBUG
// #define MS_SODAQ2GBEER6_DEBUG_DEEP

#ifdef MS_SODAQ2GBEER6_DEBUG
#define MS_DEBUGGING_STD "Sodaq2GBeeR6"
#endif

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * The GPRSBee R6+ has the `PWR_KEY` tied to the input voltage, so there is no
 * warm-up time needed
 */
#define S2GBR6_WAKE_DELAY_MS 0

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "SIMComSIM800.h"

#ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for the Sodaq 2GBee revisions 6 and higher
 * based on the SIMCOM SIM800h.
 *
 * @copydetails #SIM800_ATRESPONSE_TIME_MS
 *
 * @copydetails #SIM800_DISCONNECT_TIME_MS
 *
 * @note The Sodaq GPRSBee doesn't expose the SIM800's reset pin..
 *
 * @note The power pin of the SIM800 is wired to the XBee's `DTR` pin, the
 * `PWR_KEY` itself is not exposed - it is tied inversely to the power in to the
 * module.  This leaves no way to wake up from minimum power mode.  To prevent
 * large power draw, the module must be powered off between data points.
 *
 * @note The normal `Vin` pin of the Bee socket (pin 1) is used for voltage
 * reference only.
 *
 * @see @ref gprsbee_page
 */
class Sodaq2GBeeR6 : public SIMComSIM800 {
 public:
    /**
     * @brief Construct a new Sodaq 2GBee R6 object
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This is the pin labeled `STATUS` in Sodaq (and SIMCom)'s documentation.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see SIMComSIM800::SIMComSIM800
     */
    Sodaq2GBeeR6(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                 const char* apn);
    /**
     * @brief Construct a new Sodaq 2GBee R6 object
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param vRefPin The digital pin number of a pin on the mcu controlling the
     * voltage reference (pin 1) for the GPRSBee
     * @param statusPin @copydoc loggerModem::_statusPin
     * This is the pin labeled `STATUS` in Sodaq (and SIMCom)'s documentation.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @note The order of the pins in the constructor is different from other
     * modems for backwards compatibility and because the Sodaq documentation is
     * somewhat muddled on the distinction between the `PWRKEY` and the module
     * power in.
     */
    Sodaq2GBeeR6(Stream* modemStream, int8_t vRefPin, int8_t statusPin,
                 int8_t powerPin, const char* apn);
    /**
     * @brief Destroy the Sodaq 2GBee R6 object - no action taken
     */
    ~Sodaq2GBeeR6();

    /**
     * @brief Sets the pin to use to control voltage reference on the GPRSBee.
     *
     * @param vRefPin The pin on the MCU controlling the `VREF` pin on the
     * GPRSBee (bee pin 1).
     */
    void setVRefPin(int8_t vRefPin);

 protected:
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;

 private:
    int8_t _vRefPin;
};

#endif  // SRC_MODEMS_SODAQ2GBEER6_H_

/**
 * @file Sodaq2GBeeR6.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Sodaq2GBeeR6 subclass of the SIMComSIM800 class and is
 * specific to the Sodaq 2GBee revisions 6 and higher based on the SIMCOM
 * SIM800h.
 */
/* clang-format off */
/**
 * @defgroup modem_gprsbee Sodaq GPRSBee
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_gprsbee_notes Introduction
 *
 * This is for the Sodaq GPRSBee **R6 and higher**.
 * This board is based on the [SIMCom SIM800H](https://simcom.ee/modules/gsm-gprs/sim800/),
 * but adds an extra transistor to the `PWR_KEY` so it is turned on and off in a different way.
 * For earlier Sodaq GPRSBee's, use the standard SIM800
 * [constructor](#modem_sim800).
 *
 * The modem constructor follows the typical modem pattern, except that the
 * Sodaq GPRSBee's do not expose the SIM800's reset pin or its sleep request
 * (`PWRKEY`) pin.
 * The SIM800H's `PWRKEY` is always held at the inverse of the main power.
 * It cannot be controlled.
 * Thus the GPRSBeeR6 can only be "put to sleep" by cutting the power to the
 * SIM800H.
 *
 * @note **The power pin of the GPRSBee R6 and R7 is wired to the XBee socket
 * pin usually used as the sleep request pin!!**  *On the GPRSBee, the XBee
 * socket pin usually used for Vin (pin 1) is used for communication reference
 * voltage only !*
 *
 * If you are capable of controlling the voltage reference pin (Bee socket pin 1)
 * of the GPRSBee, you can set that pin using the function `setVRefPin(int8_t vRefPin)`.
 *
 * @section modem_gprsbee_docs Manufacturer Documentation
 * The GPRSBee schematics are available here:
 * https://support.sodaq.com/sodaq-one/gprsbee-5/
 *
 * @section modem_gprsbee_ctor Modem Constructor
 * {{ @ref Sodaq2GBeeR6::Sodaq2GBeeR6 }}
 *
 * ___
 * @section modem_gprsbee_examples Example Code
 * The GPRSBee is used in the @menulink{sodaq_2g_bee_r6} example.
 *
 * @menusnip{sodaq_2g_bee_r6}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_SODAQ2GBEER6_H_
#define SRC_MODEMS_SODAQ2GBEER6_H_

// Debugging Statement
// #define MS_SODAQ2GBEER6_DEBUG
// #define MS_SODAQ2GBEER6_DEBUG_DEEP

#ifdef MS_SODAQ2GBEER6_DEBUG
#define MS_DEBUGGING_STD "Sodaq2GBeeR6"
#endif

/** @ingroup modem_gprsbee */
/**@{*/

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
 * @brief The loggerModem subclass for the [Sodaq 2GBee](@ref modem_gprsbee)
 * revisions 6 and higher based on the SIMCOM SIM800H.
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
/**@}*/
#endif  // SRC_MODEMS_SODAQ2GBEER6_H_

/**
 * @file DigiXBee3GBypass.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DigiXBee3GBypass subclass of the DigiXBee class for Digi
 * Cellular XBee's based on u-blox SARA U201 chips and operated in bypass mode.
 */
/* clang-format off */
/**
 * @defgroup modem_digi_3g_bypass Digi XBee® Cellular 3G
 *
 * @ingroup modem_digi
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_digi_3g_bypass_notes Introduction
 *
 * @warning Digi strongly recommends against this, but it actually seems to be
 * more stable in our tests.
 * Your milage may vary.
 *
 * When operated in Digi's "bypass" mode, the u-blox SARA U201 based 3G XBee
 * with 2G fallback can be implented as a DigiXBee3GBypass object -
 * a subclass of DigiXBee and loggerModem.
 * Bypass refers to the fact that XBee3's main processor is bypassed - acting
 * only as a pass-through to the u-blox cellular component.
 *
 * The 3G module *must* have **2 amps** of power available.
 *
 * @section modem_digi_3g_bypass_docs Manufacturer Documentation
 * The Digi product page for the 3G module is here:
 * https://www.digi.com/products/embedded-systems/cellular-modems/digi-xbee-cellular-3g
 *
 * @section modem_digi_3g_bypass_ctor Modem Constructor
 * {{ @ref DigiXBee3GBypass::DigiXBee3GBypass }}
 *
 * ___
 * @section modem_digi_3g_bypass_examples Example Code
 * The digi_cellular is used in the @menulink{digi_xbee_3g_bypass} example.
 *
 * @menusnip{digi_xbee_3g_bypass}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEE3GBYPASS_H_
#define SRC_MODEMS_DIGIXBEE3GBYPASS_H_

// Debugging Statement
// #define MS_DIGIXBEE3GBYPASS_DEBUG
// #define MS_DIGIXBEE3GBYPASS_DEBUG_DEEP

#ifdef MS_DIGIXBEE3GBYPASS_DEBUG
#define MS_DEBUGGING_STD "DigiXBee3GBypass"
#endif

/** @ingroup modem_digi_3g_bypass */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_UBLOX
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#undef TINY_GSM_MODEM_HAS_WIFI
#include "DigiXBee.h"

#ifdef MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for [Digi Cellular XBee's](@ref modem_digi)
 * based on u-blox SARA U201 chips and operated in
 * [bypass mode](@ref modem_digi_3g_bypass).
 *
 * @warning Digi strongly recommends against this, but it actually seems to be
 * more stable in our tests.  Your milage may vary.
 */
class DigiXBee3GBypass : public DigiXBee {
 public:
    /**
     * @brief Construct a new Digi XBee 3G Bypass object
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This can be either the pin named `ON/SLEEP_N/DIO9` or `CTS_N/DIO7` pin in
     * Digi's hardware reference.
     * @param useCTSStatus True to use the `CTS_N/DIO7` pin of the XBee as a
     * status indicator rather than the true status (`ON/SLEEP_N/DIO9`) pin.
     * This inverts the loggerModem::_statusLevel.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This shold be the pin called `RESET_N` in Digi's hardware reference.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This shold be the pin called `DTR_N/SLEEP_RQ/DIO8` in Digi's hardware
     * reference.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see DigiXBee::DigiXBee
     */
    DigiXBee3GBypass(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                     bool useCTSStatus, int8_t modemResetPin,
                     int8_t modemSleepRqPin, const char* apn);
    /**
     * @brief Destroy the Digi XBee 3G Bypass object - no action needed
     */
    ~DigiXBee3GBypass();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

#ifdef MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    /**
     * @brief Public reference to the TinyGSM modem.
     */
    TinyGsm gsmModem;
    /**
     * @brief Public reference to the TinyGSM Client.
     */
    TinyGsmClient gsmClient;

 protected:
    bool isInternetAvailable(void) override;
    /**
     * @copybrief loggerModem::extraModemSetup()
     *
     * For XBees, this sets the appropriate operating mode (transparent or
     * bypass), enables pin sleep, sets the DIO pins to the expected functions,
     * and reboots the modem to ensure all settings are applied.
     *
     * @return **bool** True if the extra setup succeeded.
     */
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
};
/**@}*/
#endif  // SRC_MODEMS_DIGIXBEE3GBYPASS_H_

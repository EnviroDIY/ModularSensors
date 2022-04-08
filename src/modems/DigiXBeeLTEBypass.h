/**
 * @file DigiXBeeLTEBypass.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DigiXBeeLTEBypass subclass of the DigiXBee class for Digi
 * Cellular XBee3's based on u-blox SARA R410M chips and operated in bypass
 * mode.
 */
/* clang-format off */
/**
 * @defgroup modem_digi_lte_bypass Digi XBee® Cellular LTE-M/NB-IoT
 *
 * @ingroup modem_digi
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_digi_lte_bypass_notes Introduction
 *
 * @warning Digi strongly recommends against this, but it actually seems to be
 * more stable in our tests.
 * Your milage may vary.
 *
 * When operated in Digi's "bypass" mode, the u-blox SARA R410M based XBee3 can
 * be implented as a DigiXBeeLTEBypass object - a subclass of DigiXBee and
 * loggerModem.
 * Bypass refers to the fact that XBee3's main processor is bypassed - acting
 * only as a pass-through to the u-blox cellular component.
 *
 * This LTE-M module can just about almost get by with only 500mA power supplied, but it definitely not ideal.
 *
 * @section modem_digi_lte_bypass_docs Manufacturer Documentation
 * The Digi product page for the LTE-M module is here:
 * https://www.digi.com/products/embedded-systems/digi-xbee/cellular-modems/xbee3-cellular-lte-m-nb-iot
 *
 * @section modem_digi_lte_bypass_ctor Modem Constructor
 * {{ @ref DigiXBeeLTEBypass::DigiXBeeLTEBypass }}
 *
 * ___
 * @section modem_digi_lte_bypass_examples Example Code
 * @subsection modem_digi_lte_bypass_modem_obj Creating the Modem Object
 *
 * The digi_cellular is used in the @menulink{digi_xbee_lte_bypass} example.
 *
 * @menusnip{digi_xbee_lte_bypass}
 *
 * @section modem_digi_lte_bypass_network LTE Network Selection
 *
 * It is good practice to select which network you'll be connecting to based
 * on your SIM card and signal availability.
 * Example code for this can also be found in the
 * [menu a la carte example](@ref setup_r4_carrrier).
 *
 * @note The network selection for an LTE-M XBee in bypass mode is identical
 * to that for a Sodaq LTE-M UBee or any other module based on the u-blox SARA
 * R4 series.
 *
 * @menusnip{setup_r4_carrrier}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEELTEBYPASS_H_
#define SRC_MODEMS_DIGIXBEELTEBYPASS_H_

// Debugging Statement
// #define MS_DIGIXBEELTEBYPASS_DEBUG
// #define MS_DIGIXBEELTEBYPASS_DEBUG_DEEP

#ifdef MS_DIGIXBEELTEBYPASS_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeLTEBypass"
#endif

/** @ingroup modem_digi_lte_bypass */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SARAR4
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

#ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

/**
 * @brief The class for any of Digi's cellular LTE-M [XBee3](@ref modem_digi)
 * modules operating in [Digi's "bypass" mode](@ref modem_digi_lte_bypass).
 *
 * @warning Digi strongly recommends against this, but it actually seems to be
 * more stable in our tests.  Your milage may vary.
 */
class DigiXBeeLTEBypass : public DigiXBee {
 public:
    /**
     * @brief Construct a new Digi XBee LTE Bypass object.
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
    DigiXBeeLTEBypass(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                      bool useCTSStatus, int8_t modemResetPin,
                      int8_t modemSleepRqPin, const char* apn);
    /**
     * @brief Destroy the Digi XBee LTE Bypass object - no action needed
     */
    ~DigiXBeeLTEBypass();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

#ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
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
#endif  // SRC_MODEMS_DIGIXBEELTEBYPASS_H_

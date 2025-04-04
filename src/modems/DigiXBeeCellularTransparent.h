/**
 * @file DigiXBeeCellularTransparent.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Greg Cutrell <gcutrell@limno.com>
 *
 * @brief Contains the DigiXBeeCellularTransparent class for Digi Cellular
 * XBee's operating in transparent mode.
 *
 * Modified to allow for username and password required by some cell providers,
 * such as Soracom (https://www.soracom.io)
 */
/* clang-format off */
/**
 * @defgroup modem_digi_cellular XBee Cellular
 *
 * @ingroup modem_digi
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_digi_cellular_notes Introduction
 *
 * **_All_** Digi _cellular_ modems can be implented as a DigiXBeeCellularTransparent
 * object - a subclass of DigiXBee and loggerModem.
 * The "transparent" refers to the Digi name for the operating mode of the module.
 * It is transparent in that data received by the module on the serial
 * interface is output directly the connected client - the module becomes
 * transparent allowing data to pass right through.
 *
 * The power requirements for the cellular XBee's depend on the specific module.
 * The 3G module *must* have **2 amps** of power available.
 * Most of the others can get by with ~1amp (which is still 2x what a standard
 * USB port of Arduino board can provide.)
 * The LTE-M module can just about almost get by on 500mA, but it definitely
 * not ideal.
 *
 * @note The u-blox based Digi XBee's (3G global and LTE-M global) may be more
 * stable used in bypass mode.
 * The Telit based Digi XBees (LTE Cat1 both Verizon and AT&T) can only use
 * this (transparent) mode.
 *
 * @section modem_digi_cellular_docs Manufacturer Documentation
 * The Digi product page for the various cellular modules is here:
 * https://www.digi.com/products/embedded-systems/digi-xbee/cellular-modems
 *
 * @section modem_digi_cellular_ctor Modem Constructor
 * {{ @ref DigiXBeeCellularTransparent::DigiXBeeCellularTransparent }}
 *
 * ___
 * @section modem_digi_cellular_examples Example Code
 * @subsection modem_digi_cellular_modem_obj Creating the Modem Object
 *
 * A transparent-mode Digi cellular module is used in the
 * @menulink{digi_xbee_cellular_transparent} example.
 *
 * @menusnip{digi_xbee_cellular_transparent}
 *
 * @subsection modem_digi_cellular_network LTE Network Selection
 *
 * For LTE boards, it is good practice to select which network you'll be
 * connecting to based on your SIM card and signal availability.
 * Example code for this can also be found in the
 * menulink{setup_xbeec_carrier} example.
 *
 * @menusnip{setup_xbeec_carrier}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEECELLULARTRANSPARENT_H_
#define SRC_MODEMS_DIGIXBEECELLULARTRANSPARENT_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeCellularTransparent"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_XBEE

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "TinyGsmClient.h"
#undef TINY_GSM_MODEM_HAS_WIFI
#include "DigiXBee.h"

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

/** @ingroup modem_digi_cellular */
/**@{*/

/**
 * @brief The class for any of
 * [Digi Cellular XBee or XBee3](@ref modem_digi) modules operating in Digi's
 * ["transparent" mode](@ref modem_digi_cellular).
 *
 * The "transparent" refers to the Digi name for the operating mode of the
 * module.
 *
 * @note The u-blox based Digi XBee's (3G global and LTE-M global) may be more
 * stable used in bypass mode.  The Telit based Digi XBees (LTE Cat1 both
 * Verizon and AT&T) can only use this mode.
 */
class DigiXBeeCellularTransparent : public DigiXBee {
 public:
    /**
     * @brief Construct a new Digi XBee Cellular Transparent object
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
     * @param user The user name, if required, associated with the APN;
     * optional, defaulting to NULL
     * @param pwd The password, if required, associated with the APN; optional,
     * defaulting to NULL
     *
     * @see DigiXBee::DigiXBee
     */
    DigiXBeeCellularTransparent(Stream* modemStream, int8_t powerPin,
                                int8_t statusPin, bool useCTSStatus,
                                int8_t modemResetPin, int8_t modemSleepRqPin,
                                const char* apn, const char* user = NULL,
                                const char* pwd = NULL);
    /**
     * @brief Destroy the Digi XBee Cellular Transparent object - no action
     * needed
     */
    ~DigiXBeeCellularTransparent();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    virtual Client* createClient() override;
    virtual void    deleteClient(Client* client);
    virtual Client* createSecureClient() override;
    virtual void    deleteSecureClient(Client* client);
    virtual Client* createSecureClient(
        SSLAuthMode sslAuthMode, SSLVersion sslVersion = SSLVersion::TLS1_2,
        const char* CAcertName = nullptr, const char* clientCertName = nullptr,
        const char* clientKeyName = nullptr) override;
    virtual Client*
    createSecureClient(const char* pskIdent, const char* psKey,
                       SSLVersion sslVersion = SSLVersion::TLS1_2) override;
    virtual Client*
    createSecureClient(const char* pskTableName,
                       SSLVersion  sslVersion = SSLVersion::TLS1_2) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                               int16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool updateModemMetadata(void) override;

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    /**
     * @brief Public reference to the TinyGSM modem.
     */
    TinyGsm gsmModem;

 protected:
    bool isInternetAvailable(void) override;
    bool modemWakeFxn(void) override;
    bool modemSleepFxn(void) override;
    /**
     * @copybrief loggerModem::extraModemSetup()
     *
     * For XBees, this sets the appropriate operating mode (transparent or
     * bypass), enables pin sleep, sets the DIO pins to the expected functions,
     * and reboots the modem to ensure all settings are applied.
     *
     * @return True if the extra setup succeeded.
     */
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;   ///< Internal reference to the cellular APN
    const char* _user;  ///< Internal reference to the APN's user name
    const char* _pwd;   ///< Internal reference to the APN's password
};
/**@}*/
#endif  // SRC_MODEMS_DIGIXBEECELLULARTRANSPARENT_H_

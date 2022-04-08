/**
 * @file DigiXBeeWifi.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DigiXBeeWifi subclass of the DigiXBee class for Digi S6B
 * Wifi XBee's.
 */
/* clang-format off */
/**
 * @defgroup modem_digi_wifi XBee Wifi S6B
 *
 * @ingroup modem_digi
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_digi_wifi_notes Introduction
 *
 * Digi's wifi XBee is implemented as a DigiXBeeWifi object - a subclass of
 * DigiXBee and loggerModem.
 * The S6B is run in Digi's "transparent" mode.
 * The maxiumum power draw is ~300mA.
 *
 * @section modem_digi_wifi_docs Manufacturer Documentation
 * The Digi product page for the S6B wifi module is here:
 * https://www.digi.com/products/embedded-systems/rf-modules/2-4-ghz-modules/xbee-wi-fi
 *
 * @section modem_digi_wifi_ctor Modem Constructor
 * {{ @ref DigiXBeeWifi::DigiXBeeWifi }}
 *
 * ___
 * @section modem_digi_wifi_examples Example Code
 * The digi_cellular is used in the @menulink{digi_xbee_lte_bypass} example.
 *
 * @menusnip{digi_xbee_lte_bypass}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEEWIFI_H_
#define SRC_MODEMS_DIGIXBEEWIFI_H_

// Debugging Statement
// #define MS_DIGIXBEEWIFI_DEBUG
// #define MS_DIGIXBEEWIFI_DEBUG_DEEP

#ifdef MS_DIGIXBEEWIFI_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeWifi"
#endif

/** @ingroup modem_digi_wifi */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_XBEE
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
#undef TINY_GSM_MODEM_HAS_GPRS
#include "DigiXBee.h"

#ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

/**
 * @brief The class for the [Digi XBee](@ref modem_digi)
 * [S6B wifi](@ref modem_digi_wifi) module operating in Digi's "transparent"
 * mode.
 */
class DigiXBeeWifi : public DigiXBee {
 public:
    /**
     * @brief Construct a new Digi XBee Wifi object
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
     * @param ssid The wifi network ID.
     * @param pwd The wifi network password, assuming WPA2.
     */
    DigiXBeeWifi(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                 bool useCTSStatus, int8_t modemResetPin,
                 int8_t modemSleepRqPin, const char* ssid, const char* pwd);
    /**
     * @brief Destroy the Digi XBee Wifi object - no action taken
     */
    ~DigiXBeeWifi();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool updateModemMetadata(void) override;

#ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
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
    const char* _ssid;
    const char* _pwd;
};
/**@}*/
#endif  // SRC_MODEMS_DIGIXBEEWIFI_H_

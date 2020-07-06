/**
 * @file DigiXBeeCellularTransparent.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DigiXBeeCellularTransparent class for Digi Cellular
 * XBee's operating in transparent mode.
 */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEECELLULARTRANSPARENT_H_
#define SRC_MODEMS_DIGIXBEECELLULARTRANSPARENT_H_

// Debugging Statement
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeCellularTransparent"
#endif

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
#undef TINY_GSM_MODEM_HAS_WIFI
#include "DigiXBee.h"

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

/**
 * @brief The class for any of Digi's cellular XBee or XBee3 modules operating
 * in Digi's "transparent" mode.
 *
 * The "transparent" refers to the Digi name for the operating mode of the
 * module.
 *
 * @note The u-blox based Digi XBee's (3G global and LTE-M global) may be more
 * stable used in bypass mode.  The Telit based Digi XBees (LTE Cat1 both
 * Verizon and AT&T) can only use this mode.
 *
 * @see #DigiXBee
 * @see @ref xbees_xbee_cellular_transparent
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
     *
     * @see DigiXBee::DigiXBee
     */
    DigiXBeeCellularTransparent(Stream* modemStream, int8_t powerPin,
                                int8_t statusPin, bool useCTSStatus,
                                int8_t modemResetPin, int8_t modemSleepRqPin,
                                const char* apn);
    /**
     * @brief Destroy the Digi XBee Cellular Transparent object - no action
     * needed
     */
    ~DigiXBeeCellularTransparent();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;
    uint32_t getNISTTimeOrig(void);
    uint32_t getTimeNTP(void);
    uint32_t getTimeCellTower(void);

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool updateModemMetadata(void) override;

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
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

  // Az Extensions
  void setApn(const char *APN, bool copyId = false);
  String getApn(void);

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
     * @return true The extra setup succeeded.
     * @return false The extra setup failed.
     */
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
    // Az extension
    char *_apn_buf = NULL; // Pointer to updated buffer
};

#endif  // SRC_MODEMS_DIGIXBEECELLULARTRANSPARENT_H_

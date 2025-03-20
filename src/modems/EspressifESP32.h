/**
 * @file EspressifESP32.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the EspressifESP32 subclass of loggerModem which _SHOULD_
 * work for essentially any breakout or variant of the ESP32 wifi/bluetooth chip
 * that has been flashed with Espressif's AT command firmware.
 */
/**
 * @defgroup modem_esp32 Espressif ESP32 and variants
 *
 * @ingroup modem_espressif
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_esp32_notes Introduction
 *
 * ESP32s are available everywhere. The AT commands are the same for all of
 * them. This library requires AT command firmware versions 3.2 or higher.  To
 * update to firmware 3.2+ from versions prior to 3.2, you must reflash the
 * firmware. You cannot update over-the-air.
 *
 * @section modem_esp32_dfrobot DFRobot ESPBee
 *
 * I don't actually recommend this module.
 * It gets hot and eats power for no apparent reason.
 * And the pin connections are a bit strange.
 * But it is cheap and available.
 * The pins are technically available to use the ESP's "light sleep" but I've
 * never successfully gotten the module to actually enter light sleep mode.
 * It always runs at full power draw.
 * It's not possible to use deep sleep on the DFRobot bee.
 *
 * @section modem_esp32_docs Manufacturer Documentation
 * And the page for the ESP32 is here:
 * https://www.espressif.com/en/products/socs/esp32
 *
 * @section modem_esp32_ctor Modem Constructor
 * {{ @ref EspressifESP32::EspressifESP32 }}
 *
 * ___
 * @section modem_esp32_examples Example Code
 * The ESP32 is used in the @menulink{espressif_esp32} example and the
 * [AWS IoT Core(@ref AWS_IOT_Core.ino) example.
 *
 * @menusnip{espressif_esp32}
 */

// Header Guards
#ifndef SRC_MODEMS_ESPRESSIFESP32_H_
#define SRC_MODEMS_ESPRESSIFESP32_H_

// Include config before anything else
#include "ModSensorConfig.h"

// Debugging Statement
// #define MS_ESPRESSIFESP32_DEBUG
// #define MS_ESPRESSIFESP32_DEBUG_DEEP

#if defined(MS_ESPRESSIFESP32_DEBUG)
#define MS_DEBUGGING_STD "EspressifESP32"
#endif

#if defined(MS_ESPRESSIFESP32_DEBUG_DEEP)
#define MS_DEBUGGING_DEEP "EspressifESP32"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_ESP32

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "TinyGsmClient.h"
#include "Espressif.h"

#if defined(MS_ESPRESSIFESP32_DEBUG_DEEP)
#include <StreamDebugger.h>
#endif

/** @ingroup modem_esp32 */

/**
 * @brief The loggerModem subclass for any breakout of the
 * [Espressif ESP32](@ref modem_esp32) wifi chip or ESP32 wifi/bluetooth
 * chip that has been flashed with Espressif's AT command firmware.
 *
 * @warning Light sleep modes on the ESP32 may not function as expected (or at
 * all).
 */
class EspressifESP32 : public Espressif {
 public:
    // Constructors/Destructor
    /**
     * @brief Construct a new Espressif ESP32 object.
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This is the ESP's `RSTB/DIO16` pin.
     * @param ssid The wifi network ID.
     * @param pwd The wifi network password, **assuming WPA2**.
     *
     * @see loggerModem::loggerModem
     */
    EspressifESP32(Stream* modemStream, int8_t powerPin, int8_t modemResetPin,
                   const char* ssid, const char* pwd);
    /**
     * @brief Destroy the Espressif ESP32 object - no action taken
     */
    ~EspressifESP32();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    virtual Client* createClient() override;
    virtual void    deleteClient(Client* _client);
    virtual Client* createSecureClient() override;
    virtual void    deleteSecureClient(Client* _client);
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

#ifdef MS_ESPRESSIFESP32_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    /**
     * @brief Public reference to the TinyGSM modem.
     */
    TinyGsm gsmModem;

 protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;
};
/**@}*/
#endif  // SRC_MODEMS_ESPRESSIFESP32_H_

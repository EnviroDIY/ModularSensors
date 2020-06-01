/**
 * @file EspressifESP8266.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the EspressifESP8266 subclass of loggerModem which SHOULD
 * work for essentially any breakout of the Espressif ESP8266 wifi chip or ESP32
 * wifi/bluetooth chip that has been flashed with Espressif's AT command
 * firmware.
 */

// Header Guards
#ifndef SRC_MODEMS_ESPRESSIFESP8266_H_
#define SRC_MODEMS_ESPRESSIFESP8266_H_

// Debugging Statement
// #define MS_ESPRESSIFESP8266_DEBUG
// #define MS_ESPRESSIFESP8266_DEBUG_DEEP

#ifdef MS_ESPRESSIFESP8266_DEBUG
#define MS_DEBUGGING_STD "EspressifESP8266"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_ESP8266
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * It is not possible to get status from the ESP8266 in deep sleep mode - during
 * deep sleep the pin state is undefined.
 *
 * For cases where a pin is defined for light sleep mode, the Espressif
 * documentation states:  since the system needs some time to wake up from light
 * sleep, it is suggested that wait at least 5ms before sending next AT command.
 * The documentation doesn't say anything about the time before the pin reaches
 * the expected level.  The status level during light sleep is user selectable,
 * but we set it low for wake and high for sleep.  Of course, despite being able
 * to configure light sleep mode for the module, it's not actually possible to
 * purposefully enter light sleep via AT commands, so we are dependent on the
 * module deciding it's been idle long enough and entering sleep on its own.  It
 * is a terrible system.  Use a deep-sleep with reset if possible.
 */
#define ESP8266_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #ESP8266_STATUS_LEVEL
 */
#define ESP8266_STATUS_TIME_MS 350

/**
 * @brief The loggerModem::_resetLevel.
 *
 * The ESP8266 is reset with a low pulse on pin 32.  The reset time is
 * undocumented but very fast - 1ms seems to be sufficient
 */
#define ESP8266_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #ESP8266_RESET_LEVEL
 */
#define ESP8266_RESET_PULSE_MS 1

// See notes above.. this is user configurable, but useless
/**
 * @brief The loggerModem::_wakeLevel.
 *
 * This light sleep wake level is user configurable on the ESP8266.  This
 * library uses a LOW level for wake.
 *
 * @note Light sleep modes on the ESP8266 may not function as expected (or at
 * all).
 */
#define ESP8266_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #ESP8266_WAKE_LEVEL
 */
#define ESP8266_WAKE_PULSE_MS 0

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * The ESP8266 turns on when power is applied regardless of pin states.  No
 * further wake command is needed.
 */
#define ESP8266_WAKE_DELAY_MS 0
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * The serial response time is undocumented for the ESP8266.  Other users online
 * esetimate about 350ms.
 */
#define ESP8266_ATRESPONSE_TIME_MS 350

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * The disconnect time for the ESP8266 is not documented (and the status pin
 * isn't valid) so this time is just an estimate.
 */
#define ESP8266_DISCONNECT_TIME_MS 500

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

/**
 * @brief The loggerModem subclass for any breakout of the Espressif ESP8266
 * wifi chip or ESP32 wifi/bluetooth chip that has been flashed with Espressif's
 * AT command firmware.
 *
 * @copydetails #ESP8266_STATUS_LEVEL
 * @copydetails #ESP8266_RESET_LEVEL
 * @copydetails #ESP8266_WAKE_LEVEL
 * @copydetails #ESP8266_WAKE_DELAY_MS
 * @copydetails #ESP8266_ATRESPONSE_TIME_MS
 * @copydetails #ESP8266_DISCONNECT_TIME_MS
 */
class EspressifESP8266 : public loggerModem {
 public:
    // Constructor/Destructor
    /**
     * @brief Construct a new Espressif ESP8266 object
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * @param ssid The wifi network ID.
     * @param pwd The wifi network password, assuming WPA2.
     * @param espSleepRqPin The DIO pin on the ESP8266 assigned to light sleep
     * wake.
     * @param espStatusPin The DIO pin on the ESP8566 assigned to status
     * indication.
     */
    EspressifESP8266(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                     int8_t modemResetPin, int8_t modemSleepRqPin,
                     const char* ssid, const char* pwd,
                     int8_t espSleepRqPin = -1, int8_t espStatusPin = -1);
    /**
     * @brief Destroy the Espressif ESP8266 object - no action taken
     */
    ~EspressifESP8266();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm       gsmModem;
    TinyGsmClient gsmClient;

    // Need the stream for tossing junk on boot
    Stream* _modemStream;

 protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    bool        ESPwaitForBoot(void);
    const char* _ssid;
    const char* _pwd;

    int8_t _espSleepRqPin;
    int8_t _espStatusPin;
};

#endif  // SRC_MODEMS_ESPRESSIFESP8266_H_

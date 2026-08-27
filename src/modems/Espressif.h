/**
 * @file Espressif.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Espressif subclass of loggerModem which is a parent class
 * for the ESP8266, the original ESP32, and the various ESP32 flavors.
 */
/* clang-format off */
/**
 * @defgroup modem_espressif Espressif ESP8266, ESP32, and various ESP32 flavors
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_espressif_notes Introduction
 *
 * These are handy, cheap, and very widely available WiFi and
 * WiFi/Bluetooth modules.
 * They use relatively little power amd can be put in a deep sleep mode to
 * greatly reduce power draw.
 * The types of sleep they can use are dependent on the breakout you have.
 * If you're shopping for boards, I strongly recommend getting a breakout that
 * exposes the deep sleep wake up (RSTB/DIO16) pin so that you can put the chip
 * in its lowest power mode.
 * When using the AT command firmware, it is not possible to purposefully enter
 * light sleep mode.
 * The module will enter light sleep on its own after some (undocumented) idle
 * period.
 * In my testing, I've never seen the module drop to the documented light sleep
 * power levels regardless of the length of time idle.
 *
 * @section modem_espressif_docs Manufacturer Documentation
 * More information on the ESP8266 is here:
 * https://www.espressif.com/en/products/socs/esp8266
 * And the page for the ESP32 is here:
 * https://www.espressif.com/en/products/socs/esp32
 *
 * @section modem_espressif_ctor Modem Constructor
 * {{ @ref Espressif::Espressif }}
 *
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_ESPRESSIF_H_
#define SRC_MODEMS_ESPRESSIF_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#if defined(MS_ESPRESSIF_DEBUG) || defined(MS_ESPRESSIFESP8266_DEBUG) || \
    defined(MS_ESPRESSIFESP32_DEBUG)
#define MS_DEBUGGING_STD "Espressif"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "LoggerModemImpl.h"

/** @ingroup modem_espressif */
/**@{*/

/**
 * @anchor modem_espressif_pins_timing
 * @name Modem Pin Settings and Timing
 * The timing and pin level settings for most Espressif modules
 */
/**@{*/

/**
 * @brief The loggerModem::_resetLevel.
 *
 * The Espressif is reset with a low pulse on the reset pin.  The reset time is
 * undocumented but very fast - 1ms seems to be sufficient
 */
#define ESPRESSIF_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #ESPRESSIF_RESET_LEVEL
 */
#define ESPRESSIF_RESET_PULSE_MS 1

// See notes above.. this is user configurable, but useless
/**
 * @brief The loggerModem::_wakeLevel.
 *
 * This light sleep wake level is user configurable on Espressif modules.  This
 * library uses a `LOW` level for wake.
 */
#define ESPRESSIF_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #ESPRESSIF_WAKE_LEVEL
 */
#define ESPRESSIF_WAKE_PULSE_MS 0

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Espressif modules turn on when power is applied regardless of pin states.  No
 * further wake command is needed.
 */
#define ESPRESSIF_WAKE_DELAY_MS 0
/**
 * @brief The minimum time before the Espressif module will spit out it's boot
 * log after power on or reset.
 */
#define ESPRESSIF_BOOT_LOG_DELAY_MS 200L
/**
 * @brief The maximum time to listen for the Espressif module's boot log after
 * power on or reset.
 */
#define ESPRESSIF_BOOT_LOG_TIMEOUT_MS 1000L
/**
 * @brief The loggerModem::_max_at_response_time_ms.
 *
 * The serial response time after boot (via power on or reset) is undocumented
 * for the ESP8266.  Other users online estimate about 350ms.
 *
 * In my fiddling, the ESP32 running AT firmware takes a bit longer; 850ms may
 * be safe.
 *
 * The serial response time on waking from light sleep is 5ms.
 */
#define ESPRESSIF_AT_RESPONSE_TIME_MS 850

/**
 * @brief The loggerModem::_disconnectTime_ms.
 *
 * The disconnect time for the ESP8266/ESP32 is not documented (and the status
 * pin isn't valid) so this time is just an estimate.
 */
#define ESPRESSIF_DISCONNECT_TIME_MS 500

/**
 * @brief The amount of time in ms it takes the ESP8266 to reconnect using saved
 * credentials.
 */
#define ESPRESSIF_RECONNECT_TIME_MS 2500
/**@}*/

/**
 * @brief The loggerModem subclass for almost any Espressif wifi or
 * wifi/bluetooth chip that has been flashed with Espressif's AT command
 * firmware.
 */
template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T>
class Espressif : public loggerModemImpl<GsmModemType_T,      // Modem Type
                                         ClientType_T,        // TCP Client Type
                                         SecureClientType_T,  // SSL Client
                                                              // Type
                                         true  // signal quality is RSSI
                                         > {
 public:
    // Constructors/Destructor
    /**
     * @brief Construct a new Espressif object.
     *
     * The constructor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @remark The Espressif modules do not have any status pin capabilites.  In
     * older firmware versions, AT commands were provided to set up pins to
     * check the status of the module, but these have been deprecated and
     * removed in newer firmware versions.  The status pin is not used in this
     * library.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc _powerPin
     * @param modemResetPin @copydoc _modemResetPin
     * This is the ESP's `RSTB/DIO16` pin.
     * @param ssid The wifi network ID.
     * @param pwd The wifi network password, **assuming WPA2**.
     * @param modemSleepRqPin @copydoc _modemSleepRqPin  On Espressif modules,
     * the sleep request pin *only applies to light sleep*.  You cannot select a
     * pin to wake from deep sleep.  The only pin that can wake from deep sleep
     * is the reset pin, which is handled by the `modemResetPin`
     * parameter.
     * @param espSleepRqPin the Espressif sleep request pin for light sleep.
     * This is a GPIO on the Espressif module, not a pin on the MCU.
     * @see loggerModem::loggerModem
     */
    Espressif(Stream* modemStream, int8_t powerPin, int8_t modemResetPin,
              const char* ssid, const char* pwd, int8_t modemSleepRqPin = -1,
              int8_t espSleepRqPin = -1)
        : loggerModemImpl<GsmModemType_T,      // Modem Type
                          ClientType_T,        // TCP Client Type
                          SecureClientType_T,  // SSL Client Type
                          true                 // signal quality is RSSI
                          >(
              modemStream, powerPin, modemSleepRqPin, LOW, modemResetPin,
              ESPRESSIF_RESET_LEVEL, ESPRESSIF_RESET_PULSE_MS, -1,
              ESPRESSIF_WAKE_LEVEL, ESPRESSIF_WAKE_PULSE_MS, 0,
              ESPRESSIF_DISCONNECT_TIME_MS, ESPRESSIF_WAKE_DELAY_MS,
              ESPRESSIF_AT_RESPONSE_TIME_MS),
          _modemStream(modemStream),
          _ssid(ssid),
          _pwd(pwd),
          _espSleepRqPin(espSleepRqPin) {}
    /**
     * @brief Destroy the Espressif object - no action taken
     */
    ~Espressif() override = default;

    /**
     * @brief A pointer to the Arduino serial Stream used for communication
     * between the MCU and the Espressif module.
     *
     * We need to keep the pointer to the stream for tossing junk on boot.
     */
    Stream* _modemStream;

 protected:
    bool connectWithCredentials() override {
        return this->gsmModem.networkConnect(this->_ssid, this->_pwd);
    }


    bool extraModemSetup() override {
        if (this->_modemSleepRqPin >= 0) {
            digitalWrite(this->_modemSleepRqPin, !this->_wakeLevel);
        }
        bool success = this->gsmModem.init();
        // Attempt to get the modem name even without a successful init
        // The full make and model won't be returned, but it will at least be
        // something that identifies the modem as an ESP32, which is helpful for
        // debugging.
        this->_modemName = this->gsmModem.getModemName();
        if (success) {
            // AT+CWCOUNTRY=<country_policy>,<country_code>,<start_channel>,<total_channel_count>
            // <country_policy>:
            //     0: will change the county code to be the same as the AP that
            //     the ESP32 is connected to. 1: the country code will not
            //     change, always be the one set by command.
            // <country_code>: country code. Maximum length: 3 characters. Refer
            // to
            //     ISO 3166-1 alpha-2 for country codes.
            // <start_channel>: the channel number to start. Range: [1,14].
            // <total_channel_count>: total number of channels.
            // We set the country code to default to US, but allow it to change
            // if the AP is in a different country.
            this->gsmModem.sendAT(GF("+CWCOUNTRY=0,\"US\",1,13"));
            success &= (this->gsmModem.waitResponse() == 1);
        }
#if 0
        // Make sure we're staying in station mode so sleep can happen
        this->gsmModem.sendAT(GF("+CWMODE_DEF=1"));
        gsmModem.waitResponse();
        // Make sure that, at minimum, modem-sleep is on
        this->gsmModem.sendAT(GF("+SLEEP=2"));
        this->gsmModem.waitResponse();
#endif
#if 0
        // Set the wifi settings as default
        // This will speed up connecting after resets
        this->gsmModem.sendAT(GF("+CWJAP_DEF=\""), _ssid, GF("\",\""), _pwd,
                              GF("\""));
        success = (this->gsmModem.waitResponse(30000L, GFP(GSM_OK), GF("FAIL\r\n")) != 1);
#endif
        return success;
    }

    bool modemWakeFxn() override {
        bool success = true;
        if (this->_powerPin >= 0) {  // Turns on when power is applied
            MS_DEEP_DBG(
                F("Power pin"), this->_powerPin,
                F("takes priority over reset pin, modem wakes on power on"));
            if (this->_modemSleepRqPin >= 0) {
                digitalWrite(this->_modemSleepRqPin, !this->_wakeLevel);
            }
            success &= this->ESPwaitForBoot();
            if (this->_modemSleepRqPin >= 0) {
                digitalWrite(this->_modemSleepRqPin, this->_wakeLevel);
            }
            return success;
        } else if (this->_modemResetPin >= 0) {
            MS_DBG(F("Sending a reset pulse to pin"), this->_modemResetPin,
                   F("to wake Espressif module from deep sleep"));
            digitalWrite(this->_modemResetPin, LOW);
            delay(this->_resetPulse_ms);
            digitalWrite(this->_modemResetPin, HIGH);
            if (this->_modemSleepRqPin >= 0) {
                digitalWrite(this->_modemSleepRqPin, !this->_wakeLevel);
            }
            success &= this->ESPwaitForBoot();
            if (this->_modemSleepRqPin >= 0) {
                digitalWrite(this->_modemSleepRqPin, this->_wakeLevel);
            }
            return success;
        } else if (this->_modemSleepRqPin >= 0) {
            MS_DBG(F("Setting pin"), this->_modemSleepRqPin,
                   this->_wakeLevel ? F("HIGH") : F("LOW"),
                   F("to wake Espressif module from light sleep"));
            digitalWrite(this->_modemSleepRqPin, this->_wakeLevel);
            return success;
        } else {
            MS_DEEP_DBG(F("No pins for waking the Espressif module. Hopefully "
                          "it's in the state you want."));
            return success;
        }
    }


 protected:
    /**
     * @brief Waits for the Espressif module to reboot and print out it's boot
     * up string. Because the boot up string is at a different baud rate (74880
     * baud), it usually comes out as junk.
     *
     * @return True if text (assumed to be the start message) was received;
     * false if text was received after boot.
     */
    bool ESPwaitForBoot() {
        // Wait for boot - finished when characters start coming
        // NOTE: After every "hard" reset (either power off or via RST-B), the
        // ESP sends out a boot log from the ROM on UART1 at 74880 baud.  We're
        // not going to worry about the odd baud rate since we're simply
        // throwing the characters away.
        MS_DBG(F("Waiting for boot-up message from Espressif module"));
        delay(ESPRESSIF_BOOT_LOG_DELAY_MS);  // It will take at least this long
        uint32_t start   = millis();
        bool     success = false;
        while (!_modemStream->available() &&
               millis() - start < ESPRESSIF_BOOT_LOG_TIMEOUT_MS) {
            yield();
        }
        if (_modemStream->available()) {
            success = true;
            // Read the boot log to empty it from the serial buffer
            while (_modemStream->available()) {
                _modemStream->read();
                delay(2);
            }
        }
        return success;
    }
    const char* _ssid;  ///< Internal reference to the WiFi SSID
    const char* _pwd;   ///< Internal reference to the WiFi password

    /// Internal reference to the Espressif sleep request pin for light sleep
    /// This is a GPIO on the espressif module, not a pin on the MCU.
    int8_t _espSleepRqPin = -1;
};
/**@}*/
#endif  // SRC_MODEMS_ESPRESSIF_H_

// cSpell:ignore RSTB

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

// Include config before anything else
#include "ModSensorConfig.h"

// Debugging Statement
// #define MS_ESPRESSIF_DEBUG

#if defined(MS_ESPRESSIF_DEBUG) || defined(MS_ESPRESSIFESP8266_DEBUG) || \
    defined(MS_ESPRESSIFESP32_DEBUG)
#define MS_DEBUGGING_STD "Espressif"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"

/** @ingroup modem_espressif */
/**@{*/

/**
 * @anchor modem_espressif_pins_timing
 * @name Modem Pin Settings and Timing
 * The timing and pin level settings for most Espressifs modules
 */
/**@{*/
/**
 * @brief The loggerModem::_statusLevel.
 *
 * It is not possible to get status from the Espressif modules in deep sleep
 * mode - during deep sleep the pin state is undefined.
 *
 * For cases where a pin is defined for light sleep mode, the Espressif
 * documentation states:
 * > since the system needs some time to wake up from light sleep, it is
 * > suggested that wait at least 5ms before sending next AT command.
 * The documentation doesn't say anything about the time before the pin reaches
 * the expected level.  The status level during light sleep is user selectable,
 * this library sets it low for wake and high for sleep.  Of course, despite
 * being able to configure light sleep mode for the module, it's not actually
 * possible to purposefully enter light sleep via AT commands, so we are
 * dependent on the module deciding it's been idle long enough and entering
 * sleep on its own.  It is a terrible system.  **Use a deep-sleep with reset if
 * possible.**
 */
#define ESPRESSIF_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #ESPRESSIF_STATUS_LEVEL
 */
#define ESPRESSIF_STATUS_TIME_MS 350

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
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * The serial response time after boot (via power on or reset) is undocumented
 * for the ESP8266.  Other users online estimate about 350ms.
 *
 * In my fiddling, the ESP32 running AT firmware takes a bit longer; 700ms may
 * be safe.
 *
 * The serial response time on waking from light sleep is 5ms.
 */
#define ESPRESSIF_ATRESPONSE_TIME_MS 700

/**
 * @brief The loggerModem::_disconnetTime_ms.
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
 *
 * @warning Light sleep modes on the ESP8266 may not function as expected (or at
 * all).
 */
class Espressif : public loggerModem {
 public:
    // Constructors/Destructor
    /**
     * @brief Construct a new Espressif object.
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
    Espressif(Stream* modemStream, int8_t powerPin, int8_t modemResetPin,
              const char* ssid, const char* pwd);
    /**
     * @brief Destroy the Espressif object - no action taken
     */
    virtual ~Espressif();

    /**
     * @brief A pointer to the Arduino serial Stream used for communication
     * between the MCU and the Espressif module.
     *
     * We need to keep the pointer to the stream for tossing junk on boot.
     */
    Stream* _modemStream;

 protected:
    bool modemWakeFxn(void) override;

 protected:
    /**
     * @brief Waits for the Espressif module to reboot and print out it's boot
     * up string. Because the boot up string is at a different baud rate (74880
     * baud), it usually comes out as junk.
     *
     * @return True if text (assumed to be the start message) was received;
     * false if text was received after boot.
     */
    bool        ESPwaitForBoot(void);
    const char* _ssid;  ///< Internal reference to the WiFi SSID
    const char* _pwd;   ///< Internal reference to the WiFi password
};
/**@}*/
#endif  // SRC_MODEMS_ESPRESSIF_H_

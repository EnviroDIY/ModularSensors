/**
 * @file EspressifESP8266.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the EspressifESP8266 subclass of loggerModem which _SHOULD_
 * work for essentially any breakout of the Espressif ESP8266 wifi chip or ESP32
 * wifi/bluetooth chip that has been flashed with Espressif's AT command
 * firmware.
 */
/* clang-format off */
/**
 * @defgroup modem_esp8266 Espressif ESP8266 and ESP32
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_esp8266_notes Introduction
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
 * @section modem_esp8266_dfrobot DFRobot ESPBee
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
 * @section modem_esp8266_docs Manufacturer Documentation
 * More information on the ESP8266 is here:
 * https://www.espressif.com/en/products/socs/esp8266
 * And the page for the ESP32 is here:
 * https://www.espressif.com/en/products/socs/esp32
 *
 * @section modem_esp8266_ctor Modem Constructor
 * {{ @ref EspressifESP8266::EspressifESP8266 }}
 *
 * ___
 * @section modem_esp8266_examples Example Code
 * The ESP8266 is used in the @menulink{espressif_esp8266} example and the
 * [logging to ThingSpeak](@ref logging_to_ThingSpeak.ino) example.
 *
 * @menusnip{espressif_esp8266}
 */
/* clang-format on */


// Header Guards
#ifndef SRC_MODEMS_ESPRESSIFESP8266_H_
#define SRC_MODEMS_ESPRESSIFESP8266_H_

// Debugging Statement
// #define MS_ESPRESSIFESP8266_DEBUG
// #define MS_ESPRESSIFESP8266_DEBUG_DEEP

#ifdef MS_ESPRESSIFESP8266_DEBUG
#define MS_DEBUGGING_STD "EspressifESP8266"
#endif

/** @ingroup modem_esp8266 */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_ESP8266
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * It is not possible to get status from the ESP8266 in deep sleep mode - during
 * deep sleep the pin state is undefined.
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
 * library uses a `LOW` level for wake.
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
 * The serial response time after boot (via power on or reset) is undocumented
 * for the ESP8266.  Other users online estimate about 350ms.
 *
 * In my fiddling, the ESP32 running AT firmware takes a bit longer; 700ms may
 * be safe.
 *
 * The serial response time on waking from light sleep is 5ms.
 */
#define ESP8266_ATRESPONSE_TIME_MS 700

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
 * @brief The loggerModem subclass for any breakout of the
 * [Espressif ESP8266](@ref modem_esp8266) wifi chip or ESP32 wifi/bluetooth
 * chip that has been flashed with Espressif's AT command firmware.
 *
 * @warning Light sleep modes on the ESP8266 may not function as expected (or at
 * all).
 */
class EspressifESP8266 : public loggerModem {
 public:
    // Constructors/Destructor
    /**
     * @brief Construct a new Espressif ESP8266 object.
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
    EspressifESP8266(Stream* modemStream, int8_t powerPin, int8_t modemResetPin,
                     const char* ssid, const char* pwd);
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

    /**
     * @brief Public reference to the TinyGSM modem.
     */
    TinyGsm gsmModem;
    /**
     * @brief Public reference to the TinyGSM Client.
     */
    TinyGsmClient gsmClient;

    /**
     * @brief A pointer to the Arduino serial Stream used for communication
     * between the MCU and the ESP8266.
     *
     * We need to keep the pointer to the stream for tossing junk on boot.
     */
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
};

/**
 * @brief typedef to avoid confusion for users
 */
typedef EspressifESP8266 EspressifESP32;

/**@}*/
#endif  // SRC_MODEMS_ESPRESSIFESP8266_H_

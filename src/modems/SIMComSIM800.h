/**
 * @file SIMComSIM800.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SIMComSIM800 subclass of loggerModem for Adafruit Fona
 * 2G, the Sodaq GPRSBeeR4 and almost any other module based on the SIMCOM
 * SIM800 or SIM900 modules and thier variants.
 */
/* clang-format off */
/**
 * @defgroup modem_sim800 SIMCom SIM800
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_sim800_notes Introduction
 *
 * There are a multitude of boards available that feature a variant of the
 * SIMCom SIM800 or the nearly identical SIM900, including the
 * [Adafruit Fona](whttps://www.adafruit.com/product/1946) Mini cellular GSM
 * breakout.
 * Almost all of those boards should work with ModularSensors as a generic
 * SIM800.
 * The one exception is the Sodaq GPRSBee **R6 and higher**, which has its own
 * [constructor](@ref modem_gprsbee).
 * The earlier Sodaq GPRSBee's (ie, R4) do use this version.
 *
 * The SIM800 consumes up to 2A of power while connecting to the network.
 * That is 4x what a typical USB or Arduino board can supply, so expect to give
 * the module it's own independent power source.
 *
 * The Adafruit _3G_ Fona is not currently supported.
 *
 * @section modem_sim800_docs Manufacturer Documentation
 * The module datasheet and AT commands are available here:
 * https://simcom.ee/modules/gsm-gprs/sim800/
 *
 * @section modem_sim800_ctor Modem Constructor
 * {{ @ref SIMComSIM800::SIMComSIM800 }}
 *
 * ___
 * @section modem_sim800_examples Example Code
 * The SIM800 is used in the @menulink{sim_com_sim800} example.
 *
 * @menusnip{sim_com_sim800}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_SIMCOMSIM800_H_
#define SRC_MODEMS_SIMCOMSIM800_H_

// Debugging Statement
// #define MS_SIMCOMSIM800_DEBUG
// #define MS_SIMCOMSIM800_DEBUG_DEEP

#ifdef MS_SIMCOMSIM800_DEBUG
#define MS_DEBUGGING_STD "SIMComSIM800"
#endif

/** @ingroup modem_sim800 */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SIM800
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * SIM800 status can be monitored on the `STATUS` pin which is active `HIGH`
 * Time after end pulse until status pin becomes active:
 *   - SIM800 - >3sec from start of 1s pulse
 *   - SIM900 - >2.2sec from end of pulse
 */
#define SIM800_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #SIM800_STATUS_LEVEL
 */
#define SIM800_STATUS_TIME_MS 3000

/**
 * @brief The loggerModem::_resetLevel.
 *
 * SIM800 is reset with a >105ms low pulse on the `RESET_N` pin
 */
#define SIM800_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #SIM800_RESET_LEVEL
 */
#define SIM800_RESET_PULSE_MS 105

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SIM800 is switched on by a > 1 second `LOW` pulse on the `PWR_ON` pin.
 * Module is switched on by a 1-3 second `LOW` pulse on the `PWR_ON` pin.
 *
 * @note Please monitor the status so on and off are correct!
 */
#define SIM800_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #SIM800_WAKE_LEVEL
 */
#define SIM800_WAKE_PULSE_MS 1100

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Time after power on before `PWRKEY` on SIM800 can be used is >0.4sec.
 */
#define SIM800_WAKE_DELAY_MS 450
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time after end pulse until serial port becomes active on SIM800 is >3sec from
 * start of 1s pulse.
 */
#define SIM800_ATRESPONSE_TIME_MS 3000

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * SIM800 power down (gracefully) takes >3sec.  We allow up to 15sec for
 * shutdown in case it is not monitored.
 */
#define SIM800_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SIMCOMSIM800_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for the Adafruit Fona 2G, the Sodaq GPRSBeeR4
 * and almost any other module based on the [SIMCOM SIM800 or SIM900 modules and
 * thier variants](@ref modem_sim800).
 */
class SIMComSIM800 : public loggerModem {
 public:
    /**
     * @brief Construct a new SIMComSIM800 object
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This is the pin labeled `STATUS` in SIMCom's integration guide.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This is the pin labeled `RESET` in SIMCom's integration guide.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This is the pin labeled `PWRKEY` in SIMCom's integration guide.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see loggerModem::loggerModem
     */
    SIMComSIM800(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                 int8_t modemResetPin, int8_t modemSleepRqPin, const char* apn);
    /**
     * @brief Destroy the SIMComSIM800 object - no action taken
     */
    ~SIMComSIM800();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SIMCOMSIM800_DEBUG_DEEP
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
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
};
/**@}*/
#endif  // SRC_MODEMS_SIMCOMSIM800_H_

/**
 * @file SIMComSIM7080.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SIMComSIM7080 subclass of loggerModem for
 * Botletics other modules based on the SIMCOM SIM7080.
 */
/* clang-format off */
/**
 * @defgroup modem_sim7080 SIMCom SIM7080
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_sim7080_notes Introduction
 *
 * The SIMCom [SIM7080G](http://www.simcom.com/product/SIM7080G.html)
 * is a Multi-Band CAT-M and NB-IoT module solution in a SMT type.
 *
 * @section modem_sim7080_docs Manufacturer Documentation
 * The module datasheet and AT commands are available here:
 * http://www.simcom.com/product/SIM7080G.html
 * @note You must create an account with SIMCOM to be able to download
 * the documents.
 *
 * @section modem_sim7080_ctor Modem Constructor
 * {{ @ref SIMComSIM7080::SIMComSIM7080 }}
 *
 * ___
 * @section modem_sim7080_examples Example Code
 * The SIM7080 is used in the @menulink{sim_com_sim7080} example.
 *
 * @menusnip{sim_com_sim7080}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_SIMCOMSIM7080_H_
#define SRC_MODEMS_SIMCOMSIM7080_H_

// Debugging Statement
// #define MS_SIMCOMSIM7080_DEBUG
// #define MS_SIMCOMSIM7080_DEBUG_DEEP

#ifdef MS_SIMCOMSIM7080_DEBUG
#define MS_DEBUGGING_STD "SIMComSIM7080"
#endif

/** @ingroup modem_sim7080 */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SIM7080
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * Status of the SIM7080G should be monitored on the `STATUS` pin, which is at a
 * high level when the module has powered on and the firmware goes ready.
 *
 * Time after start of pulse until status pin becomes active is >1.8sec.
 */
#define SIM7080_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #SIM7080_STATUS_LEVEL
 */
#define SIM7080_STATUS_TIME_MS 1800L

/**
 * @brief The loggerModem::_resetLevel.
 *
 * The SIM7080G is reset using the `PWRKEY` **NOT** a separate `RESET` pin!
 *
 * To reset the module, the `PWRKEY` is held low for 12.6s.
 */
#define SIM7080_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #SIM7080_RESET_LEVEL
 */
#define SIM7080_RESET_PULSE_MS 12600L

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SIM7080G Module is switched on by a >1 second `LOW` pulse on the `PWRKEY`
 * pin.
 *
 * @note Module is switched OFF by a >1.2 second `LOW` pulse on the `PWRKEY`
 * pin, so by using a pulse of >1 but <1.2 s to wake the SIM7080G and using AT
 * commands to put it to sleep, we should always be in the correct state, but if
 * at all possible the status pin should be monitored to confirm.
 *
 * @note A pulse of >12.6s on the `PWRKEY` resets the module.
 */
#define SIM7080_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #SIM7080_WAKE_LEVEL
 */
#define SIM7080_WAKE_PULSE_MS 1100
/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Time after power on before `PWRKEY` on SIM7080 can be used is undocumented.
 * Using 1s.
 */
#define SIM7080_WAKE_DELAY_MS 1000L
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time after end pulse until serial port on SIM7080 becomes active is >1.8sec.
 */
#define SIM7080_ATRESPONSE_TIME_MS 1800

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * SIM7080 power down (gracefully) takes 1.8-2 sec.
 */
#define SIM7080_DISCONNECT_TIME_MS 2000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SIMCOMSIM7080_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for modules based on the [SIMCOM
 * SIM7080](@ref modem_sim7080).
 */
class SIMComSIM7080 : public loggerModem {
 public:
    /**
     * @brief Construct a new SIMComSIM7080 object
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This is the pin labeled `STATUS` in SIMCom's integration guide.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This is the pin labeled `PWRKEY` in SIMCom's integration guide.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see loggerModem::loggerModem
     *
     * @note The SIM7080G does not have a `RESET` pin.  Resets are done using
     * the `PWRKEY`.
     */
    SIMComSIM7080(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                  int8_t modemSleepRqPin, const char* apn);
    /**
     * @brief Destroy the SIMComSIM7080 object - no action needed
     */
    ~SIMComSIM7080();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SIMCOMSIM7080_DEBUG_DEEP
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
#endif  // SRC_MODEMS_SIMCOMSIM7080_H_

/**
 * @file SIMComSIM7000.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SIMComSIM7000 subclass of loggerModem for
 * Botletics other modules based on the SIMCOM SIM7000.
 */

// Header Guards
#ifndef SRC_MODEMS_SIMCOMSIM7000_H_
#define SRC_MODEMS_SIMCOMSIM7000_H_

// Debugging Statement
// #define MS_SIMCOMSIM7000_DEBUG
// #define MS_SIMCOMSIM7000_DEBUG_DEEP

#ifdef MS_SIMCOMSIM7000_DEBUG
#define MS_DEBUGGING_STD "SIMComSIM7000"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SIM7000
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * Status of the SIM7000 should be monitored on the `STATUS` pin, which is at a
 * high level when the module has powered on and the firmware goes ready.
 *
 * Time after end pulse until status pin becomes active is >4.5sec.
 */
#define SIM7000_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #SIM7000_STATUS_LEVEL
 */
#define SIM7000_STATUS_TIME_MS 5000L

/**
 * @brief The loggerModem::_resetLevel.
 *
 * The active low level time impulse on `RESET` pin to reset SIM7000 is
 * miniumum 252ms.
 */
#define SIM7000_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #SIM7000_RESET_LEVEL
 */
#define SIM7000_RESET_PULSE_MS 300

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SIM7000 Module is switched on by a >1 second `LOW` pulse on the `PWRKEY`
 * pin.
 *
 * @note Module is switched OFF by a >1.2 second `LOW` pulse on the `PWRKEY`
 * pin, so by using a pulse of >1 but <1.2 s to wake the SIM7000 and using AT
 * commands to put it to sleep, we should always be in the correct state, but if
 * at all possible the status pin should be monitored to confirm.
 */
#define SIM7000_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #SIM7000_WAKE_LEVEL
 */
#define SIM7000_WAKE_PULSE_MS 1100
/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Time after power on before `PWRKEY` on SIM7000 can be used is undocumented.
 * Using 1s.
 */
#define SIM7000_WAKE_DELAY_MS 1000L
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time after end pulse until serial port on SIM7000 becomes active is >4.5sec.
 */
#define SIM7000_ATRESPONSE_TIME_MS 4500

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * SIM7000 power down (gracefully) takes 1.8-6.9 sec.
 */
#define SIM7000_DISCONNECT_TIME_MS 7000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for Botletics, And1, and other modules based
 * on the SIMCOM SIM7000.
 *
 * #### Pin and timing information for the SIM7000
 *
 * @copydetails #SIM7000_STATUS_LEVEL
 *
 * @copydetails #SIM7000_RESET_LEVEL
 *
 * @copydetails #SIM7000_WAKE_LEVEL
 *
 * @copydetails #SIM7000_WAKE_DELAY_MS
 *
 * @copydetails #SIM7000_ATRESPONSE_TIME_MS
 *
 * @copydetails #SIM7000_DISCONNECT_TIME_MS
 */
class SIMComSIM7000 : public loggerModem {
 public:
    /**
     * @brief Construct a new SIMComSIM7000 object
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
    SIMComSIM7000(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char* apn);
    /**
     * @brief Destroy the SIMComSIM7000 object - no action needed
     */
    ~SIMComSIM7000();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
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

#endif  // SRC_MODEMS_SIMCOMSIM7000_H_

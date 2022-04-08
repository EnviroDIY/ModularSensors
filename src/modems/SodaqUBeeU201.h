/**
 * @file SodaqUBeeU201.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SodaqUBeeU201 subclass of loggerModem for the Sodaq UBee
 * based on the u-blox SARA U201 3G cellular module.
 */
/* clang-format off */
/**
 * @defgroup modem_ubee_3g Sodaq UBee 2G, 3G, and other u-blox 2G and 3G Modules
 *
 * @ingroup modem_ublox
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_ubee_3g_notes Introduction
 *
 * There are a several of boards available that feature a variant of the
 * u-blox 2G and 3G modules, including a Sodaq UBee based on the SARA U201 and
 * another based on the SARA G3.
 *
 * The default baud rate for the SARA U201 is 9600.
 *
 * Power draw for most 2G and 3G modules is up to 2A.
 * The Sodaq UBee's provide extra battery sockets to supply the additional
 * power.
 * These _MUST_ be used.
 *
 * @section modem_ubee_3g_docs Manufacturer Documentation
 * The module datasheet and AT commands for the SARA U2 series are available here:
 * https://www.u-blox.com/en/product/sara-u2-series
 * The schematics for the UBee are available here:
 * https://support.sodaq.com/Shields_and_Bees/ubee/
 *
 * @section modem_ubee_3g_ctor Modem Constructor
 * {{ @ref SodaqUBeeU201::SodaqUBeeU201 }}
 *
 * ___
 * @section modem_ubee_3g_examples Example Code
 *
 * The SARA U201 based UBee is used in the @menulink{sodaq_ubee_u201} example.
 *
 * @menusnip{sodaq_ubee_u201}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_SODAQUBEEU201_H_
#define SRC_MODEMS_SODAQUBEEU201_H_

// Debugging Statement
// #define MS_SODAQUBEEU201_DEBUG
// #define MS_SODAQUBEEU201_DEBUG_DEEP

#ifdef MS_SODAQUBEEU201_DEBUG
#define MS_DEBUGGING_STD "SodaqUBeeU201"
#endif

/** @ingroup modem_ubee_3g */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_UBLOX
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif

/**
 * @brief The loggerModem::_statusLevel.
 *
 * Status of the U201 should be monitored on the `V_INT` pin.  The time after
 * end of wake pulse until `V_INT` becomes active is unspecified in
 * documentation; we use the value from the Lisa U2, which is 100ms.
 */
#define U201_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #U201_STATUS_LEVEL
 */
#define U201_STATUS_TIME_MS 100

/**
 * @brief The loggerModem::_resetLevel.
 *
 * U201 is reset with a >50ms low pulse on the `RESET_N` pin
 */
#define U201_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #U201_RESET_LEVEL
 */
#define U201_RESET_PULSE_MS 75

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SARA U201 is switched on by a 50-80 MICRO second `LOW` pulse on the
 * `PWR_ON` pin.
 */
#define U201_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #U201_WAKE_LEVEL
 */
#define U201_WAKE_PULSE_MS 1

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * The SARA U201 turns on when power is applied - the level of `PWR_ON` then is
 * irrelevant.  No separate pulse or other wake is needed.
 */
#define U201_WAKE_DELAY_MS 0
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time until system and digital pins are operational is 6 sec typical for SARA
 * U201.
 */
#define U201_ATRESPONSE_TIME_MS 6000L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Power down time for u-blox modules _"can largely vary depending on the
 * application / network settings and the concurrent module activities."_  The
 * `V_INT` pin should be monitored and power not withdrawn until that pin reads
 * low.  We allow up to 15 seconds for shutdown in case it is not monitored.
 */
#define U201_DISCONNECT_TIME_MS 15000L


// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for the [2G/3G](@ref modem_ubee_3g)
 * [Sodaq UBee](@ref modem_ublox) based on the u-blox SARA U201 2G/3G cellular
 * module.
 */
class SodaqUBeeU201 : public loggerModem {
 public:
    /**
     * @brief Construct a new Sodaq UBee U201 object
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * For the Sodaq UBee, this is the pin labeled `ON/OFF`; pin 9 on the bee
     * socket.  The fact that this pin controls the power to the u-blox module
     * is not clear in the Sodaq documentation.
     * @param statusPin @copydoc loggerModem::_statusPin
     * This is the pin labeled `V_INT` in the u-blox integration guide.  It is
     * (misleadingly) called `CTS` in some of the Sodaq UBee documentation
     * because Sodaq wired the `V_INT` from the u-blox to the pin usually
     * reserved for `CTS` on the bee socket.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This is the pin labeled `RESET_N` in both u-blox and Sodaq documentation.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This is the pin labeled `PWR_ON` in both u-blox and Sodaq
     * documentation.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see loggerModem::loggerModem
     */
    SodaqUBeeU201(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char* apn);
    /**
     * @brief Destroy the Sodaq UBee U201 object - no action taken
     */
    ~SodaqUBeeU201();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
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
#endif  // SRC_MODEMS_SODAQUBEEU201_H_

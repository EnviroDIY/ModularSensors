/**
 * @file SodaqUBeeR410M.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SodaqUBeeR410M subclass of loggerModem for the Sodaq UBee
 * based on the u-blox SARA R410M LTE-M cellular module.
 */

// Header Guards
#ifndef SRC_MODEMS_SODAQUBEER410M_H_
#define SRC_MODEMS_SODAQUBEER410M_H_

// Debugging Statement
// #define MS_SODAQUBEER410M_DEBUG
// #define MS_SODAQUBEER410M_DEBUG_DEEP

#ifdef MS_SODAQUBEER410M_DEBUG
#define MS_DEBUGGING_STD "SodaqUBeeR410M"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SARAR4
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * `V_INT` on the SARA R4 becomes active mid-way through on-pulse so it should
 * be instantly visible
 *
 * Status should be monitored on the `V_INT` pin
 */
#define R410M_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #R410M_STATUS_LEVEL
 */
#define R410M_STATUS_TIME_MS 0

/**
 * @brief The loggerModem::_resetLevel.
 *
 * R4 series are reset with a >10 SECOND low pulse on the `RESET_N` pin
 */
#define R410M_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #R410M_RESET_LEVEL
 */
#define R410M_RESET_PULSE_MS 10000L

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SARA R410M is switched on by a 0.15-3.2 second `LOW` pulse on the
 * `PWR_ON` pin
 */
#define R410M_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #R410M_WAKE_LEVEL
 */
#define R410M_WAKE_PULSE_MS 200
/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Time after power on before `PWR_ON` on SARA R4 can be used is nclear in
 * documentation!  Using 250ms.
 */
#define R410M_WAKE_DELAY_MS 250
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time until system and digital pins on SARA R4 are operational is ~4.5s.
 */
#define R410M_ATRESPONSE_TIME_MS 4500L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Power down time for u-blox modules _"can largely vary depending on the
 * application / network settings and the concurrent module activities."_  The
 * `V_INT` pin should be monitored and power not withdrawn until that pin reads
 * low.  We allow up to 15 seconds for shutdown in case it is not monitored.
 */
#define R410M_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SODAQUBEER410M_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for the Sodaq UBee based on the u-blox SARA
 * R410M LTE-M cellular module.  This can be also used for any other breakout of
 * the the u-blox R4 or N4 series modules.
 *
 * #### Pin and timing information for the R4 series
 *
 * @copydetails #R410M_STATUS_LEVEL
 *
 * @copydetails #R410M_RESET_LEVEL
 *
 * @copydetails #R410M_WAKE_LEVEL
 *
 * @copydetails #R410M_WAKE_DELAY_MS
 *
 * @copydetails #R410M_ATRESPONSE_TIME_MS
 *
 * @copydetails #R410M_DISCONNECT_TIME_MS
 *
 * @see @ref ubee_page
 */
class SodaqUBeeR410M : public loggerModem {
 public:
    // Constructor/Destructor

#if F_CPU == 8000000L
    /**
     * @brief Construct a new Sodaq UBee R410M object
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
     * @note The SARA R410N DOES NOT save baud rate to non-volatile memory.
     * After every power loss, the module will return to the default baud rate
     * of 115200.  This library attempts to compensate by sending a baud rate
     * change command in the wake function. Because of this, 8MHz boards, LIKE
     * THE MAYFLY, *MUST* use a HardwareSerial instance as modemSerial.  This
     * requirement does not exist for 16MHz (and faster) boards.  For those, any
     * Stream instance may be used.
     *
     * @see loggerModem::loggerModem
     */
    SodaqUBeeR410M(HardwareSerial* modemStream, int8_t powerPin,
                   int8_t statusPin, int8_t modemResetPin,
                   int8_t modemSleepRqPin, const char* apn);
#else
    /**
     * @brief Construct a new Sodaq UBee R410M object
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * _For the Sodaq UBee, this is the pin labeled `ON/OFF`; pin 9 on the bee
     * socket._  The fact that this pin controls the power to the u-blox module
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
    SodaqUBeeR410M(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   const char* apn);
#endif
    /**
     * @brief Destroy the Sodaq UBee R410M object - no action needed
     */
    ~SodaqUBeeR410M();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

#ifdef MS_SODAQUBEER410M_DEBUG_DEEP
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

#if F_CPU == 8000000L
    /**
     * @brief The HardwareSerial instance used for communication between the MCU
     * and the modem.
     */
    HardwareSerial* _modemSerial;
#endif

 protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
};

#endif  // SRC_MODEMS_SODAQUBEER410M_H_

/**
 * @file LoggerModemImpl.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the template implementations for loggerModemImpl subclasses
 * using CRTP mixins.
 */
/**
 * @defgroup the_modems Supported Modems and Communication Modules
 * All implemented loggerModem classes
 *
 * @copydetails loggerModem
 *
 * @see @ref page_modem_notes
 */

// Header Guards
#ifndef SRC_LOGGERMODEMIMPL_H_
#define SRC_LOGGERMODEMIMPL_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEMIMPL_DEBUG
#define MS_DEBUGGING_STD "LoggerModemImpl"
#endif
#ifdef MS_LOGGERMODEMIMPL_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModemImpl"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "LoggerModem.h"
#include "ClockSupport.h"
#include "VariableBase.h"

#include <Arduino.h>
#include <Client.h>

// Include the TinyGSM capabilities file to check what the modem can do
#include <TinyGsmCapabilities.h>
// Include SSL types for secure client creation
#include <TinyGsmSSL.tpp>

// Include the CRTP mixin files
#include "LoggerModemPowerMixin.h"
#include "LoggerModemCommMixin.h"
#include "LoggerModemSensingMixin.h"


/**
 * @brief Template implementation class for modems.
 *
 * This template class uses the Curiously Recurring Template Pattern (CRTP)
 * to compose modem functionality from three independent mixin classes:
 * - loggerModemPowerMixin: Power management and pin control
 * - loggerModemCommMixin: Internet connectivity and client management
 * - loggerModemSensingMixin: Modem metadata polling
 *
 * Derived classes must implement the pure virtual functions:
 * - modemWakeFxn()
 * - modemSleepFxn()
 *
 * @tparam GsmModemType_T The TinyGSM modem type (e.g., TinyGsmBG96)
 * @tparam ClientType_T The TinyGSM client type (e.g.,
 * TinyGsmBG96::GsmClientBG96)
 * @tparam SecureClientType_T The TinyGSM secure client type (e.g.,
 * TinyGsmBG96::GsmClientSecureBG96)
 * @tparam signalQualityIsRSSI Whether signal quality is RSSI (true) or CSQ
 * (false)
 */
template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI = false>
class loggerModemImpl
    : public loggerModem,
      public loggerModemPowerMixin<
          loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                          signalQualityIsRSSI>>,
      public loggerModemCommMixin<
          loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                          signalQualityIsRSSI>,
          GsmModemType_T, ClientType_T, SecureClientType_T>,
      public loggerModemSensingMixin<
          loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                          signalQualityIsRSSI>,
          GsmModemType_T> {
    // Friend declarations to allow mixins to access protected members
    friend class loggerModemPowerMixin<loggerModemImpl<
        GsmModemType_T, ClientType_T, SecureClientType_T, signalQualityIsRSSI>>;
    friend class loggerModemCommMixin<
        loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                        signalQualityIsRSSI>,
        GsmModemType_T, ClientType_T, SecureClientType_T>;
    friend class loggerModemSensingMixin<
        loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                        signalQualityIsRSSI>,
        GsmModemType_T>;

 public:
    // Type members - use different names to avoid shadowing template parameters
    /// The type of the TinyGSM modem used by this loggerModemImpl
    using GsmModemType = GsmModemType_T;
    /// The type of the TinyGSM client used by this loggerModemImpl
    using ClientType = ClientType_T;
    /// The type of the TinyGSM secure client used by this loggerModemImpl
    using SecureClientType = SecureClientType_T;

    // Use TinyGSM's integral_constant for tag dispatch pattern
    /// The type of the signal quality measurement used by this loggerModemImpl
    /// - true if RSSI, false if CSQ
    typedef TinyGsmCapabilities::integral_constant<bool, signalQualityIsRSSI>
        SignalQualityIsRSSI;

    /**
     * @brief The TinyGSM modem object
     *
     * This must be publicly accessible for derived classes and user code.
     */
    GsmModemType gsmModem;

    /**
     * @brief Construct a new loggerModemImpl object.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin The digital pin number to control modem power (-1 if not
     * applicable)
     * @param statusPin The digital pin number to read modem status (-1 if not
     * applicable)
     * @param statusLevel The digital level (HIGH/LOW) that indicates "on"
     * status
     * @param modemResetPin The digital pin number to reset the modem (-1 if not
     * applicable)
     * @param resetLevel The digital level (HIGH/LOW) for reset pulse
     * @param resetPulse_ms Duration of reset pulse in milliseconds
     * @param modemSleepRqPin The digital pin number for sleep request (-1 if
     * not applicable)
     * @param wakeLevel The digital level (HIGH/LOW) for wake pulse
     * @param wakePulse_ms Duration of wake pulse in milliseconds
     * @param max_status_time_ms Maximum time to wait for status indication
     * @param max_disconnectTime_ms Maximum time for graceful disconnect
     * @param wakeDelayTime_ms Warm-up time after power-on
     * @param max_at_response_time_ms Maximum time to wait for AT command
     * response
     */
    loggerModemImpl(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                    bool statusLevel, int8_t modemResetPin, bool resetLevel,
                    uint32_t resetPulse_ms, int8_t modemSleepRqPin,
                    bool wakeLevel, uint32_t wakePulse_ms,
                    uint32_t max_status_time_ms, uint32_t max_disconnectTime_ms,
                    uint32_t wakeDelayTime_ms, uint32_t max_at_response_time_ms)
        : gsmModem(*modemStream),
          _max_at_response_time_ms(max_at_response_time_ms),
          _hasBeenSetup(false) {
        // Initialize power mixin members
        this->_powerPin          = powerPin;
        this->_statusPin         = statusPin;
        this->_statusLevel       = statusLevel;
        this->_modemResetPin     = modemResetPin;
        this->_resetLevel        = resetLevel;
        this->_resetPulse_ms     = resetPulse_ms;
        this->_modemSleepRqPin   = modemSleepRqPin;
        this->_wakeLevel         = wakeLevel;
        this->_wakePulse_ms      = wakePulse_ms;
        this->_statusTime_ms     = max_status_time_ms;
        this->_disconnectTime_ms = max_disconnectTime_ms;
        this->_wakeDelayTime_ms  = wakeDelayTime_ms;
        this->_millisPowerOn     = 0;
        this->_modemLEDPin       = -1;
        // Initialize comm mixin members
        this->_modemUTCOffset  = 0;
        this->_lastNISTrequest = 0;
    }

    /**
     * @brief Destroy the loggerModemImpl object - no action taken.
     */
    virtual ~loggerModemImpl() = default;

    /* ===================================================================== */
    /* Virtual override functions that delegate to mixin implementations    */
    /* These satisfy the pure virtual functions from loggerModem base class */
    /* ===================================================================== */

    // Power management functions from loggerModemPowerMixin
    void setModemLED(int8_t modemLEDPin) override {
        this->setModemLEDImpl(modemLEDPin);
    }
    void modemPowerUp() override {
        this->modemPowerUpImpl();
    }
    void modemPowerDown() override {
        this->modemPowerDownImpl();
    }
    bool modemWake() override {
        return this->modemWakeImpl();
    }
    bool modemSleep() override {
        return this->modemSleepImpl();
    }
    bool modemSleepPowerDown() override {
        return this->modemSleepPowerDownImpl();
    }
    bool modemHardReset() override {
        return this->modemHardResetImpl();
    }
    bool isModemAwake() override {
        return this->isModemAwakeImpl();
    }
    void setModemStatusLevel(bool level) override {
        this->setModemStatusLevelImpl(level);
    }
    void setModemWakeLevel(bool level) override {
        this->setModemWakeLevelImpl(level);
    }
    void setModemResetLevel(bool level) override {
        this->setModemResetLevelImpl(level);
    }

    // Communication and setup functions from loggerModemCommMixin
    bool connectInternet(uint32_t maxConnectionTime = 50000L) override {
        return this->connectInternetImpl(maxConnectionTime);
    }
    void disconnectInternet() override {
        this->disconnectInternetImpl();
    }
    bool isInternetAvailable() override {
        return this->isInternetAvailableImpl();
    }
    void setModemTimeZone(int8_t timeZone) override {
        this->setModemTimeZoneImpl(timeZone);
    }
    uint32_t getNISTTime() override {
        return this->getNISTTimeImpl();
    }

    // TCP client functions from loggerModemCommMixin
    Client* createClient(uint8_t mux = 0) override {
        return this->createClientImpl(mux);
    }
    void deleteClient(Client* client) override {
        this->deleteClientImpl(client);
    }

    // Secure SSL/TLS client functions from loggerModemCommMixin
    Client* createSecureClient(uint8_t mux = 0) override {
        return this->createSecureClientImpl(mux);
    }
    Client* createSecureClient(uint8_t mux, SSLAuthMode sslAuthMode,
                               SSLVersion  sslVersion     = SSLVersion::TLS1_2,
                               const char* CAcertName     = nullptr,
                               const char* clientCertName = nullptr,
                               const char* clientKeyName  = nullptr) override {
        return this->createSecureClientImpl(mux, sslAuthMode, sslVersion,
                                            CAcertName, clientCertName,
                                            clientKeyName);
    }
    Client* createSecureClient(SSLAuthMode sslAuthMode,
                               SSLVersion  sslVersion     = SSLVersion::TLS1_2,
                               const char* CAcertName     = nullptr,
                               const char* clientCertName = nullptr,
                               const char* clientKeyName  = nullptr) override {
        return this->createSecureClientImpl(sslAuthMode, sslVersion, CAcertName,
                                            clientCertName, clientKeyName);
    }
    Client* createSecureClient(
        uint8_t mux, const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) override {
        return this->createSecureClientImpl(mux, pskIdent, psKey, sslVersion);
    }
    Client* createSecureClient(
        const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) override {
        return this->createSecureClientImpl(pskIdent, psKey, sslVersion);
    }
    Client* createSecureClient(
        uint8_t mux, const char* pskTableName,
        SSLVersion sslVersion = SSLVersion::TLS1_2) override {
        return this->createSecureClientImpl(mux, pskTableName, sslVersion);
    }
    Client* createSecureClient(
        const char* pskTableName,
        SSLVersion  sslVersion = SSLVersion::TLS1_2) override {
        return this->createSecureClientImpl(pskTableName, sslVersion);
    }

    void deleteSecureClient(Client* client) override {
        this->deleteSecureClientImpl(client);
    }

    // Metadata functions from loggerModemSensingMixin
    bool getModemSignalQuality(int16_t& rssi, int16_t& percent) override {
        return this->getModemSignalQualityImpl(rssi, percent);
    }
    bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                              int16_t& milliVolts) override {
        return this->getModemBatteryStatsImpl(chargeState, percent, milliVolts);
    }
    float getModemChipTemperature() override {
        return this->getModemChipTemperatureImpl();
    }


    /**
     * @brief Set up pin modes for all modem control pins.
     *
     * This is called during wake to ensure pin modes are set correctly.
     */
    void setModemPinModes() {
        // Set-up pin modes
        if (this->_statusPin >= 0) {
            MS_DEEP_DBG(F("Initializing pin"), this->_statusPin,
                        F("for modem status with on level expected to be"),
                        this->_statusLevel ? F("HIGH") : F("LOW"));
            pinMode(this->_statusPin, INPUT);
        }
        if (this->_modemSleepRqPin >= 0) {
            MS_DEEP_DBG(F("Initializing pin"), this->_modemSleepRqPin,
                        F("for modem sleep with starting value"),
                        !this->_wakeLevel ? F("HIGH") : F("LOW"));
            pinMode(this->_modemSleepRqPin, OUTPUT);
            digitalWrite(this->_modemSleepRqPin, !this->_wakeLevel);
        }
        if (this->_modemResetPin >= 0) {
            MS_DEEP_DBG(F("Initializing pin"), this->_modemResetPin,
                        F("for modem reset with starting value"),
                        !this->_resetLevel ? F("HIGH") : F("LOW"));
            pinMode(this->_modemResetPin, OUTPUT);
            digitalWrite(this->_modemResetPin, !this->_resetLevel);
        }
        if (this->_modemLEDPin >= 0) {
            MS_DEEP_DBG(F("Initializing pin"), this->_modemLEDPin,
                        F("for modem status LED with starting value 0"));
            pinMode(this->_modemLEDPin, OUTPUT);
            digitalWrite(this->_modemLEDPin, LOW);
        }
    }

    /* ===================================================================== */
    /* Setup                                                                 */
    /* ===================================================================== */
 public:
    bool modemSetup() override {
        bool success = true;
        MS_DBG(F("Starting modem setup for"), getModemName());

        // Confirm that the modem responds to AT commands
        {
            MS_START_DEBUG_TIMER;
            MS_DBG(F("Waiting up to"), _max_at_response_time_ms,
                   F("ms for modem to respond to AT commands..."));
            success &= gsmModem.testAT(_max_at_response_time_ms + 500);
            if (success) {
                MS_DBG(F("... modem responded after"), MS_PRINT_DEBUG_TIMER,
                       F("milliseconds."));
            } else {
                MS_DBG(F("... modem did not respond!"));
                return false;
            }
        }

        // Get modem name
        _modemName = gsmModem.getModemName();
        MS_DBG(F("Modem is a:"), _modemName);

        // Initialize the modem
        MS_START_DEBUG_TIMER;
        MS_DBG(F("Initializing modem..."));
        success &= gsmModem.init();
        if (success) {
            MS_DBG(F("Modem init complete after"), MS_PRINT_DEBUG_TIMER,
                   F("milliseconds."));
        } else {
            MS_DBG(F("Modem init failed!"));
            return false;
        }

        // Get modem info
        _modemHwVersion    = gsmModem.getModemModel();
        _modemFwVersion    = gsmModem.getModemRevision();
        _modemSerialNumber = gsmModem.getModemSerialNumber();
        MS_DBG(F("Modem HW:"), _modemHwVersion);
        MS_DBG(F("Modem FW:"), _modemFwVersion);
        MS_DBG(F("Modem SN:"), _modemSerialNumber);

        // Perform extra modem-specific setup
        success &= extraModemSetup();

        if (success) {
            _hasBeenSetup = true;
            MS_DBG(F("... setup complete!  It's a"), getModemName());
        } else {
            MS_DBG(F("... setup failed!  It's a"), getModemName());
        }

        return success;
    }


    /* ===================================================================== */
    /* Modem Identification Functions                                        */
    /* ===================================================================== */
 public:
    String getModemName() override {
        return _modemName;
    }

    String getModemDevId() override {
        String devId = getModemName() + F(" HW v") + _modemHwVersion +
            F(" FW v") + _modemFwVersion + F(" SN:") + _modemSerialNumber;
        MS_DBG(F("Modem Device ID:"), devId);
        return devId;
    }


    /* ===================================================================== */
    /* Internal Members                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_member_variables
     * @name Modem member variables
     */
    /**@{*/
 protected:

    /**
     * @brief The modem name
     *
     * Set in the init() portion of the #modemSetup().
     * Returned by #getModemName().
     */
    String _modemName = F("unspecified modem");

    /**
     * @brief The modem hardware version.
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String _modemHwVersion;

    /**
     * @brief The modem firmware version.
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String _modemFwVersion;

    /**
     * @brief The modem serial number
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     */
    String _modemSerialNumber;

    /**
     * @brief The maximum time in ms to wait for AT command response
     */
    uint32_t _max_at_response_time_ms;

    /**
     * @brief Flag indicating whether modem setup has been run
     */
    bool _hasBeenSetup;

    /**
     * @brief Perform modem-specific wake sequence
     *
     * @return True if wake succeeded
     */
    virtual bool modemWakeFxn() = 0;

    /**
     * @brief Perform modem-specific sleep sequence
     *
     * @return True if sleep succeeded
     */
    virtual bool modemSleepFxn() = 0;
    /**
     * @brief Perform the parts of the modem set up process that are unique to a
     * specific module, as opposed to the parts of setup that are common to all
     * modem modules.
     * @return True if extra setup succeeded; false otherwise.
     */
    virtual bool extraModemSetup() = 0;
};

#endif  // SRC_LOGGERMODEMIMPL_H_

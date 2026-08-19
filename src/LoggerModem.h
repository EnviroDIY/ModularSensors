/**
 * @file LOGGERMODEM.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains type type-erased loggerModem class and the template
 * implementations for loggerModemImpl subclasses using CRTP mixins.
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
#ifndef SRC_LOGGERMODEM_H_
#define SRC_LOGGERMODEM_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEM_DEBUG
#define MS_DEBUGGING_STD "LoggerModem"
#endif
#ifdef MS_LOGGERMODEM_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModem"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "ClockSupport.h"
#include "VariableBase.h"

#include <Arduino.h>
#include <Client.h>

// Include the TinyGSM capabilities file to check what the modem can do
#include <TinyGsmCapabilities.h>
// Include SSL types for secure client creation
#include <TinyGsmSSL.tpp>

// Bit masks for enabling modem metadata polling
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable RSSI
/// polling.
#define MODEM_RSSI_ENABLE_BITMASK 0b00000001
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable percent
/// signal polling.
#define MODEM_PERCENT_SIGNAL_ENABLE_BITMASK 0b00000010
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable battery
/// state polling.
#define MODEM_BATTERY_STATE_ENABLE_BITMASK 0b00000100
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable battery
/// percent polling.
#define MODEM_BATTERY_PERCENT_ENABLE_BITMASK 0b00001000
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable battery
/// voltage polling.
#define MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK 0b00010000
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable
/// temperature polling.
#define MODEM_TEMPERATURE_ENABLE_BITMASK 0b00100000
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable activation
/// status polling.
#define MODEM_ACTIVATION_ENABLE_BITMASK 0b01000000
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable powered
/// status polling.
#define MODEM_POWERED_ENABLE_BITMASK 0b10000000

// Forward declare template class
template <typename GsmModemType, typename ClientType, typename SecureClientType,
          bool signalQualityIsRSSI>
class loggerModemImpl;

/**
 * @brief Base class for all modems - provides common interface.
 *
 * This class provides a common interface that allows LoggerBase and other
 * non-template code to hold pointers to modem objects regardless of their
 * specific template instantiation. All public-facing modem functionality
 * must be declared as virtual functions in this base class.
 *
 * This class provides type erasure for the template-based loggerModemImpl
 * class, allowing non-template code to interact with modem objects without
 * knowing their specific template parameters.
 */
class loggerModem {
 public:
    virtual ~loggerModem() = default;

    // Setup and identification
    virtual bool   modemSetup()    = 0;
    virtual String getModemName()  = 0;
    virtual String getModemDevId() = 0;

    // Power management
    virtual void modemPowerUp()        = 0;
    virtual bool modemWake()           = 0;
    virtual bool modemSleep()          = 0;
    virtual bool modemSleepPowerDown() = 0;
    virtual bool modemHardReset()      = 0;
    virtual bool isModemAwake()        = 0;

    // Internet connectivity
    virtual bool     connectInternet(uint32_t maxConnectionTime = 50000L) = 0;
    virtual void     disconnectInternet()                                 = 0;
    virtual bool     isInternetAvailable()                                = 0;
    virtual uint32_t getNISTTime()                                        = 0;

    // Client management
    virtual Client* createClient(uint8_t mux = 0) = 0;
    virtual void    deleteClient(Client* client)  = 0;

    // Multiple overloads for createSecureClient matching CommMixin signatures
    virtual Client* createSecureClient(uint8_t mux = 0) = 0;
    virtual Client* createSecureClient(
        uint8_t mux, SSLAuthMode sslAuthMode,
        SSLVersion  sslVersion = SSLVersion::TLS1_2,
        const char* CAcertName = nullptr, const char* clientCertName = nullptr,
        const char* clientKeyName = nullptr) = 0;
    virtual Client* createSecureClient(
        SSLAuthMode sslAuthMode, SSLVersion sslVersion = SSLVersion::TLS1_2,
        const char* CAcertName = nullptr, const char* clientCertName = nullptr,
        const char* clientKeyName = nullptr) = 0;
    virtual Client* createSecureClient(
        uint8_t mux, const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) = 0;
    virtual Client* createSecureClient(
        const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) = 0;
    virtual Client* createSecureClient(
        uint8_t mux, const char* pskTableName,
        SSLVersion sslVersion = SSLVersion::TLS1_2) = 0;
    virtual Client* createSecureClient(
        const char* pskTableName,
        SSLVersion  sslVersion = SSLVersion::TLS1_2) = 0;

    virtual void deleteSecureClient(Client* client) = 0;

    // Metadata polling
    virtual void updateModemMetadata()                  = 0;
    virtual void enableMetadataPolling(uint8_t bitmask) = 0;

    // Time zone
    virtual void setModemTimeZone(int8_t timeZone) = 0;

    // Metadata getters (for Variable classes)
    virtual float getModemRSSI()                 = 0;
    virtual float getModemSignalPercent()        = 0;
    virtual float getModemBatteryChargeState()   = 0;
    virtual float getModemBatteryChargePercent() = 0;
    virtual float getModemBatteryVoltage()       = 0;
    virtual float getModemTemperature()          = 0;
};

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
 * - extraModemSetup()
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
                          signalQualityIsRSSI>> {
    // Friend declarations to allow mixins to access protected members
    friend class loggerModemPowerMixin<loggerModemImpl<
        GsmModemType_T, ClientType_T, SecureClientType_T, signalQualityIsRSSI>>;
    friend class loggerModemCommMixin<
        loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                        signalQualityIsRSSI>,
        GsmModemType_T, ClientType_T, SecureClientType_T>;
    friend class loggerModemSensingMixin<loggerModemImpl<
        GsmModemType_T, ClientType_T, SecureClientType_T, signalQualityIsRSSI>>;

 public:
    // Type members - use different names to avoid shadowing template parameters
    using GsmModemType     = GsmModemType_T;
    using ClientType       = ClientType_T;
    using SecureClientType = SecureClientType_T;

    // Use TinyGSM's integral_constant for tag dispatch pattern
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
        // Initialize sensing mixin members
        this->_pollModemMetaData = 0;
        // Initialize comm mixin members
        this->_modemUTCOffset  = 0;
        this->_lastNISTrequest = 0;
    }

    /**
     * @brief Destroy the loggerModemImpl object - no action taken.
     */
    virtual ~loggerModemImpl() = default;

    // Bring mixin implementations into scope to satisfy base class pure
    // virtuals Power management functions from loggerModemPowerMixin
    using loggerModemPowerMixin<loggerModemImpl>::modemPowerUp;
    using loggerModemPowerMixin<loggerModemImpl>::modemWake;
    using loggerModemPowerMixin<loggerModemImpl>::modemSleep;
    using loggerModemPowerMixin<loggerModemImpl>::modemSleepPowerDown;
    using loggerModemPowerMixin<loggerModemImpl>::isModemAwake;

    // Communication functions from loggerModemCommMixin
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::connectInternet;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::disconnectInternet;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::isInternetAvailable;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::createClient;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::deleteClient;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::createSecureClient;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::deleteSecureClient;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::getNISTTime;
    using loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                               SecureClientType_T>::setModemTimeZone;

    // Metadata functions from loggerModemSensingMixin
    using loggerModemSensingMixin<loggerModemImpl>::updateModemMetadata;
    using loggerModemSensingMixin<loggerModemImpl>::enableMetadataPolling;


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

 public:
    /* ===================================================================== */
    /* Setup                                                                 */
    /* ===================================================================== */
    /**
     * @brief Set up the modem before first use.
     *
     * This performs TinyGSM initialization and any modem-specific setup.
     *
     * @return True if setup was successful
     */
    virtual bool modemSetup() {
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
        {
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
        }

        // Get modem info
        _modemHwVersion    = gsmModem.getModemModel();
        _modemFwVersion    = gsmModem.getModemRevision();
        _modemSerialNumber = gsmModem.getIMEI();
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

    /**
     * @brief Backwards-compatible wrapper for modemSetup()
     *
     * @m_deprecated_since{0,24,1}
     *
     * @return True if setup was successful
     */
    bool setup() {
        return modemSetup();
    }
    /**@}*/


    /* ===================================================================== */
    /* Modem Identification Functions                                        */
    /* ===================================================================== */
    /**
     * @brief Get the modem name
     *
     * @return Modem name string
     */
    String getModemName() {
        return _modemName;
    }

    /**
     * @brief Get the modem device identifier
     *
     * @return Device ID string (name + hardware + firmware + serial number)
     */
    String getModemDevId() {
        String devId = getModemName() + F(" HW v") + _modemHwVersion +
            F(" FW v") + _modemFwVersion + F(" SN:") + _modemSerialNumber;
        MS_DBG(F("Modem Device ID:"), devId);
        return devId;
    }

    /* ===================================================================== */
    /* Virtual Override Implementations for Metadata Getters                */
    /* ===================================================================== */
    /**
     * @brief Get the most recent RSSI measurement
     *
     * Virtual override that calls the static mixin function.
     *
     * @return The RSSI value
     */
    float getModemRSSI() override {
        return loggerModemSensingMixin<loggerModemImpl>::getModemRSSI();
    }

    /**
     * @brief Get the most recent signal strength percent measurement
     *
     * Virtual override that calls the static mixin function.
     *
     * @return The signal strength as a percent
     */
    float getModemSignalPercent() override {
        return loggerModemSensingMixin<
            loggerModemImpl>::getModemSignalPercent();
    }

    /**
     * @brief Get the most recent modem battery charge state
     *
     * Virtual override that calls the static mixin function.
     *
     * @return The battery charge state
     */
    float getModemBatteryChargeState() override {
        return loggerModemSensingMixin<
            loggerModemImpl>::getModemBatteryChargeState();
    }

    /**
     * @brief Get the most recent modem battery charge percent
     *
     * Virtual override that calls the static mixin function.
     *
     * @return The battery charge percent
     */
    float getModemBatteryChargePercent() override {
        return loggerModemSensingMixin<
            loggerModemImpl>::getModemBatteryChargePercent();
    }

    /**
     * @brief Get the most recent modem battery voltage
     *
     * Virtual override that calls the static mixin function.
     *
     * @return The battery voltage in mV
     */
    float getModemBatteryVoltage() override {
        return loggerModemSensingMixin<
            loggerModemImpl>::getModemBatteryVoltage();
    }

    /**
     * @brief Get the most recent modem temperature
     *
     * Virtual override that calls the static mixin function.
     *
     * @return The temperature in degrees Celsius
     */
    float getModemTemperature() override {
        return loggerModemSensingMixin<loggerModemImpl>::getModemTemperature();
    }

    /* ===================================================================== */
    /* Client Management Adapters                                           */
    /* ===================================================================== */
    /**
     * @brief Delete a client - adapter to match base class signature
     *
     * The base class uses Client* while the mixin uses ClientType*.
     * This adapter casts between them.
     *
     * @param client The client pointer to delete
     */
    void deleteClient(Client* client) override {
        if (client != nullptr) {
            loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                                 SecureClientType_T>::
                deleteClient(static_cast<ClientType*>(client));
        }
    }

    /**
     * @brief Delete a secure client - adapter to match base class signature
     *
     * The base class uses Client* while the mixin uses SecureClientType*.
     * This adapter casts between them.
     *
     * @param client The client pointer to delete
     */
    void deleteSecureClient(Client* client) override {
        if (client != nullptr) {
            loggerModemCommMixin<loggerModemImpl, GsmModemType_T, ClientType_T,
                                 SecureClientType_T>::
                deleteSecureClient(static_cast<SecureClientType*>(client));
        }
    }

    /* ===================================================================== */
    /* Internal Members                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_member_variables
     * @name Modem member variables
     */
    /**@{*/

    /**
     * @brief The modem name
     *
     * Set in the init() portion of the #modemSetup().
     * Returned by #getModemName().
     */
    String _modemName = "unspecified modem";

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
     * @brief The modem serial number.
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String _modemSerialNumber;

    /**
     * @brief Maximum time to wait for AT response in milliseconds
     */
    uint32_t _max_at_response_time_ms;

    /**
     * @brief Flag indicating if modem has been setup
     */
    bool _hasBeenSetup;

    /**@}*/

    /**
     * @anchor modem_static_variables
     * @name Static member variables for modem metadata
     */
    /**@{*/

    /**
     * @brief The most recent RSSI reading
     */
    static int16_t _priorRSSI;

    /**
     * @brief The most recent signal percent reading
     */
    static int16_t _priorSignalPercent;

    /**
     * @brief The most recent modem temperature reading
     */
    static float _priorModemTemp;

    /**
     * @brief The most recent battery charge state
     */
    static float _priorBatteryState;

    /**
     * @brief The most recent battery percent reading
     */
    static float _priorBatteryPercent;

    /**
     * @brief The most recent battery voltage reading
     */
    static float _priorBatteryVoltage;

    /**@}*/

 protected:
    /* ===================================================================== */
    /* Pure Virtual Functions                                               */
    /* ===================================================================== */
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
     * @brief Perform modem-specific extra setup
     *
     * @return True if setup succeeded
     */
    virtual bool extraModemSetup() = 0;
};

// Initialize static member variables
template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI>
int16_t loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                        signalQualityIsRSSI>::_priorRSSI = MS_INVALID_VALUE;

template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI>
int16_t loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                        signalQualityIsRSSI>::_priorSignalPercent =
    MS_INVALID_VALUE;

template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI>
float loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                      signalQualityIsRSSI>::_priorBatteryState =
    MS_INVALID_VALUE;

template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI>
float loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                      signalQualityIsRSSI>::_priorBatteryPercent =
    MS_INVALID_VALUE;

template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI>
float loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                      signalQualityIsRSSI>::_priorBatteryVoltage =
    MS_INVALID_VALUE;

template <typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T, bool signalQualityIsRSSI>
float loggerModemImpl<GsmModemType_T, ClientType_T, SecureClientType_T,
                      signalQualityIsRSSI>::_priorModemTemp = MS_INVALID_VALUE;

#endif  // SRC_LOGGERMODEM_H_

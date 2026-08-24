/**
 * @file LoggerModemCommMixin.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief CRTP mixin providing internet connectivity and client creation for
 * modems.
 *
 * This mixin contains all functions for connecting/disconnecting from the
 * internet and creating/deleting TCP and secure clients.
 */

// Header Guards
#ifndef SRC_LOGGERMODEMCOMMMIXIN_H_
#define SRC_LOGGERMODEMCOMMMIXIN_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEMCOMM_DEBUG
#define MS_DEBUGGING_STD "LoggerModemComm"
#endif
#ifdef MS_LOGGERMODEMCOMM_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModemComm"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

#include <Arduino.h>
#include <Client.h>

// Include EpochTime for time conversions
#include <EpochTime.h>
// Include the TinyGSM capabilities file to check what the modem can do
#include <TinyGsmCapabilities.h>
// Include TinyGsmSSL for SSLAuthMode and SSLVersion enums
#include <TinyGsmSSL.tpp>

/**
 * @brief CRTP mixin class providing internet connectivity for modems.
 *
 * This class implements the Curiously Recurring Template Pattern (CRTP) to
 * provide internet connection and client creation functionality.
 *
 * @tparam Derived The derived class type that uses this mixin
 * @tparam GsmModemType_T The TinyGSM modem type
 * @tparam ClientType_T The client type for this modem
 * @tparam SecureClientType_T The secure client type for this modem
 */
template <typename Derived, typename GsmModemType_T, typename ClientType_T,
          typename SecureClientType_T>
class loggerModemCommMixin {
 public:
    // Type aliases to avoid incomplete type issues
    ///@copydoc loggerModemImpl::GsmModemType
    using GsmModemType = GsmModemType_T;
    /// @copydoc loggerModemImpl::ClientType
    using ClientType = ClientType_T;
    /// @copydoc loggerModemImpl::SecureClientType
    using SecureClientType = SecureClientType_T;

 protected:
    /// @copydoc loggerModemPowerMixin::derived()
    inline Derived& derived() {
        return static_cast<Derived&>(*this);
    }
    /// @copydoc loggerModemPowerMixin::derived() const
    inline const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }

    /* ===================================================================== */
    /* Internet Connection                                                  */
    /* ===================================================================== */
    /**
     * @anchor modem_connection_functions_impl
     * @name Implementations of functions for internet connection and
     * disconnection Functions for managing internet connection and determining
     * connectivity status.
     */
    /**@{*/
 protected:
    /// @copydoc loggerModem::connectInternet()
    virtual bool connectInternetImpl(uint32_t maxConnectionTime = 50000L) {
        return connectInternetImpl(
            maxConnectionTime,
            typename TinyGsmCapabilities::has_gprs<GsmModemType>::type());
    }

    /**
     * @brief Common preparation for connecting to the internet
     * @return True if the modem successfully woke and is ready to connect to
     * the internet; false otherwise
     */
    virtual bool prepareForInternet() {
        bool success = true;

        // Power up, if necessary
        bool wasPowered = true;
        if (derived()._millisPowerOn == 0) {
            derived().modemPowerUpImpl();
            wasPowered = false;
        }

        // Check if the modem was awake, wake it if not
        bool wasAwake = derived().isModemAwakeImpl();
        if (!wasAwake) {
            MS_DBG(F("Waiting for modem to boot after power on ..."));
            while (millis() - derived()._millisPowerOn <
                   derived()._wakeDelayTime_ms) {
                yield();
            }
            MS_DBG(F("Waking up the modem to connect to the internet ..."));
            success &= derived().modemWakeImpl();
        } else {
            MS_DBG(F("Modem was already awake and should be ready."));
        }
        if (!wasPowered) {
            MS_DBG(F("Modem was powered to connect to the internet!  "
                     "Remember to turn it off when you're done."));
        } else if (!wasAwake) {
            MS_DBG(F("Modem was woken up to connect to the internet!   "
                     "Remember to put it to sleep when you're done."));
        }
        return success;
    }

    /**
     * @brief The connect to the internet function for modems that have cellular
     * (GPRS, EPS, etc) connectivity.
     *
     * Wait for the modem to successfully register on the cellular
     * network and then request that it establish either EPS or GPRS data
     * connection.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data connection.  Defaults to 50,000ms
     * (50s).
     * @return True if EPS or GPRS data connection has been
     * established.  False if the modem was unresponsive, unable to register
     * with the cellular network, or unable to establish a EPS or GPRS
     * connection.
     */
    bool connectInternetImpl(uint32_t maxConnectionTime,
                             TinyGsmCapabilities::true_type) {
        bool success = derived().prepareForInternet();
        if (!success) return false;

        MS_START_DEBUG_TIMER
        MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,
               F("seconds for cellular network registration..."));
        if (derived().gsmModem.waitForNetwork(maxConnectionTime)) {
            // for all cellular modems **except the XBee** we need to actively
            // connect to the APN using the gprsConnect function after we've
            // connected to the base cellular network.  The XBee stores the APN
            // in NVM and automatically inputs in in the network connection
            // process.  We *avoid* setting the APN unnecessarily on the XBee so
            // as not to wear out the limited write flash.
            if (strcmp(Derived::GsmModemType::ModemConfig::MODEM_MANUFACTURER,
                       "XBee") != 0) {
                MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER,
                       F("milliseconds.  Connecting to GPRS..."));
                if (!derived().connectWithCredentials()) {
                    MS_DBG(F("...GPRS attach failed."));
                    return false;
                }
            }
            MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,
                   F("milliseconds."));
            return true;
        } else {
            MS_DBG(F("...GPRS connection failed."));
            return false;
        }
    }

    /**
     * @brief The connect to the internet function for modems that have WiFi
     * connectivity and do not have cellular connectivity
     *
     * Wait for the modem see if the modem will reestablish a connection on its
     * own with previously saved connections and then send new credentials and
     * wait for a new connection if that fails.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data connection.  Defaults to 50,000ms
     * (50s).
     * @return True if WiFi data connection has been established.  False if the
     * modem was unresponsive or unable to connect to the network.
     */
    bool connectInternetImpl(uint32_t maxConnectionTime,
                             TinyGsmCapabilities::false_type) {
        bool success = derived().prepareForInternet();
        if (!success) return false;

        const uint32_t reconnectTime = derived().autoReconnectTime();
        MS_START_DEBUG_TIMER
        MS_DBG(F("\nWaiting"), reconnectTime,
               F("ms to see if WiFi connects without sending new "
                 "credentials..."));
        if (!(derived().gsmModem.isNetworkConnected())) {
            // If still not connected, send new credentials
            if (!(derived().gsmModem.waitForNetwork(reconnectTime))) {
                MS_DBG(F("Sending credentials..."));
                for (uint8_t i = 0; i < 5; i++) {
                    if (derived().connectWithCredentials()) { break; }
                }
                MS_DBG(F("Waiting up to"), maxConnectionTime / 1000,
                       F("seconds for connection"));
                if (!derived().gsmModem.waitForNetwork(maxConnectionTime)) {
                    MS_DBG(F("... WiFi connection failed"));
                    return false;
                }
            }
        }
        MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds!"));
        return success;
    }

    /**
     * @brief Default connection with credentials - override in derived class.
     *
     * This should be overridden in derived classes to implement modem-specific
     * credential handling (e.g., GPRS APN, WiFi SSID/password).
     *
     * @return True if the connection command was successful, false otherwise.
     * True does not necessarily mean that the connection was established, only
     * that the command to connect was sent successfully.
     */
    virtual bool connectWithCredentials() {
        // Default: return true
        // Derived classes with _apn member should override to call:
        // return gsmModem.gprsConnect(_apn, "", "");
        // Derived classes with _ssid and _password members should override to
        // call: return gsmModem.networkConnect(_ssid, _password);
        return true;
    }

    /**
     * @brief Default auto-reconnect time for WiFi modems.
     *
     * This can be overridden in derived classes for modem-specific timing.
     *
     * @return The time in milliseconds to wait before attempting credential
     * connection
     */
    virtual uint32_t autoReconnectTime() const {
        return 10000L;
    }

    /// @copydoc loggerModem::disconnectInternet()
    virtual void disconnectInternetImpl() {
        MS_START_DEBUG_TIMER;
        disconnectInternetImpl(
            typename TinyGsmCapabilities::has_gprs<GsmModemType>::type());
        MS_DBG(F("Disconnected from internet after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds."));
    }

    /**
     * @brief The disconnect function for modems with cellular connectivity.
     *
     * Detach from EPS or GPRS data connection and then deregister from the
     * cellular network.
     */
    void disconnectInternetImpl(TinyGsmCapabilities::true_type) {
        derived().gsmModem.gprsDisconnect();
    }

    /**
     * @brief The disconnect function for modems without cellular (ie, with
     * WiFi) connectivity.
     */
    void disconnectInternetImpl(TinyGsmCapabilities::false_type) {
        derived().gsmModem.networkDisconnect();
    }

    /// @copydoc loggerModem::isInternetAvailable()
    virtual bool isInternetAvailableImpl() {
        return isInternetAvailableImpl(
            typename TinyGsmCapabilities::has_gprs<GsmModemType>::type());
    }

    /**
     * @brief The internet verification function for modems with cellular
     * connectivity.
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    bool isInternetAvailableImpl(TinyGsmCapabilities::true_type) {
        return derived().gsmModem.isGprsConnected();
    }

    /**
     * @brief The internet verification function for modems without cellular
     * connectivity (ie, WiFi modems)
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    bool isInternetAvailableImpl(TinyGsmCapabilities::false_type) {
        return derived().gsmModem.isNetworkConnected();
    }
    /**@}*/

    /* ===================================================================== */
    /* TCP Clients                                                           */
    /* ===================================================================== */
    /**
     * @anchor modem_client_functions_impl
     * @name Implementations of functions for creating standard TCP clients
     */
    /**@{*/
 protected:
    /// @copydoc loggerModem::createClient()
    Client* createClientImpl(uint8_t mux = 0) {
        // Use the new keyword to create a new client on the **heap**
        return new ClientType(derived().gsmModem, mux);
    }

    /// @copydoc loggerModem::deleteClient(Client*)
    virtual void deleteClientImpl(Client* client) {
        delete static_cast<ClientType*>(client);
    }
    /**@}*/

    /* ===================================================================== */
    /* Secure Clients                                                        */
    /* ===================================================================== */
    /**
     * @anchor modem_ssl_client_functions_impl
     * @name Implementations of functions for creating secured (TLS/SSL) clients
     */
    /**@{*/
 protected:
    /// @copydoc loggerModem::createSecureClient(uint8_t)
    Client* createSecureClientImpl(uint8_t mux = 0) {
        return createSecureClientImpl(
            mux, typename TinyGsmCapabilities::has_ssl<GsmModemType>::type());
    }
    /**
     * @brief The create secure client function for modems that support SSL
     * @copydetails loggerModem::createSecureClient(uint8_t)
     */
    Client* createSecureClientImpl(uint8_t mux,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, mux);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL
     * @return A null pointer
     */
    Client* createSecureClientImpl(uint8_t, TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::createSecureClient(uint8_t, SSLAuthMode,
    /// SSLVersion, const char*, const char*, const char*)
    Client* createSecureClientImpl(uint8_t mux, SSLAuthMode sslAuthMode,
                                   SSLVersion  sslVersion = SSLVersion::TLS1_2,
                                   const char* CAcertName = nullptr,
                                   const char* clientCertName = nullptr,
                                   const char* clientKeyName  = nullptr) {
        return createSecureClientImpl(
            mux, sslAuthMode, sslVersion, CAcertName, clientCertName,
            clientKeyName,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
 protected:
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(uint8_t, SSLAuthMode,
     * SSLVersion, const char*, const char*, const char*)
     */
    Client* createSecureClientImpl(uint8_t mux, SSLAuthMode sslAuthMode,
                                   SSLVersion  sslVersion,
                                   const char* CAcertName,
                                   const char* clientCertName,
                                   const char* clientKeyName,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, mux, sslAuthMode,
                                    sslVersion, CAcertName, clientCertName,
                                    clientKeyName);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL with certificate specification
     * @return A null pointer
     */
    Client* createSecureClientImpl(uint8_t, SSLAuthMode, SSLVersion,
                                   const char*, const char*, const char*,
                                   TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::createSecureClient(SSLAuthMode, SSLVersion,
    /// const char*, const char*, const char*)
    Client* createSecureClientImpl(SSLAuthMode sslAuthMode,
                                   SSLVersion  sslVersion = SSLVersion::TLS1_2,
                                   const char* CAcertName = nullptr,
                                   const char* clientCertName = nullptr,
                                   const char* clientKeyName  = nullptr) {
        return createSecureClientImpl(
            sslAuthMode, sslVersion, CAcertName, clientCertName, clientKeyName,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(SSLAuthMode, SSLVersion,
     * const char*, const char*, const char*)
     */
    Client* createSecureClientImpl(SSLAuthMode sslAuthMode,
                                   SSLVersion  sslVersion,
                                   const char* CAcertName,
                                   const char* clientCertName,
                                   const char* clientKeyName,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, sslAuthMode, sslVersion,
                                    CAcertName, clientCertName, clientKeyName);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL with certificate specification
     * @return A null pointer
     */
    Client* createSecureClientImpl(SSLAuthMode, SSLVersion, const char*,
                                   const char*, const char*,
                                   TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::createSecureClient(uint8_t, const char*,
    /// const char*, SSLVersion)
    Client* createSecureClientImpl(uint8_t mux, const char* pskIdent,
                                   const char* psKey,
                                   SSLVersion sslVersion = SSLVersion::TLS1_2) {
        return createSecureClientImpl(
            mux, pskIdent, psKey, sslVersion,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that support certificates specification
     * @copydetails loggerModem::createSecureClient(uint8_t, const char*,
     * const char*, SSLVersion)
     */
    Client* createSecureClientImpl(uint8_t mux, const char* pskIdent,
                                   const char* psKey, SSLVersion sslVersion,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, mux, pskIdent, psKey,
                                    sslVersion);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL certificates specification
     * @return A null pointer
     */
    Client* createSecureClientImpl(uint8_t, const char*, const char*,
                                   SSLVersion,
                                   TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::createSecureClient(const char*, const char*,
    /// SSLVersion)
    Client* createSecureClientImpl(const char* pskIdent, const char* psKey,
                                   SSLVersion sslVersion = SSLVersion::TLS1_2) {
        return createSecureClientImpl(
            pskIdent, psKey, sslVersion,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that support certificates specification
     * @copydetails loggerModem::createSecureClient(const char*, const char*,
     * SSLVersion)
     */
    Client* createSecureClientImpl(const char* pskIdent, const char* psKey,
                                   SSLVersion sslVersion,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, pskIdent, psKey,
                                    sslVersion);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL certificates specification
     * @return A null pointer
     */
    Client* createSecureClientImpl(const char*, const char*, SSLVersion,
                                   TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::createSecureClient(uint8_t, const char*,
    /// SSLVersion)
    Client* createSecureClientImpl(uint8_t mux, const char* pskTableName,
                                   SSLVersion sslVersion = SSLVersion::TLS1_2) {
        return createSecureClientImpl(
            mux, pskTableName, sslVersion,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(uint8_t, const char*,
     * SSLVersion)
     */
    Client* createSecureClientImpl(uint8_t mux, const char* pskTableName,
                                   SSLVersion sslVersion,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, mux, pskTableName,
                                    sslVersion);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL with certificate specification
     * @return A null pointer
     */
    Client* createSecureClientImpl(uint8_t, const char*, SSLVersion,
                                   TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::createSecureClient(const char*, SSLVersion)
    Client* createSecureClientImpl(const char* pskTableName,
                                   SSLVersion sslVersion = SSLVersion::TLS1_2) {
        return createSecureClientImpl(
            pskTableName, sslVersion,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(const char*, SSLVersion)
     */
    Client* createSecureClientImpl(const char* pskTableName,
                                   SSLVersion  sslVersion,
                                   TinyGsmCapabilities::true_type) {
        return new SecureClientType(derived().gsmModem, pskTableName,
                                    sslVersion);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL with certificate specification
     * @return A null pointer
     */
    Client* createSecureClientImpl(const char*, SSLVersion,
                                   TinyGsmCapabilities::false_type) {
        return nullptr;
    }

    /// @copydoc loggerModem::deleteSecureClient(Client*)
    virtual void deleteSecureClientImpl(Client* client) {
        deleteSecureClientImpl(
            client,
            typename TinyGsmCapabilities::has_ssl<GsmModemType>::type());
    }
    /**
     * @brief The delete secure client function for modems with SSL capabilities
     * @copydetails loggerModem::deleteSecureClient(Client*)
     */
    void deleteSecureClientImpl(Client* client,
                                TinyGsmCapabilities::true_type) {
        // WARNING: This static_cast is safe ONLY if the client was created by
        // createSecureClient(). Mismatched create/delete calls will cause
        // undefined behavior.
        delete static_cast<SecureClientType*>(client);
    }
    /**
     * @brief The delete secure client function for modems that do not support
     * SSL
     */
    void deleteSecureClientImpl(Client*, TinyGsmCapabilities::false_type) {}
    /**@}*/


    /* ===================================================================== */
    /* NIST and Network Time Protocol (NTP) synchronization                  */
    /* ===================================================================== */
    /**
     * @anchor modem_time_sync_functions_impl
     * @name Implementations of functions for NTP and NIST time synchronization
     */
    /**@{*/
 protected:
    /**
     * @brief A helper typedef to combine the two qualities needed to get NTP
     * time directly from the modem rather than from a NIST server.
     *
     * To get the time directly from the modem, the modem must support both NTP
     * __and__ Time functions.
     */
    typedef TinyGsmCapabilities::integral_constant<
        bool,
        TinyGsmCapabilities::has_ntp<GsmModemType>::value &&
            TinyGsmCapabilities::has_time<GsmModemType>::value>
        HasNTPAndTime;

 public:
    /**
     * @brief Synchronize the modem's internal clock via NTP
     *
     * This needs to be called at wake because many modules forget their time
     * when they are powered down.  Without a proper time set, it is generally
     * not possible to make a SSL connection.
     *
     * @return True if the modem successfully requested a time sync from the NTP
     * server; false otherwise.  Note that this does not guarantee that the time
     * was successfully set, only that the request was sent successfully.
     */
    bool syncNTP() {
        return syncNTP(
            typename TinyGsmCapabilities::has_ntp<GsmModemType>::type());
    }

 protected:
    /**
     * @brief NTP sync for modems with NTP support
     * @copydetails syncNTP()
     */
    bool syncNTP(TinyGsmCapabilities::true_type) {
        return derived().gsmModem.NTPServerSync("pool.ntp.org",
                                                derived()._modemUTCOffset);
    }

    /**
     * @brief NTP sync for modems without NTP support
     * @return False, as NTP sync is not supported on this modem
     */
    bool syncNTP(TinyGsmCapabilities::false_type) {
        return false;
    }

 protected:
    /// @copydoc loggerModem::setModemTimeZone(int8_t)
    void setModemTimeZoneImpl(int8_t timeZone) {
        derived()._modemUTCOffset = timeZone;
    }


    /// @copydoc loggerModem::getNISTTime()
    virtual uint32_t getNISTTimeImpl() {
        // Check for and bail if not connected to the internet.
        if (!isInternetAvailableImpl()) {
            MS_DBG(F("No internet connection, cannot get network time."));
            return 0;
        }
        return getNISTTimeImpl(typename HasNTPAndTime::type());
    }
    /**
     * @brief The get NIST time function for modems that have NTP and Time
     * capabilities.
     *
     * This makes use of the modems AT commands to get the time from an NTP
     * server.  The modem must have both NTP and Time capabilities for this to
     * work.
     *
     * @return Unix timestamp (seconds since Jan 1, 1970 UTC)
     */
    uint32_t getNISTTimeImpl(TinyGsmCapabilities::true_type) {
        derived().gsmModem.NTPServerSync("pool.ntp.org", 0);
        derived().gsmModem.waitForTimeSync();

        int seconds = 0, minutes = 0, hours = 0;
        int day = 0, month = 0, year = 0;
        if (!derived().gsmModem.getNetworkTime(&year, &month, &day, &hours,
                                               &minutes, &seconds, 0)) {
            return 0;
        }

        tm timeParts       = {};
        timeParts.tm_sec   = seconds;
        timeParts.tm_min   = minutes;
        timeParts.tm_hour  = hours;
        timeParts.tm_mday  = day;
        timeParts.tm_mon   = month - 1;
        timeParts.tm_year  = year - 1900;
        timeParts.tm_isdst = 0;
        return static_cast<uint32_t>(mktime(&timeParts)) -
            TimeUtils::getCoreTimeZone();
    }

    /**
     * @brief The get NIST time function for modems that do not have both NTP
     * and Time capabilities.
     *
     * This uses the NIST time protocol over TCP (RFC-868) to get the time.  The
     * NIST time protocol requires a TCP connection to port 37 and returns 4
     * bytes representing the time since Jan 1, 1900.
     *
     * @note We must ensure that we do not ping the daylight server more than
     * once every 4 seconds.  NIST clearly specifies here that this is a
     * requirement for all software that accesses its servers:
     * https://tf.nist.gov/tf-cgi/servers.cgi
     *
     * @return Unix timestamp (seconds since Jan 1, 1970 UTC)
     */
    uint32_t getNISTTimeImpl(TinyGsmCapabilities::false_type) {
        // create a client
        Client* nistClient = derived().createClientImpl();
        // The NIST connection frequently opens and closes very quickly; so fast
        // that every module I've tested sometimes misses the response, so we
        // attempt multiple connections to increase the likelihood of
        // successfully receiving the time.
        for (uint8_t i = 0; i < NIST_TIME_PROTOCOL_RETRIES; i++) {
            // we absolutely must wait at least 4 seconds between NIST requests
            while (millis() - _lastNISTrequest < NIST_TIME_PROTOCOL_SPACING)
                yield();

            const char* nistServer = NIST_TIME_PROTOCOL_HOST;
            uint16_t    nistPort   = NIST_TIME_PROTOCOL_PORT;

            if (!nistClient) {
                MS_DBG(F("Failed to create NIST client"));
                continue;
            }

            MS_DBG(F("Connecting to NIST time server"));
            MS_START_DEBUG_TIMER;
            if (!nistClient->connect(nistServer, nistPort)) {
                // If the connection failed, go to the next loop
                MS_DBG(F("Unable to open TCP connection to NIST!"));
                continue;
            }

            _lastNISTrequest = millis();
            // wait up to the timeout for the expected 4 bytes
            while (nistClient->connected() &&
                   nistClient->available() < NIST_TIME_PROTOCOL_BYTES &&
                   millis() - _lastNISTrequest < NIST_TIME_PROTOCOL_TIMEOUT) {
                yield();
            }
            MS_DBG(F("Time from NIST received after"), MS_PRINT_DEBUG_TIMER,
                   F("milliseconds"));

            // NOTE: We don't waste time checking for or closing the
            // connection.  We know NIST closes it immediately after
            // sending the 4 bytes.

            // we must have at least 4 bytes for a valid time
            if (nistClient->available() >= NIST_TIME_PROTOCOL_BYTES) {
                byte nistBytes[NIST_TIME_PROTOCOL_BYTES] = {0};
                nistClient->read(nistBytes, NIST_TIME_PROTOCOL_BYTES);
                uint32_t nistParsed = parseNISTBytes(nistBytes);
                if (nistParsed != 0) {
                    MS_DBG(F("Got non-zero NIST timestamp"));
                    // delete the NIST client so we don't have a memory leak!
                    derived().deleteClientImpl(nistClient);
                    return nistParsed;
                } else {
                    MS_DBG(F("Invalid/Zero NIST timestamp"));
                }
            } else {
                MS_DBG(F("NIST time server did not respond!"));
            }
        }
        // delete the NIST client so we don't have a memory leak!
        derived().deleteClientImpl(nistClient);
        return 0;
    }


    /**
     * @brief Convert the 4 bytes returned by the NIST time protocol to the
     * number of seconds since January 1, 1970 in UTC.
     *
     * NIST supplies a 4 byte response to any TCP connection made on port 37.
     * This is the 32-bit number of seconds since January 1, 1970 00:00:00 UTC.
     * The server closes the TCP connection immediately after sending the data,
     * so there is no need to close it
     *
     * @param nistBytes 4 bytes from NIST
     * @return the number of seconds since January 1, 1970 00:00:00
     * UTC
     */
    static uint32_t parseNISTBytes(byte nistBytes[NIST_TIME_PROTOCOL_BYTES]) {
        uint32_t secFrom1900 = 0;
        for (byte i = 0; i < NIST_TIME_PROTOCOL_BYTES; i++) {
            MS_DBG(F("Response Byte"), i, ':', static_cast<char>(nistBytes[i]),
                   '=', static_cast<uint8_t>(nistBytes[i]), '=',
                   String(nistBytes[i], BIN));
            secFrom1900 = (secFrom1900 << 8) | nistBytes[i];
        }
        MS_DBG(F("Seconds from Jan 1, 1900 returned by NIST (UTC):"),
               secFrom1900, '=', String(secFrom1900, BIN));

        // Convert from NIST epoch (seconds since Jan 1, 1900) to Unix epoch
        // (seconds since Jan 1, 1970)
        uint32_t unixTimeStamp = secFrom1900 - EPOCH_NIST_TO_UNIX;
        MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);
        // Sanity check the result
        if (unixTimeStamp < EARLIEST_SANE_UNIX_TIMESTAMP) {
            return 0;
        } else if (unixTimeStamp > LATEST_SANE_UNIX_TIMESTAMP) {
            return 0;
        } else {
            return unixTimeStamp;
        }
    }

    /**
     * @brief UTC timezone offset in hours
     */
    int8_t _modemUTCOffset = 0;
    /**
     * @brief Last time NIST was queried
     */
    uint32_t _lastNISTrequest = 0;
    /**@}*/
};

#endif  // SRC_LOGGERMODEMCOMMMIXIN_H_

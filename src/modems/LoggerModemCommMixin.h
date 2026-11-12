/**
 * @file LoggerModemCommMixin.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief CRTP mixin providing internet connectivity for modems.
 *
 * This mixin contains functions for connecting/disconnecting from the internet
 * and synchronizing time.
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

#include "NISTTime.h"

#include <Arduino.h>

// Include EpochTime for time conversions
#include <EpochTime.h>
// Include the TinyGSM capabilities file to check what the modem can do
#include <TinyGsmCapabilities.h>
// Include TinyGsmSSL for SSLAuthMode and SSLVersion enums
#include <mixins/TinyGsmSSL.tpp>

/**
 * @brief CRTP mixin class providing internet connectivity for modems.
 *
 * This class implements the Curiously Recurring Template Pattern (CRTP) to
 * provide internet connection and client creation functionality.
 *
 * @tparam Derived The derived class type that uses this mixin
 * @tparam GsmModemType_T The TinyGSM modem type
 */
template <typename Derived, typename GsmModemType_T>
class loggerModemCommMixin {
 public:
    // Type aliases to avoid incomplete type issues
    ///@copydoc loggerModemImpl::GsmModemType
    using GsmModemType = GsmModemType_T;

 protected:
    /// @copydoc loggerModemPowerMixin::derived()
    inline Derived& derived() {
        return static_cast<Derived&>(*this);
    }
    /// @copydoc loggerModemPowerMixin::derived() const
    inline const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }

    /**
     * @brief Protected constructor for initializing communication members.
     */
    loggerModemCommMixin() : _modemUTCOffset(0) {}

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
        bool success = derived().prepareForInternet();
        if (!success) return false;
        success &=
            connectInternetImpl(maxConnectionTime,
                                typename TinyGsmCapabilities::has_cellular_data<
                                    GsmModemType>::type());
        // After connecting to the internet, instruct the module to sync with
        // NTP. For some modules, this causes an immediate sync, so this needs
        // to be done after the internet connection is established. For other
        // modules, this will just set the NTP server and the module will sync
        // on its own schedule. Either way, we don't check the return value here
        // because this always returns false for modules that don't support NTP
        // sync and for modules that do support NTP sync, the return value is
        // not a reliable indicator of whether the sync was successful, only
        // that the module accepted the request.
        derived().syncNTP();
        return success;
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
     * network and then request that it establish either a data connection.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data connection.  Defaults to 50,000ms
     * (50s).
     * @return True if a data connection has been
     * established.  False if the modem was unresponsive, unable to register
     * with the cellular network, or unable to establish a data connection.
     */
    bool connectInternetImpl(uint32_t maxConnectionTime,
                             TinyGsmCapabilities::true_type) {
        MS_START_DEBUG_TIMER
        MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,
               F("seconds for cellular network registration..."));
        if (derived().gsmModem.waitForNetwork(maxConnectionTime)) {
            MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER,
                   F("milliseconds.  Connecting to the cellular network..."));
            if (!derived().connectWithCredentials()) {
                MS_DBG(F("...cellular data attach failed."));
                return false;
            }
            MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,
                   F("milliseconds."));
            return true;
        } else {
            MS_DBG(F("...cellular registration failed."));
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
        return true;
    }

    /**
     * @brief Default connection with credentials - override in derived class.
     *
     * This should be overridden in derived classes to implement modem-specific
     * credential handling (e.g., cellular APN, WiFi SSID/password).
     *
     * @return True if the connection command was successful, false otherwise.
     * True does not necessarily mean that the connection was established, only
     * that the command to connect was sent successfully.
     */
    virtual bool connectWithCredentials() = 0;
    // Default: return true
    // Derived classes with _apn member should override to call:
    // return gsmModem.dataConnect(_apn, "", "");
    // Derived classes with _ssid and _password members should override to
    // call: return gsmModem.networkConnect(_ssid, _password);

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
        disconnectInternetImpl(typename TinyGsmCapabilities::has_cellular_data<
                               GsmModemType>::type());
        MS_DBG(F("Disconnected from internet after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds."));
    }

    /**
     * @brief The disconnect function for modems with cellular connectivity.
     *
     * Detach from data connection and then deregister from the cellular
     * network.
     */
    void disconnectInternetImpl(TinyGsmCapabilities::true_type) {
        derived().gsmModem.dataDisconnect();
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
            typename TinyGsmCapabilities::has_cellular_data<
                GsmModemType>::type());
    }

    /**
     * @brief The internet verification function for modems with cellular
     * connectivity.
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    bool isInternetAvailableImpl(TinyGsmCapabilities::true_type) {
        return derived().gsmModem.isDataConnected();
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
    /* NIST and Network Time Protocol (NTP) synchronization                  */
    /* ===================================================================== */
    /**
     * @anchor modem_time_sync_functions_impl
     * @name Implementations of functions for NTP and NIST time synchronization
     */
    /**@{*/

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
                                                derived()._modemUTCOffset * 4);
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

        int16_t year     = 0;
        int16_t month    = 0;
        int16_t day      = 0;
        int16_t hours    = 0;
        int16_t minutes  = 0;
        int16_t seconds  = 0;
        float   timezone = 0;  // in quarter hours!
        if (!derived().gsmModem.getNetworkTime(&year, &month, &day, &hours,
                                               &minutes, &seconds, &timezone)) {
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

        // Convert the standard calendar fields using the processor's local
        // timezone. The timezone value is not part of this conversion.
        time_t t = mktime(&timeParts);
        // cast into a timestamp_t
        timestamp_t ts = static_cast<timestamp_t>(t);
        //  convert to Unix/UTC
        return TimeUtils::convertOffsetAndEpoch(ts, timezone * 15 * 60,
                                                TimeUtils::getCoreEpochStart(),
                                                0, epochStart::unix_epoch);
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
        using NISTClient = typename GsmModemType::TinyGsmClient;

        // create a new client for the NIST time protocol
        NISTClient nistClient(derived().gsmModem);
        // make sure the client attached to the modem correctly
        if (!nistClient.isAttached()) {
            MS_DBG(F("No modem socket available for NIST client"));
            return 0;
        }
        // get the time
        uint32_t returned_time = NISTTime::query(nistClient);
        // detach the client
        nistClient.detach();
        return returned_time;
    }

    /**
     * @brief UTC timezone offset in hours
     */
    int8_t _modemUTCOffset = 0;
    /**@}*/
};

#endif  // SRC_LOGGERMODEMCOMMMIXIN_H_

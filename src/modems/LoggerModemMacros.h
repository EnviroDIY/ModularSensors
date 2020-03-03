/*
 *LoggerModemMacros.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file contains PRE-ROCESSOR MACROS for use with logger modems
 * NOTE:  These are NOT FUNCTIONS they are pre-processor macros that I'm
 * collecting here to avoid writing functions later.
 */

/* Header Guards */
#ifndef LoggerModemMacros_h
#define LoggerModemMacros_h

// Set up the modem

#define MS_MODEM_EXTRA_SETUP(specificModem)     \
    bool specificModem::extraModemSetup(void) { \
        bool success = gsmModem.init();         \
        gsmClient.init(&gsmModem);              \
        _modemName = gsmModem.getModemName();   \
        return success;                         \
    }

// The function to wake up the modem
#define MS_MODEM_WAKE(specificModem)                                           \
    bool specificModem::modemWake(void) {                                      \
        /* Power up */                                                         \
        if (_millisPowerOn == 0) modemPowerUp();                               \
                                                                               \
        /* Set-up pin modes */                                                 \
        /* Because the modem calls wake BEFORE the first setup, we must set    \
         * the pin modes here */                                               \
        setModemPinModes();                                                    \
                                                                               \
        while (millis() - _millisPowerOn < _wakeDelayTime_ms) {}               \
                                                                               \
        /* Check the status pin and wake bits before running wake function.    \
         * Don't want to accidently pulse an already on modem to off */        \
        /* NOTE:  It's possible that the status pin is on, but the modem is    \
         * actually mid-shutdown.  In that case, we'll mistakenly skip         \
         * re-waking it.  This only applies to modules with a pulse wake (ie,  \
         * non-zero wake time).  For all modules that do pulse on, where       \
         * possible I've selected a pulse time that is sufficient to wake but  \
         * not quite long enough to put it to sleep and am using AT commands   \
         * to sleep.  This *should* keep everything lined up.*/                \
        if (_statusPin >= 0 && digitalRead(_statusPin) == _statusLevel &&      \
            _wakePulse_ms > 0) {                                               \
            MS_DBG(getModemName(), F("was already on!  (status pin"),          \
                   _statusPin, F("level = "),                                  \
                   _statusLevel ? F("HIGH") : F("LOW"),                        \
                   F("Will not run wake function."));                          \
        } else {                                                               \
            /* Run the input wake function */                                  \
            MS_DBG(F("Running wake function for"), getModemName());            \
            if (!modemWakeFxn()) {                                             \
                MS_DBG(F("Wake function for"), getModemName(),                 \
                       F("did not run as expected!"));                         \
            }                                                                  \
        }                                                                      \
                                                                               \
        uint8_t resets  = 0;                                                   \
        bool    success = false;                                               \
        while (!success && resets < 2) {                                       \
            /* Check that the modem is responding to AT commands */            \
            MS_START_DEBUG_TIMER;                                              \
            MS_DBG(F("\nWaiting up to"), _max_atresponse_time_ms, F("ms for"), \
                   getModemName(), F("to respond to AT commands..."));         \
            success = gsmModem.testAT(_max_atresponse_time_ms + 500);          \
            if (success) {                                                     \
                MS_DBG(F("... AT OK after"), MS_PRINT_DEBUG_TIMER,             \
                       F("milliseconds!"));                                    \
            } else {                                                           \
                MS_DBG(F("No response to AT commands!"));                      \
            }                                                                  \
                                                                               \
            /* Re-check the status pin */                                      \
            if ((_statusPin >= 0 && digitalRead(_statusPin) != _statusLevel && \
                 !success) ||                                                  \
                !success) {                                                    \
                MS_DBG(getModemName(), F("doesn't appear to be responsive!")); \
                if (_statusPin >= 0) {                                         \
                    MS_DBG(F("Status pin"), _statusPin, F("on"),               \
                           getModemName(), F("is"),                            \
                           digitalRead(_statusPin) ? F("HIGH") : F("LOW"),     \
                           F("indicating it is off!"));                        \
                }                                                              \
                                                                               \
                MS_DBG(F("Attempting a hard reset on the modem! "),            \
                       resets + 1);                                            \
                if (!modemHardReset()) {                                       \
                    /* Exit if we can't hard reset */                          \
                    break;                                                     \
                } else {                                                       \
                    resets++;                                                  \
                }                                                              \
            }                                                                  \
        }                                                                      \
                                                                               \
        /* Clean any junk out of the modem buffer */                           \
        gsmModem.streamClear();                                                \
                                                                               \
        /* Re-run the modem init, or setup if necessary */                     \
        /* This will turn off echo, which often turns itself back on after a   \
         * reset/power loss */                                                 \
        /* This also checks the SIM card state */                              \
        if (!_hasBeenSetup) {                                                  \
            /* If we run setup, take success value entirely from that*/        \
            success = modemSetup();                                            \
        } else {                                                               \
            success &= gsmModem.init();                                        \
        }                                                                      \
        gsmClient.init(&gsmModem);                                             \
                                                                               \
        if (success) {                                                         \
            modemLEDOn();                                                      \
            MS_DBG(getModemName(), F("should be awake and ready to go."));     \
        } else {                                                               \
            MS_DBG(getModemName(), F("failed to wake!"));                      \
        }                                                                      \
                                                                               \
        return success;                                                        \
    }

#if defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_IS_INTERNET_AVAILABLE(specificModem) \
    bool specificModem::isInternetAvailable(void) {   \
        return gsmModem.isGprsConnected();            \
    }

#ifndef TINY_GSM_MODEM_XBEE
#define MS_MODEM_SET_APN                                    \
    MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER, \
           F("milliseconds.  Connecting to GPRS..."));      \
    gsmModem.gprsConnect(_apn, "", "");
#else  // #ifndef TINY_GSM_MODEM_XBEE
#define MS_MODEM_SET_APN
#endif  // #ifndef TINY_GSM_MODEM_XBEE

#define MS_MODEM_CONNECT_INTERNET(specificModem)                      \
    bool specificModem::connectInternet(uint32_t maxConnectionTime) { \
        MS_START_DEBUG_TIMER                                          \
        MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,        \
               F("seconds for cellular network registration..."));    \
        if (gsmModem.waitForNetwork(maxConnectionTime)) {             \
            MS_MODEM_SET_APN                                          \
            MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,    \
                   F("milliseconds."));                               \
            return true;                                              \
        } else {                                                      \
            MS_DBG(F("...GPRS connection failed."));                  \
            return false;                                             \
        }                                                             \
    }

#define MS_MODEM_DISCONNECT_INTERNET(specificModem)           \
    void specificModem::disconnectInternet(void) {            \
        MS_START_DEBUG_TIMER;                                 \
        gsmModem.gprsDisconnect();                            \
        MS_DBG(F("Disconnected from cellular network after"), \
               MS_PRINT_DEBUG_TIMER, F("milliseconds."));     \
    }

#else  // from #if defined TINY_GSM_MODEM_HAS_GPRS (ie, this is wifi)
#define MS_MODEM_IS_INTERNET_AVAILABLE(specificModem) \
    bool specificModem::isInternetAvailable(void) {   \
        return gsmModem.isNetworkConnected();         \
    }

#define MS_MODEM_CONNECT_INTERNET(specificModem)                      \
    bool specificModem::connectInternet(uint32_t maxConnectionTime) { \
        MS_START_DEBUG_TIMER                                          \
        MS_DBG(F("\nAttempting to connect to WiFi network..."));      \
        if (!(gsmModem.isNetworkConnected())) {                       \
            MS_DBG(F("Sending credentials..."));                      \
            while (!gsmModem.networkConnect(_ssid, _pwd)) {}          \
            MS_DBG(F("Waiting up to"), maxConnectionTime / 1000,      \
                   F("seconds for connection"));                      \
            if (!gsmModem.waitForNetwork(maxConnectionTime)) {        \
                MS_DBG(F("... WiFi connection failed"));              \
                return false;                                         \
            }                                                         \
        }                                                             \
        MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER,   \
               F("milliseconds!"));                                   \
        return true;                                                  \
    }

#define MS_MODEM_DISCONNECT_INTERNET(specificModem)       \
    void specificModem::disconnectInternet(void) {        \
        MS_START_DEBUG_TIMER;                             \
        gsmModem.networkDisconnect();                     \
        MS_DBG(F("Disconnected from WiFi network after"), \
               MS_PRINT_DEBUG_TIMER, F("milliseconds.")); \
    }
#endif  // #if defined TINY_GSM_MODEM_HAS_GPRS

// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
#define MS_MODEM_GET_NIST_TIME(specificModem)                                  \
    uint32_t specificModem::getNISTTime(void) {                                \
        /* bail if not connected to the internet */                            \
        if (!isInternetAvailable()) {                                          \
            MS_DBG(F("No internet connection, cannot connect to NIST."));      \
            return 0;                                                          \
        }                                                                      \
                                                                               \
        /* Try up to 12 times to get a timestamp from NIST */                  \
        for (uint8_t i = 0; i < 12; i++) {                                     \
            /* Must ensure that we do not ping the daylight server more than   \
             * once every 4 seconds.  NIST clearly specifies here that this is \
             * a requirement for all software that accesses its servers:       \
             * https://tf.nist.gov/tf-cgi/servers.cgi */                       \
            while (millis() < _lastNISTrequest + 4000) {}                      \
                                                                               \
            /* Make TCP connection */                                          \
            MS_DBG(F("\nConnecting to NIST daytime Server"));                  \
            bool connectionMade = gsmClient.connect("time.nist.gov", 37, 15);  \
                                                                               \
            /* Wait up to 5 seconds for a response */                          \
            if (connectionMade) {                                              \
                uint32_t start = millis();                                     \
                while (gsmClient && gsmClient.available() < 4 &&               \
                       millis() - start < 5000L) {}                            \
                                                                               \
                if (gsmClient.available() >= 4) {                              \
                    MS_DBG(F("NIST responded after"), millis() - start,        \
                           F("ms"));                                           \
                    byte response[4] = {0};                                    \
                    gsmClient.read(response, 4);                               \
                    if (gsmClient.connected()) gsmClient.stop();               \
                    return parseNISTBytes(response);                           \
                } else {                                                       \
                    MS_DBG(F("NIST Time server did not respond!"));            \
                    if (gsmClient.connected()) gsmClient.stop();               \
                }                                                              \
            } else {                                                           \
                MS_DBG(F("Unable to open TCP to NIST!"));                      \
            }                                                                  \
        }                                                                      \
        return 0;                                                              \
    }

#if defined TINY_GSM_MODEM_XBEE || defined TINY_GSM_MODEM_ESP8266
#define MS_MODEM_CALC_SIGNAL_QUALITY                            \
    rssi = signalQual;                                          \
    MS_DBG(F("Raw signal is already in units of RSSI:"), rssi); \
    percent = getPctFromRSSI(signalQual);                       \
    MS_DBG(F("Signal percent calcuated from RSSI:"), percent);
#else
#define MS_MODEM_CALC_SIGNAL_QUALITY             \
    rssi = getRSSIFromCSQ(signalQual);           \
    MS_DBG(F("RSSI Estimated from CSQ:"), rssi); \
    percent = getPctFromCSQ(signalQual);         \
    MS_DBG(F("Signal percent calcuated from CSQ:"), percent);
#endif

#define MS_MODEM_GET_MODEM_SIGNAL_QUALITY(specificModem)                      \
    bool specificModem::getModemSignalQuality(int16_t& rssi,                  \
                                              int16_t& percent) {             \
        /* Get signal quality */                                              \
        /* NOTE:  We can't actually distinguish between a bad modem response, \
         * no modem response, and a real response from the modem of no        \
         * service/signal.  The TinyGSM getSignalQuality function returns the \
         * same "no signal" value (99 CSQ or 0 RSSI) in all 3 cases. */       \
        MS_DBG(F("Getting signal quality:"));                                 \
        int16_t signalQual = gsmModem.getSignalQuality();                     \
        MS_DBG(F("Raw signal quality:"), signalQual);                         \
                                                                              \
        /* Convert signal quality to RSSI, if necessary */                    \
        MS_MODEM_CALC_SIGNAL_QUALITY                                          \
                                                                              \
        return true;                                                          \
    }

#ifdef TINY_GSM_MODEM_HAS_BATTERY
#define MS_MODEM_GET_MODEM_BATTERY_DATA(specificModem)                  \
    bool specificModem::getModemBatteryStats(                           \
        uint8_t& chargeState, int8_t& percent, uint16_t& milliVolts) {  \
        MS_DBG(F("Getting modem battery data:"));                       \
        return gsmModem.getBattStats(chargeState, percent, milliVolts); \
    }

#else
#define MS_MODEM_GET_MODEM_BATTERY_DATA(specificModem)                 \
    bool specificModem::getModemBatteryStats(                          \
        uint8_t& chargeState, int8_t& percent, uint16_t& milliVolts) { \
        MS_DBG(F("This modem doesn't return battery information!"));   \
        chargeState = 99;                                              \
        percent     = -99;                                             \
        milliVolts  = 9999;                                            \
        return false;                                                  \
    }
#endif

#ifdef TINY_GSM_MODEM_HAS_TEMPERATURE
#define MS_MODEM_GET_MODEM_TEMPERATURE_DATA(specificModem) \
    float specificModem::getModemChipTemperature(void) {   \
        MS_DBG(F("Getting temperature:"));                 \
        float temp = gsmModem.getTemperature();            \
        MS_DBG(F("Temperature:"), temp);                   \
                                                           \
        return temp;                                       \
    }

#else
#define MS_MODEM_GET_MODEM_TEMPERATURE_DATA(specificModem)   \
    float specificModem::getModemChipTemperature(void) {     \
        MS_DBG(F("This modem doesn't return temperature!")); \
        return (float)-9999;                                 \
    }
#endif

#endif

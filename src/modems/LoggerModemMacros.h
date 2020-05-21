/**
 * @file LoggerModemMacros.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains PRE-ROCESSOR MACROS for use with logger modems.
 *
 * @note These are NOT FUNCTIONS; they are pre-processor macros that I am
 * collecting here to avoid writing the same functions multiple times later.
 */

/* Header Guards */
#ifndef SRC_MODEMS_LOGGERMODEMMACROS_H_
#define SRC_MODEMS_LOGGERMODEMMACROS_H_


/**
 *
 * @brief Creates an extraModemSetup function for a specific modem subclass.
 *
 * @param specificModem The modem subclass
 *
 * @return An extraModemSetup function specific to a single modem subclass.
 */
#define MS_MODEM_EXTRA_SETUP(specificModem)     \
    bool specificModem::extraModemSetup(void) { \
        bool success = gsmModem.init();         \
        gsmClient.init(&gsmModem);              \
        _modemName = gsmModem.getModemName();   \
        return success;                         \
    }


/**
 *
 * @brief Creates an isModemAwake function for a specific modem subclass.
 *
 * @param specificModem The modem subclass
 *
 * @return An isModemAwake function specific to a single modem subclass.
 */
#define MS_IS_MODEM_AWAKE(specificModem)                                       \
    bool specificModem::isModemAwake(void) {                                   \
        if (_wakePulse_ms > 0 && _statusPin >= 0) {                            \
            /* If there's a pulse wake up (ie, non-zero wake time) and there's \
             * a status pin, use that to determine if the modem was awake      \
             * before setup began.*/                                           \
            bool levelNow = digitalRead(_statusPin);                           \
            MS_DBG(getModemName(), F("status pin"), _statusPin, F("level = "), \
                   levelNow ? F("HIGH") : F("LOW"), F("meaning"),              \
                   getModemName(), F("should be"),                             \
                   levelNow == static_cast<int>(_statusLevel) ? F("on")        \
                                                              : F("off"));     \
            return levelNow == static_cast<int>(_statusLevel);                 \
        } else if (_wakePulse_ms == 0) {                                       \
            /* If the wake up is one where a pin is held (0 wake time) then    \
             * we're going to check the level of the held pin as the           \
             * indication of whether attempts were made to wake the modem      \
             * before entering the setup function*/                            \
            int8_t sleepRqBitNumber =                                          \
                log(digitalPinToBitMask(_modemSleepRqPin)) / log(2);           \
            int8_t currentRqPinState = bitRead(                                \
                *portInputRegister(digitalPinToPort(_modemSleepRqPin)),        \
                sleepRqBitNumber);                                             \
            MS_DBG(F("Current state of sleep request pin"), _modemSleepRqPin,  \
                   '=', currentRqPinState ? F("HIGH") : F("LOW"),              \
                   F("meaning"), getModemName(), F("should be"),               \
                   currentRqPinState == static_cast<int8_t>(_wakeLevel)        \
                       ? F("on")                                               \
                       : F("off"));                                            \
            return (currentRqPinState == static_cast<int8_t>(_wakeLevel));     \
        } else if (_statusPin < 0) {                                           \
            /* If there's no status pin, but still a pulsed wake up, try       \
             * checking if the modem responds to AT commands*/                 \
            int8_t i   = 5;                                                    \
            bool   res = false;                                                \
            while (i && !res) {                                                \
                gsmModem.sendAT(GF(""));                                       \
                res = gsmModem.waitResponse(100) == 1;                         \
                if (res) break;                                                \
                delay(50);                                                     \
                i--;                                                           \
            }                                                                  \
            MS_DBG(F("Tested AT command and got"),                             \
                   res ? F("OK") : F("no response"), F("meaning"),             \
                   getModemName(),                                             \
                   res ? F("must be awake") : F("is probably asleep"));        \
            return res;                                                        \
        } else { /*shouldn't get here*/                                        \
            return true;                                                       \
        }                                                                      \
    }


/**
 *
 * @brief Creates a modemWake function for a specific modem subclass.
 *
 * @param specificModem The modem subclass
 *
 * @return A modemWake function specific to a single modem subclass.
 */
#define MS_MODEM_WAKE(specificModem)                                           \
    bool specificModem::modemWake(void) {                                      \
        /* Power up */                                                         \
        if (_millisPowerOn == 0) { modemPowerUp(); }                           \
                                                                               \
        /* Set-up pin modes */                                                 \
        /* Because the modem calls wake BEFORE the first setup, we must set    \
         * the pin modes here */                                               \
        setModemPinModes();                                                    \
                                                                               \
        MS_DBG(F("Wait"), _wakeDelayTime_ms, F("for warm-up"));                \
        while (millis() - _millisPowerOn < _wakeDelayTime_ms) {}               \
                                                                               \
        if (isModemAwake()) {                                                  \
            MS_DBG(getModemName(),                                             \
                   F("was already on! Will not run wake function."));          \
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
        /* This will turn off echo, which often turns itself back on after     \
         * a reset/power loss */                                               \
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
/**
 *
 * @brief Creates an isInternetAvailable function for a specific *cellular*
 * modem subclass.
 *
 * @param specificModem The modem subclass
 *
 * @return An isInternetAvailable function specific to a single *cellular* modem
 * subclass.
 */
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
// NOTE:  We eust ensure that we do not ping the daylight server more
// than once every 4 seconds.  NIST clearly specifies here that this is
// a requirement for all software that accesses its servers:
// https://tf.nist.gov/tf-cgi/servers.cgi */
#define MS_MODEM_GET_NIST_TIME(specificModem)                                 \
    uint32_t specificModem::getNISTTime(void) {                               \
        /* bail if not connected to the internet */                           \
        if (!isInternetAvailable()) {                                         \
            MS_DBG(F("No internet connection, cannot connect to NIST."));     \
            return 0;                                                         \
        }                                                                     \
                                                                              \
        /* Try up to 12 times to get a timestamp from NIST */                 \
        for (uint8_t i = 0; i < 12; i++) {                                    \
            while (millis() < _lastNISTrequest + 4000) {}                     \
                                                                              \
            /* Make TCP connection */                                         \
            MS_DBG(F("\nConnecting to NIST daytime Server"));                 \
            bool connectionMade = gsmClient.connect("time.nist.gov", 37, 15); \
                                                                              \
            /* Wait up to 5 seconds for a response */                         \
            if (connectionMade) {                                             \
                uint32_t start = millis();                                    \
                while (gsmClient && gsmClient.available() < 4 &&              \
                       millis() - start < 5000L) {}                           \
                                                                              \
                if (gsmClient.available() >= 4) {                             \
                    MS_DBG(F("NIST responded after"), millis() - start,       \
                           F("ms"));                                          \
                    byte response[4] = {0};                                   \
                    gsmClient.read(response, 4);                              \
                    if (gsmClient.connected()) gsmClient.stop();              \
                    return parseNISTBytes(response);                          \
                } else {                                                      \
                    MS_DBG(F("NIST Time server did not respond!"));           \
                    if (gsmClient.connected()) gsmClient.stop();              \
                }                                                             \
            } else {                                                          \
                MS_DBG(F("Unable to open TCP to NIST!"));                     \
            }                                                                 \
        }                                                                     \
        return 0;                                                             \
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

/* NOTE:  We can't actually distinguish between a bad modem response,
 * no modem response, and a real response from the modem of no
 * service/signal.  The TinyGSM getSignalQuality function returns the
 * same "no signal" value (99 CSQ or 0 RSSI) in all 3 cases. */
#define MS_MODEM_GET_MODEM_SIGNAL_QUALITY(specificModem)          \
    bool specificModem::getModemSignalQuality(int16_t& rssi,      \
                                              int16_t& percent) { \
        /* Get signal quality */                                  \
        MS_DBG(F("Getting signal quality:"));                     \
        int16_t signalQual = gsmModem.getSignalQuality();         \
        MS_DBG(F("Raw signal quality:"), signalQual);             \
                                                                  \
        /* Convert signal quality to RSSI, if necessary */        \
        MS_MODEM_CALC_SIGNAL_QUALITY                              \
                                                                  \
        return true;                                              \
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
        return static_cast<float>(-9999);                    \
    }
#endif

#endif  // SRC_MODEMS_LOGGERMODEMMACROS_H_

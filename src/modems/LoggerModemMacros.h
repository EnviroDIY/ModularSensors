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

// Header Guards
#ifndef LoggerModemMacros_h
#define LoggerModemMacros_h

#define MS_MODEM_HARD_RESET(specificModem)                             \
    bool specificModem::modemHardReset(void)                           \
    {                                                                  \
        if (_modemResetPin >= 0)                                       \
        {                                                              \
            MS_DBG(F("Doing a hard reset on the modem!"));             \
            digitalWrite(_modemResetPin, LOW);                         \
            delay(200);                                                \
            digitalWrite(_modemResetPin, HIGH);                        \
            return gsmModem.init();                                    \
        }                                                              \
        else                                                           \
        {                                                              \
            MS_DBG(F("No pin has been provided to reset the modem!")); \
            return false;                                              \
        }                                                              \
    }

#if defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_IS_INTERNET_AVAILABLE(specificModem) \
    bool specificModem::isInternetAvailable(void)     \
    {                                                 \
        return gsmModem.isGprsConnected();            \
    }
#else
#define MS_MODEM_IS_INTERNET_AVAILABLE(specificModem) \
    bool specificModem::isInternetAvailable(void)     \
    {                                                 \
        return gsmModem.isNetworkConnected();         \
    }
#endif

#if defined TINY_GSM_MODEM_XBEE || defined TINY_GSM_MODEM_ESP8266
#define MS_MODEM_CALC_SIGNAL_RSSI \
    rssi = signalQual;            \
    MS_DBG(F("Raw signal is already in units of RSSI:"), rssi);
#define MS_MODEM_CALC_SIGNAL_PERCENT      \
    percent = getPctFromRSSI(signalQual); \
    MS_DBG(F("Signal percent calcuated from RSSI:"), percent);
#else
#define MS_MODEM_CALC_SIGNAL_RSSI      \
    rssi = getRSSIFromCSQ(signalQual); \
    MS_DBG(F("RSSI Estimated from CSQ:"), rssi);
#define MS_MODEM_CALC_SIGNAL_PERCENT     \
    percent = getPctFromCSQ(signalQual); \
    MS_DBG(F("Signal percent calcuated from CSQ:"), percent);
#endif

#define MS_MODEM_GET_MODEM_SIGNAL_QUALITY(specificModem)                               \
    bool specificModem::getModemSignalQuality(int16_t &rssi, int16_t &percent)         \
    {                                                                                  \
        /* Get signal quality */                                                       \
        /* NOTE:  We can't actually distinguish between a bad modem response, no */    \
        /* modem response, and a real response from the modem of no service/signal. */ \
        /* The TinyGSM getSignalQuality function returns the same "no signal" */       \
        /* value (99 CSQ or 0 RSSI) in all 3 cases. */                                 \
        MS_DBG(F("Getting signal quality:"));                                          \
        int16_t signalQual = gsmModem.getSignalQuality();                              \
        MS_DBG(F("Raw signal quality:"), signalQual);                                  \
                                                                                       \
        /* Convert signal quality to RSSI, if necessary */                             \
        MS_MODEM_CALC_SIGNAL_RSSI;                                                     \
        MS_MODEM_CALC_SIGNAL_PERCENT;                                                  \
                                                                                       \
        return true;                                                                   \
    }

#define MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(specificModem)                                               \
    bool specificModem::getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) \
    {                                                                                                     \
        MS_DBG(F("Getting modem battery data:"));                                                         \
        return gsmModem.getBattStats(chargeState, percent, milliVolts);                                   \
    }

#define MS_MODEM_GET_MODEM_BATTERY_NA(specificModem)                                                      \
    bool specificModem::getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) \
    {                                                                                                     \
        MS_DBG(F("This modem doesn't return battery information!"));                                      \
        chargeState = 0;                                                                                  \
        percent = 0;                                                                                      \
        milliVolts = 0;                                                                                   \
        return false;                                                                                     \
    }

// NOTE:  Most modems don't give this
#define MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(specificModem) \
    float specificModem::getModemChipTemperature(void)              \
    {                                                           \
        MS_DBG(F("Getting temperature:"));                      \
        float temp = gsmModem.getTemperature();                 \
        MS_DBG(F("Temperature:"), temp);                        \
                                                                \
        return temp;                                            \
    }

#define MS_MODEM_GET_MODEM_TEMPERATURE_NA(specificModem)     \
    float specificModem::getModemChipTemperature(void)           \
    {                                                        \
        MS_DBG(F("This modem doesn't return temperature!")); \
        return (float)-9999;                                 \
    }

#define MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK                                      \
    bool retVal = true;                                                            \
                                                                                   \
    modemPowerUp();                                                                \
    while (millis() - _millisPowerOn < _wakeDelayTime_ms) {}                       \
    retVal &= modemWake();                                                         \
    if (!retVal)                                                                   \
    {                                                                              \
        MS_DBG(F("Modem did't wake up! Cannot connect to the internet!"));         \
        return retVal;                                                             \
    }                                                                              \
                                                                                   \
    /* Check that the modem is responding to AT commands.  If not, give up. */     \
    /* TODO:  Check status pin? */                                                 \
    MS_START_DEBUG_TIMER;                                                          \
    MS_DBG(F("\nWaiting for"), getModemName(), F("to respond to AT commands...")); \
    if (!gsmModem.testAT(_max_atresponse_time_ms + 500))                           \
    {                                                                              \
        MS_DBG(F("No response to AT commands!"));                                  \
        MS_DBG(F("Attempting a hard reset on the modem!"));                        \
        if (!modemHardReset())                                                     \
            return false;                                                          \
    }                                                                              \
    else                                                                           \
    {                                                                              \
        MS_DBG(F("... AT OK after"), MS_PRINT_DEBUG_TIMER, F("milliseconds!"));    \
    }

#if defined TINY_GSM_MODEM_XBEE
#define MS_MODEM_CONNECT_INTERNET(specificModem)                    \
    bool specificModem::connectInternet(uint32_t maxConnectionTime) \
    {                                                               \
                                                                    \
        MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK                       \
                                                                    \
        MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,      \
               F("seconds for internet availability..."));          \
        if (gsmModem.waitForNetwork(maxConnectionTime))             \
        {                                                           \
            MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,  \
                   F("milliseconds."));                             \
            return true;                                            \
        }                                                           \
        else                                                        \
        {                                                           \
            MS_DBG(F("... connection failed."));                    \
            return false;                                           \
        }                                                           \
    }

#elif defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_CONNECT_INTERNET(specificModem)                    \
    bool specificModem::connectInternet(uint32_t maxConnectionTime) \
    {                                                               \
                                                                    \
        MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK                       \
                                                                    \
        MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,      \
               F("seconds for cellular network registration..."));  \
        if (gsmModem.waitForNetwork(maxConnectionTime))             \
        {                                                           \
            MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER, \
                   F("milliseconds.  Connecting to GPRS..."));      \
            gsmModem.gprsConnect(_apn, "", "");                     \
            MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,  \
                   F("milliseconds."));                             \
            return true;                                            \
        }                                                           \
        else                                                        \
        {                                                           \
            MS_DBG(F("...GPRS connection failed."));                \
            return false;                                           \
        }                                                           \
    }

#else
#define MS_MODEM_CONNECT_INTERNET(specificModem)                    \
    bool specificModem::connectInternet(uint32_t maxConnectionTime) \
    {                                                               \
                                                                    \
        MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK                       \
                                                                    \
        MS_DBG(F("\nAttempting to connect to WiFi network..."));    \
        if (!(gsmModem.isNetworkConnected()))                       \
        {                                                           \
            MS_DBG(F("Sending credentials..."));                    \
            while (!gsmModem.networkConnect(_ssid, _pwd))           \
            {                                                       \
            };                                                      \
            MS_DBG(F("Waiting up to"), maxConnectionTime / 1000,    \
                   F("seconds for connection"));                    \
            if (!gsmModem.waitForNetwork(maxConnectionTime))        \
            {                                                       \
                MS_DBG(F("... WiFi connection failed"));            \
                return false;                                       \
            }                                                       \
        }                                                           \
        MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER, \
               F("milliseconds!"));                                 \
        return true;                                                \
    }
#endif

#if defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_DISCONNECT_INTERNET(specificModem)                                 \
    void specificModem::disconnectInternet(void)                                    \
    {                                                                               \
        MS_START_DEBUG_TIMER;                                                       \
        gsmModem.gprsDisconnect();                                                  \
        MS_DBG(F("Disconnected from cellular network after"), MS_PRINT_DEBUG_TIMER, \
               F("milliseconds."));                                                 \
    }
#else
#define MS_MODEM_DISCONNECT_INTERNET(specificModem)                             \
    void specificModem::disconnectInternet(void)                                \
    {                                                                           \
        MS_START_DEBUG_TIMER;                                                   \
        gsmModem.networkDisconnect();                                           \
        MS_DBG(F("Disconnected from WiFi network after"), MS_PRINT_DEBUG_TIMER, \
               F("milliseconds."));                                             \
    }
#endif

// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
#define MS_MODEM_GET_NIST_TIME(specificModem)                                                        \
    uint32_t specificModem::getNISTTime(void)                                                        \
    {                                                                                                \
        /* bail if not connected to the internet */                                                  \
        if (!isInternetAvailable())                                                                  \
        {                                                                                            \
            MS_DBG(F("No internet connection, cannot connect to NIST."));                            \
            return 0;                                                                                \
        }                                                                                            \
                                                                                                     \
        /* Try up to 12 times to get a timestamp from NIST */                                        \
        for (uint8_t i = 0; i < 12; i++)                                                             \
        {                                                                                            \
            /* Must ensure that we do not ping the daylight server more than once every 4 seconds */ \
            /* NIST clearly specifies here that this is a requirement for all software */            \
            /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */                 \
            while (millis() < _lastNISTrequest + 4000)                                               \
            {                                                                                        \
            }                                                                                        \
                                                                                                     \
            /* Make TCP connection */                                                                \
            MS_DBG(F("\nConnecting to NIST daytime Server"));                                        \
            bool connectionMade = gsmClient.connect("time.nist.gov", 37, 15);                        \
                                                                                                     \
            /* Wait up to 5 seconds for a response */                                                \
            if (connectionMade)                                                                      \
            {                                                                                        \
                uint32_t start = millis();                                                           \
                while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L)           \
                {                                                                                    \
                }                                                                                    \
                                                                                                     \
                if (gsmClient.available() >= 4)                                                      \
                {                                                                                    \
                    MS_DBG(F("NIST responded after"), millis() - start, F("ms"));                    \
                    byte response[4] = {0};                                                          \
                    gsmClient.read(response, 4);                                                     \
                    if (gsmClient.connected())                                                       \
                        gsmClient.stop();                                                            \
                    return parseNISTBytes(response);                                                 \
                }                                                                                    \
                else                                                                                 \
                {                                                                                    \
                    MS_DBG(F("NIST Time server did not respond!"));                                  \
                    if (gsmClient.connected())                                                       \
                        gsmClient.stop();                                                            \
                }                                                                                    \
            }                                                                                        \
            else                                                                                     \
            {                                                                                        \
                MS_DBG(F("Unable to open TCP to NIST!"));                                            \
            }                                                                                        \
        }                                                                                            \
        return 0;                                                                                    \
    }

#endif

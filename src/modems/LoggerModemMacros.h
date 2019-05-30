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

#define MS_MODEM_DID_AT_RESPOND(specificModem) \
bool specificModem::didATRespond(void) \
{ \
    return gsmModem.testAT(10); \
}


#if defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_IS_INTERNET_AVAILABLE(specificModem) \
bool specificModem::isInternetAvailable(void) \
{ \
    return gsmModem.isGprsConnected(); \
}
#else
#define MS_MODEM_IS_INTERNET_AVAILABLE(specificModem) \
bool specificModem::isInternetAvailable(void) \
{ \
    return gsmModem.isNetworkConnected(); \
}
#endif


// This checks to see if enough time has passed for measurement completion
// In the case of the modem, we consider a measurement to be "complete" when
// the modem has registered on the network *and* returns good signal strength.
// In theory, both of these things happen at the same time - as soon as the
// module detects a network with sufficient signal strength, it connects and
// will respond corretly to requests for its connection status and the signal
// strength.  In reality sometimes the modem might respond with successful
// network connection before it responds with a valid signal strength or it
// might be able to return a real measurement of cellular signal strength but
// not be able to register to the network.  We'd prefer to wait until it both
// responses are good so we're getting an actual signal strength and it's as
// close as possible to what the antenna is will see when the data publishers
// push data.
#if defined TINY_GSM_MODEM_XBEE || defined TINY_GSM_MODEM_HAS_GPRS
// Cellular modems and wifi modems with the connection paramters always
// saved to flash (like XBees) begin searching for and attempt to register
// to the network as soon as they are awake - the GPRS paramters that need
// to be set to actually *use* the network don't have to be set until we
// make the attempt to use it.
#define MS_MODEM_IMEC_WAIT_LINE elapsed_in_wait = now - _millisSensorActivated;
#else
// For Wifi modems without settings in flash, the connection parameters
// need to set before it can register to the network - that is done in the
// startSingleMeasurement() function and becomes the measurement request time.
#define MS_MODEM_IMEC_WAIT_LINE elapsed_in_wait = now - _millisMeasurementRequested;
#endif


#define MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(specificModem) \
bool specificModem::verifyMeasurementComplete(bool debug) \
{ \
    /* If a measurement failed to start, the sensor will never return a result, */ \
    /* so the measurement time is essentially already passed */ \
    /* For a cellular modem nothing happens to "start" a measurement so bit 6 */ \
    /* will be set by startSingleMeasurement() as long as bit 4 was set by wake(). */ \
    /* For a WiFi modem, startSingleMeasurement actually sets the WiFi connection */ \
    /* parameters. */ \
    if (!bitRead(_sensorStatus, 6)) \
    { \
        if (debug) {MS_DBG(getSensorName(), \
            F("is not measuring and will not return a value!"));} \
        return true; \
    } \
\
    /* just defining this to not call multiple times below */ \
    uint32_t now = millis(); \
\
    /* We don't want to ping any of the modems too fast so they don't get */ \
    /* overwhelmed.  Make sure we've waited a little */ \
    if (now - _lastConnectionCheck < 250) return false; \
\
    /* Check how long we've been waiting for the network connection and/or a */ \
    /* good measurement of signal quality. */ \
    uint32_t elapsed_in_wait; \
    MS_MODEM_IMEC_WAIT_LINE \
\
    /* If we're connected AND receiving valid signal strength, measurement is complete */ \
    /* In theory these happen at the same time, but in reality one or the other */ \
    /* may happen first. */ \
    bool isConnected = gsmModem.isNetworkConnected(); \
    int signalResponse = gsmModem.getSignalQuality(); \
    if (isConnected && signalResponse != 0 && signalResponse != 99) \
    { \
        if (debug) MS_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"), \
               getSensorName(), F("is now registered on the network and reporting valid signal strength!")); \
        _lastConnectionCheck = now; \
        return true; \
    } \
\
    /* If we've exceeded the allowed time to wait for the network, give up */ \
    if (elapsed_in_wait > _measurementTime_ms) \
    { \
        if (debug) MS_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"), \
               getSensorName(), F("has maxed out wait for network registration!  Ending wait.")); \
         /* Leave status bits and times set - can still get a valid value! */ \
        return true; \
    } \
\
    /* If the modem isn't registered yet or doesn't report valid signal, we still need to wait */ \
    _lastConnectionCheck = now; \
    return false; \
}


#if defined TINY_GSM_MODEM_XBEE || defined TINY_GSM_MODEM_ESP8266
#define MS_MODEM_CALC_SIGNAL_RSSI \
rssi = signalQual; \
MS_DBG(F("Raw signal is already in units of RSSI:"), rssi);
#define MS_MODEM_CALC_SIGNAL_PERCENT \
percent = getPctFromRSSI(signalQual); \
MS_DBG(F("Signal percent calcuated from RSSI:"), percent);
#else
#define MS_MODEM_CALC_SIGNAL_RSSI \
rssi = getRSSIFromCSQ(signalQual); \
MS_DBG(F("RSSI Estimated from CSQ:"), rssi);
#define MS_MODEM_CALC_SIGNAL_PERCENT \
percent = getPctFromCSQ(signalQual); \
MS_DBG(F("Signal percent calcuated from CSQ:"), percent);
#endif

#define MS_MODEM_GET_MODEM_SIGNAL_QUALITY(specificModem) \
bool specificModem::getModemSignalQuality(int16_t &rssi, int16_t &percent) \
{ \
    /* Get signal quality */ \
    /* NOTE:  We can't actually distinguish between a bad modem response, no */ \
    /* modem response, and a real response from the modem of no service/signal. */ \
    /* The TinyGSM getSignalQuality function returns the same "no signal" */ \
    /* value (99 CSQ or 0 RSSI) in all 3 cases. */ \
    MS_DBG(F("Getting signal quality:")); \
    int16_t signalQual = gsmModem.getSignalQuality(); \
    MS_DBG(F("Raw signal quality:"), signalQual); \
\
    /* Convert signal quality to RSSI, if necessary */ \
    MS_MODEM_CALC_SIGNAL_RSSI; \
    MS_MODEM_CALC_SIGNAL_PERCENT; \
\
    return true; \
}


#define MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(specificModem) \
bool specificModem::getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) \
{ \
    MS_DBG(F("Getting modem battery data:")); \
    return gsmModem.getBattStats(chargeState, percent, milliVolts); \
}


#define MS_MODEM_GET_MODEM_BATTERY_NA(specificModem) \
bool specificModem::getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) \
{ \
    MS_DBG(F("This modem doesn't return battery information!")); \
    return false; \
}


// NOTE:  Most modems don't give this
#define MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(specificModem) \
float specificModem::getModemTemperature(void) \
{ \
    MS_DBG(F("Getting temperature:")); \
    float temp = gsmModem.getTemperature(); \
    MS_DBG(F("Temperature:"), temp); \
\
    return temp; \
}


#define MS_MODEM_GET_MODEM_TEMPERATURE_NA(specificModem) \
float specificModem::getModemTemperature(void) \
{ \
    MS_DBG(F("This modem doesn't return temperature!")); \
    return (float)-9999; \
}



#define MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK \
    bool retVal = true; \
\
    /* NOT yet powered */ \
    if (bitRead(_sensorStatus, 1) == 0 || bitRead(_sensorStatus, 2) == 0)   \
    { \
        modemPowerUp(); \
    } \
    /* No attempts yet to wake the modem */ \
    if (bitRead(_sensorStatus, 3) == 0) \
    { \
        waitForWarmUp(); \
        retVal &= wake(); \
    } \
    /* Not yet setup */ \
    if (bitRead(_sensorStatus, 0) == 0) \
    { \
        /* Set-up if necessary */ \
        retVal &= setup(); \
    } \
    if (!retVal) \
    { \
        MS_DBG(F("Modem did't wake up! Cannot connect to the internet!")); \
        return retVal; \
    } \
\
    /* Check that the modem is responding to AT commands.  If not, give up. */ \
    MS_START_DEBUG_TIMER; \
    MS_DBG(F("\nWaiting for"), getSensorName(), F("to respond to AT commands...")); \
    if (!gsmModem.testAT(_stabilizationTime_ms + 500)) \
    { \
        MS_DBG(F("No response to AT commands! Cannot connect to the internet!")); \
        return false; \
    } \
    else MS_DBG(F("... AT OK after"), MS_PRINT_DEBUG_TIMER, F("milliseconds!"));


#if defined TINY_GSM_MODEM_XBEE
#define MS_MODEM_CONNECT_INTERNET(specificModem) \
bool specificModem::connectInternet(uint32_t maxConnectionTime) \
{ \
 \
    MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK \
\
    MS_DBG(F("\nWaiting up to"), maxConnectionTime/1000, \
               F("seconds for internet availability...")); \
    if (gsmModem.waitForNetwork(maxConnectionTime)) \
    { \
        MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER, \
                   F("milliseconds.")); \
        return true; \
    } \
    else \
    { \
        MS_DBG(F("... connection failed.")); \
        return false; \
    } \
}

#elif defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_CONNECT_INTERNET(specificModem) \
bool specificModem::connectInternet(uint32_t maxConnectionTime) \
{ \
 \
    MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK \
\
    MS_DBG(F("\nWaiting up to"), maxConnectionTime/1000, \
               F("seconds for cellular network registration...")); \
    if (gsmModem.waitForNetwork(maxConnectionTime)) \
    { \
        MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER, \
                   F("milliseconds.  Connecting to GPRS...")); \
        gsmModem.gprsConnect(_apn, "", ""); \
        MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER, \
                   F("milliseconds.")); \
        return true; \
    } \
    else \
    { \
        MS_DBG(F("...GPRS connection failed.")); \
        return false; \
    } \
}


#else
#define MS_MODEM_CONNECT_INTERNET(specificModem) \
bool specificModem::connectInternet(uint32_t maxConnectionTime) \
{ \
\
    MS_MODEM_CONNECT_INTERNET_FIRST_CHUNK \
    \
    MS_DBG(F("\nAttempting to connect to WiFi network...")); \
    if (!(gsmModem.isNetworkConnected())) \
    { \
        MS_DBG(F("Sending credentials...")); \
        while (!gsmModem.networkConnect(_ssid, _pwd)) {}; \
        MS_DBG(F("Waiting up to"), maxConnectionTime/1000, \
                   F("seconds for connection")); \
        if (!gsmModem.waitForNetwork(maxConnectionTime)) \
        { \
            MS_DBG(F("... WiFi connection failed")); \
            return false; \
        } \
    } \
    MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER, \
               F("milliseconds!")); \
    return true; \
}
#endif


#if defined TINY_GSM_MODEM_HAS_GPRS
#define MS_MODEM_DISCONNECT_INTERNET(specificModem) \
void specificModem::disconnectInternet(void) \
{ \
    MS_START_DEBUG_TIMER; \
    gsmModem.gprsDisconnect(); \
    MS_DBG(F("Disconnected from cellular network after"), MS_PRINT_DEBUG_TIMER, \
               F("milliseconds.")); \
}
#else
#define MS_MODEM_DISCONNECT_INTERNET(specificModem) \
void specificModem::disconnectInternet(void) \
{ \
    MS_START_DEBUG_TIMER; \
    gsmModem.networkDisconnect(); \
    MS_DBG(F("Disconnected from WiFi network after"), MS_PRINT_DEBUG_TIMER, \
               F("milliseconds.")); \
}
#endif


// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
#define MS_MODEM_GET_NIST_TIME(specificModem) \
uint32_t specificModem::getNISTTime(void) \
{ \
    /* bail if not connected to the internet */ \
    if (!isInternetAvailable()) \
    { \
        MS_DBG(F("No internet connection, cannot connect to NIST.")); \
        return 0; \
    } \
\
    /* Must ensure that we do not ping the daylight more than once every 4 seconds */ \
    /* NIST clearly specifies here that this is a requirement for all software */ \
    /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */ \
    while (millis() < _lastNISTrequest + 4000) {} \
\
    /* Make TCP connection */ \
    MS_DBG(F("Connecting to NIST daytime Server")); \
    bool connectionMade = false; \
    if (_modemName.indexOf(F("XBee")) >= 0) \
    { \
        /* This is the IP address of time-c-g.nist.gov */ \
        /* XBee's address lookup falters on time.nist.gov */ \
        IPAddress ip(129, 6, 15, 30); \
        connectionMade = gsmClient.connect(ip, 37); \
        /* Wait again so NIST doesn't refuse us! */ \
        delay(4000L); \
        /* Need to send something before connection is made */ \
        gsmClient.println('!'); \
        /* Need this delay!  Can get away with 50, but 100 is safer. */ \
        /*delay(100);*/ \
    } \
    else connectionMade = gsmClient.connect("time.nist.gov", 37); \
\
    /* Wait up to 5 seconds for a response */ \
    if (connectionMade) \
    { \
        uint32_t start = millis(); \
        while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L){} \
\
        if (gsmClient.available() >= 4) \
        { \
            MS_DBG(F("NIST responded after"), millis() - start, F("ms")); \
            /* Response is returned as 32-bit number as soon as connection is made */ \
            /* Connection is then immediately closed, so there is no need to close it */ \
            uint32_t secFrom1900 = 0; \
            byte response[4] = {0}; \
            for (uint8_t i = 0; i < 4; i++) \
            { \
                response[i] = gsmClient.read(); \
                MS_DBG(F("Response Byte"), i, ':', (char)response[i], \
                           '=', response[i], '=', String(response[i], BIN)); \
                secFrom1900 += 0x000000FF & response[i]; \
                /* MS_DBG(F("\nseconds from 1900 after byte:"),String(secFrom1900, BIN)); */ \
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;} \
            } \
            MS_DBG(F("Seconds from 1900 returned by NIST (UTC):"), \
                       secFrom1900, '=', String(secFrom1900, BIN)); \
\
            /* Close the TCP connection, just in case */ \
            gsmClient.stop(); \
\
            /* Return the timestamp */ \
            uint32_t unixTimeStamp = secFrom1900 - 2208988800; \
            MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp); \
            /* If before Jan 1, 2017 or after Jan 1, 2030, most likely an error */ \
            if (unixTimeStamp < 1483228800) return 0; \
            else if (unixTimeStamp > 1893456000) return 0; \
            else return unixTimeStamp; \
        } \
        else \
        { \
            MS_DBG(F("NIST Time server did not respond!")); \
            return 0; \
        } \
    } \
    else MS_DBG(F("Unable to open TCP to NIST!")); \
    return 0; \
}

#endif

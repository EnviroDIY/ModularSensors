/**
 * @file NISTTime.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the NISTTime helper class.
 */

#include "NISTTime.h"

uint32_t NISTTime::_lastRequest = 0;


void NISTTime::waitForNextRequest() {
    while (millis() - _lastRequest < NIST_TIME_PROTOCOL_SPACING) { yield(); }
    _lastRequest = millis();
}


uint32_t NISTTime::query(Client& nistClient) {
    // The NIST connection frequently opens and closes very quickly; so fast
    // that every module tested sometimes misses the response, so attempt
    // multiple connections to increase the likelihood of receiving the time.
    for (uint8_t i = 0; i < NIST_TIME_PROTOCOL_RETRIES; i++) {
        waitForNextRequest();

        const char* nistServer = NIST_TIME_PROTOCOL_HOST;
        uint16_t    nistPort   = NIST_TIME_PROTOCOL_PORT;

        MS_DBG(F("Connecting to NIST time server"));
        MS_START_DEBUG_TIMER;
        if (!nistClient.connect(nistServer, nistPort)) {
            MS_DBG(F("Unable to open TCP connection to NIST!"));
            nistClient.stop();
            continue;
        }

        uint32_t responseStart = millis();
        // Wait up to the timeout for the expected four bytes.
        while (nistClient.connected() &&
               nistClient.available() < NIST_TIME_PROTOCOL_BYTES &&
               millis() - responseStart < NIST_TIME_PROTOCOL_TIMEOUT) {
            yield();
        }
        MS_DBG(F("Time from NIST received after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds"));

        // The NIST server closes the connection immediately after sending the
        // four-byte response.
        if (nistClient.available() >= NIST_TIME_PROTOCOL_BYTES) {
            byte nistBytes[NIST_TIME_PROTOCOL_BYTES] = {0};
            nistClient.read(nistBytes, NIST_TIME_PROTOCOL_BYTES);
            uint32_t nistParsed = parseResponse(nistBytes);
            nistClient.stop();
            if (nistParsed != 0) {
                MS_DBG(F("Got non-zero NIST timestamp"));
                return nistParsed;
            }
            MS_DBG(F("Invalid/Zero NIST timestamp"));
        } else {
            MS_DBG(F("NIST time server did not respond!"));
            nistClient.stop();
        }
    }
    return 0;
}


uint32_t NISTTime::parseResponse(
    const byte nistBytes[NIST_TIME_PROTOCOL_BYTES]) {
    uint32_t secFrom1900 = 0;
    for (byte i = 0; i < NIST_TIME_PROTOCOL_BYTES; i++) {
        MS_DBG(F("Response Byte"), i, ':', static_cast<char>(nistBytes[i]), '=',
               static_cast<uint8_t>(nistBytes[i]), '=',
               String(nistBytes[i], BIN));
        secFrom1900 = (secFrom1900 << 8) | nistBytes[i];
    }
    MS_DBG(F("Seconds from Jan 1, 1900 returned by NIST (UTC):"), secFrom1900,
           '=', String(secFrom1900, BIN));

    // Convert from NIST epoch (seconds since Jan 1, 1900) to Unix epoch
    // (seconds since Jan 1, 1970).
    uint32_t unixTimeStamp = secFrom1900 - EPOCH_NIST_TO_UNIX;
    MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);

    if (unixTimeStamp < EARLIEST_SANE_UNIX_TIMESTAMP ||
        unixTimeStamp > LATEST_SANE_UNIX_TIMESTAMP) {
        return 0;
    }
    return unixTimeStamp;
}

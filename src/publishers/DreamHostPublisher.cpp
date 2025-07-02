/**
 * @file DreamHostPublisher.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the DreamHostPublisher class.
 */

#include "DreamHostPublisher.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

// Constant portions of the requests
const char* DreamHostPublisher::dreamhostHost  = "swrcsensors.dreamhosters.com";
const int   DreamHostPublisher::dreamhostPort  = 80;
const char* DreamHostPublisher::loggerTag      = "?LoggerID=";
const char* DreamHostPublisher::timestampTagDH = "&Loggertime=";

// Constructors
DreamHostPublisher::DreamHostPublisher() : dataPublisher() {}

DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {}

DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, Client* inClient,
                                       int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}

DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, const char* dhUrl,
                                       int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setDreamHostPortalRX(dhUrl);
}
DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, Client* inClient,
                                       const char* dhUrl, int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setDreamHostPortalRX(dhUrl);
}
// Destructor
DreamHostPublisher::~DreamHostPublisher() {}


// Functions for private SWRC server
void DreamHostPublisher::setDreamHostPortalRX(const char* dhUrl) {
    _DreamHostPortalRX = dhUrl;
}


// A way to set members in the begin to use with a bare constructor
void DreamHostPublisher::begin(Logger& baseLogger, Client* inClient,
                               const char* dhUrl) {
    setDreamHostPortalRX(dhUrl);
    dataPublisher::begin(baseLogger, inClient);
}
void DreamHostPublisher::begin(Logger& baseLogger, const char* dhUrl) {
    setDreamHostPortalRX(dhUrl);
    dataPublisher::begin(baseLogger);
}


// Post the data to dream host.
// int16_t DreamHostPublisher::postDataDreamHost(void)
int16_t DreamHostPublisher::publishData(Client* outClient, bool) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;
    int16_t  responseCode   = 0;

    // Open a TCP/IP connection to DreamHost
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(dreamhostHost, dreamhostPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
        txBufferInit(outClient);

        // copy the initial post header into the tx buffer
        txBufferAppend(getHeader);

        // add in the dreamhost receiver URL
        txBufferAppend(_DreamHostPortalRX);

        // start the URL parameters
        txBufferAppend(loggerTag);
        txBufferAppend(_baseLogger->getLoggerID());

        txBufferAppend(timestampTagDH);
        ltoa((Logger::markedLocalUnixTime - 946684800), tempBuffer,
             10);  // BASE 10
        txBufferAppend(tempBuffer);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            txBufferAppend('&');
            txBufferAppend(_baseLogger->getVarCodeAtI(i).c_str());
            txBufferAppend('=');
            txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
        }

        // add the rest of the HTTP GET headers to the outgoing buffer
        txBufferAppend(HTTPtag);
        txBufferAppend(hostHeader);
        txBufferAppend(dreamhostHost);
        txBufferAppend('\r');
        txBufferAppend('\n');
        txBufferAppend('\r');
        txBufferAppend('\n');

        // Flush the complete request
        txBufferFlush();

        // Wait 10 seconds for a response from the server
        uint32_t start = millis();
        while ((millis() - start) < 10000L && outClient->connected() &&
               outClient->available() < 12) {
            delay(10);
        }

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = outClient->readBytes(tempBuffer, 12);
        // Process the HTTP response code
        // The first 9 characters should be "HTTP/1.1 "
        if (did_respond > 0) {
            char responseCode_char[4];
            memcpy(responseCode_char, tempBuffer + 9, 3);
            // Null terminate the string
            memset(responseCode_char + 3, '\0', 1);
            responseCode = atoi(responseCode_char);
            PRINTOUT(F("\n-- Response Code --"));
            PRINTOUT(responseCode);
        } else {
            responseCode = 504;
            PRINTOUT(F("\n-- NO RESPONSE FROM SERVER --"));
        }

#if defined(MS_OUTPUT) || defined(MS_2ND_OUTPUT)
        // throw the rest of the response into the tx buffer so we can debug it
        txBufferInit(nullptr);
        txBufferAppend(tempBuffer, 12, true);
        while (outClient->available()) {
            char c = outClient->read();
            txBufferAppend(c);
        }
        txBufferFlush();
#endif

        // Close the TCP/IP connection
        MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        outClient->stop();
        MS_DBG(F("Client stopped after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost --"));
    }

    return responseCode;
}

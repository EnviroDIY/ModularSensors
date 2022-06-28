/**
 * @file DreamHostPublisher.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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

DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, uint8_t sendEveryX,
                                       uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {}

DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, Client* inClient,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {}

DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, const char* dhUrl,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    setDreamHostPortalRX(dhUrl);
}
DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, Client* inClient,
                                       const char* dhUrl, uint8_t sendEveryX,
                                       uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    setDreamHostPortalRX(dhUrl);
}
// Destructor
DreamHostPublisher::~DreamHostPublisher() {}


// Functions for private SWRC server
void DreamHostPublisher::setDreamHostPortalRX(const char* dhUrl) {
    _DreamHostPortalRX = dhUrl;
}


// This prints the URL out to an Arduino stream
void DreamHostPublisher::printSensorDataDreamHost(Stream* stream) {
    stream->print(_DreamHostPortalRX);
    stream->print(loggerTag);
    stream->print(_baseLogger->getLoggerID());
    stream->print(timestampTagDH);
    stream->print(String(Logger::markedLocalEpochTime -
                         946684800));  // Correct time from epoch to y2k

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        stream->print('&');
        stream->print(_baseLogger->getVarCodeAtI(i));
        stream->print('=');
        stream->print(_baseLogger->getValueStringAtI(i));
    }
}


// This prints a fully structured GET request for DreamHost to the
// specified stream
void DreamHostPublisher::printDreamHostRequest(Stream* stream) {
    // Start the request
    stream->print(getHeader);

    // Stream the full URL with parameters
    printSensorDataDreamHost(stream);

    // Send the rest of the HTTP header
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(dreamhostHost);
    stream->print(F("\r\n\r\n"));
}


// A way to begin with everything already set
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
int16_t DreamHostPublisher::publishData(Client* outClient) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    // Open a TCP/IP connection to DreamHost
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(dreamhostHost, dreamhostPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));

        // copy the initial post header into the tx buffer
        snprintf(txBuffer, sizeof(txBuffer), "%s", getHeader);

        // add in the dreamhost receiver URL
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", _DreamHostPortalRX);

        // start the URL parameters
        if (bufferFree() < 16) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", loggerTag);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s",
                 _baseLogger->getLoggerID());

        if (bufferFree() < 22) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", timestampTagDH);
        ltoa((Logger::markedLocalEpochTime - 946684800), tempBuffer,
             10);  // BASE 10
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(outClient);

            txBuffer[strlen(txBuffer)] = '&';
            _baseLogger->getVarCodeAtI(i).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            txBuffer[strlen(txBuffer)] = '=';
            _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
        }

        // add the rest of the HTTP GET headers to the outgoing buffer
        if (bufferFree() < 52) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", HTTPtag);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", hostHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", dreamhostHost);
        txBuffer[strlen(txBuffer)] = '\r';
        txBuffer[strlen(txBuffer)] = '\n';
        txBuffer[strlen(txBuffer)] = '\r';
        txBuffer[strlen(txBuffer)] = '\n';

        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(outClient);

        // Wait 10 seconds for a response from the server
        uint32_t start = millis();
        while ((millis() - start) < 10000L && outClient->available() < 12) {
            delay(10);
        }

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = outClient->readBytes(tempBuffer, 12);

        // Close the TCP/IP connection
        MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        outClient->stop();
        MS_DBG(F("Client stopped after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost --"));
    }

    // Process the HTTP response
    int16_t responseCode = 0;
    if (did_respond > 0) {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++) {
            responseCode_char[i] = tempBuffer[i + 9];
        }
        responseCode = atoi(responseCode_char);
    } else {
        responseCode = 504;
    }

    PRINTOUT(F("-- Response Code --"));
    PRINTOUT(responseCode);

    return responseCode;
}

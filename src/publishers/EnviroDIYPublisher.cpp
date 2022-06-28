/**
 * @file EnviroDIYPublisher.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the EnviroDIYPublisher class.
 */

#include "EnviroDIYPublisher.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constant values for post requests
// I want to refer to these more than once while ensuring there is only one copy
// in memory
const char* EnviroDIYPublisher::postEndpoint        = "/api/data-stream/";
const char* EnviroDIYPublisher::enviroDIYHost       = "data.envirodiy.org";
const int   EnviroDIYPublisher::enviroDIYPort       = 80;
const char* EnviroDIYPublisher::tokenHeader         = "\r\nTOKEN: ";
const char* EnviroDIYPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* EnviroDIYPublisher::contentTypeHeader =
    "\r\nContent-Type: application/json\r\n\r\n";

const char* EnviroDIYPublisher::samplingFeatureTag = "{\"sampling_feature\":\"";
const char* EnviroDIYPublisher::timestampTag       = "\",\"timestamp\":\"";


// Constructors
EnviroDIYPublisher::EnviroDIYPublisher() : dataPublisher() {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, uint8_t sendEveryX,
                                       uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger&     baseLogger,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
// Destructor
EnviroDIYPublisher::~EnviroDIYPublisher() {}


void EnviroDIYPublisher::setToken(const char* registrationToken) {
    _registrationToken = registrationToken;
}


// Calculates how long the JSON will be
uint16_t EnviroDIYPublisher::calculateJsonSize() {
    uint16_t jsonLength = 21;  // {"sampling_feature":"
    jsonLength += 36;          // sampling feature UUID
    jsonLength += 15;          // ","timestamp":"
    jsonLength += 25;          // markedISO8601Time
    jsonLength += 2;           //  ",
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        jsonLength += 1;   //  "
        jsonLength += 36;  // variable UUID
        jsonLength += 2;   //  ":
        jsonLength += _baseLogger->getValueStringAtI(i).length();
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            jsonLength += 1;  // ,
        }
    }
    jsonLength += 1;  // }

    return jsonLength;
}

// This prints a properly formatted JSON for EnviroDIY to an Arduino stream
void EnviroDIYPublisher::printSensorDataJSON(Stream* stream) {
    stream->print(samplingFeatureTag);
    stream->print(_baseLogger->getSamplingFeatureUUID());
    stream->print(timestampTag);
    stream->print(Logger::formatDateTime_ISO8601(Logger::markedLocalEpochTime));
    stream->print(F("\","));

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        stream->print('"');
        stream->print(_baseLogger->getVarUUIDAtI(i));
        stream->print(F("\":"));
        stream->print(_baseLogger->getValueStringAtI(i));
        if (i + 1 != _baseLogger->getArrayVarCount()) { stream->print(','); }
    }

    stream->print('}');
}


// This prints a fully structured post request for EnviroDIY to the
// specified stream.
void EnviroDIYPublisher::printEnviroDIYRequest(Stream* stream) {
    // Stream the HTTP headers for the post request
    stream->print(postHeader);
    stream->print(postEndpoint);
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(enviroDIYHost);
    stream->print(tokenHeader);
    stream->print(_registrationToken);
    stream->print(contentLengthHeader);
    stream->print(calculateJsonSize());
    stream->print(contentTypeHeader);

    // Stream the JSON itself
    printSensorDataJSON(stream);
}


// A way to begin with everything already set
void EnviroDIYPublisher::begin(Logger& baseLogger, Client* inClient,
                               const char* registrationToken,
                               const char* samplingFeatureUUID) {
    setToken(registrationToken);
    dataPublisher::begin(baseLogger, inClient);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
void EnviroDIYPublisher::begin(Logger&     baseLogger,
                               const char* registrationToken,
                               const char* samplingFeatureUUID) {
    setToken(registrationToken);
    dataPublisher::begin(baseLogger);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}


// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
// int16_t EnviroDIYPublisher::postDataEnviroDIY(void)
int16_t EnviroDIYPublisher::publishData(Client* outClient) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(enviroDIYHost, enviroDIYPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));

        // copy the initial post header into the tx buffer
        snprintf(txBuffer, sizeof(txBuffer), "%s", postHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", postEndpoint);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        // before adding each line/chunk to the outgoing buffer, we make sure
        // there is space for that line, sending out buffer if not
        if (bufferFree() < 28) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", hostHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", enviroDIYHost);

        if (bufferFree() < 47) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", tokenHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", _registrationToken);

        if (bufferFree() < 26) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s",
                 contentLengthHeader);
        itoa(calculateJsonSize(), tempBuffer, 10);  // BASE 10
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);

        if (bufferFree() < 42) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        if (bufferFree() < 21) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", samplingFeatureTag);

        if (bufferFree() < 36) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s",
                 _baseLogger->getSamplingFeatureUUID());

        if (bufferFree() < 42) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", timestampTag);
        Logger::formatDateTime_ISO8601(Logger::markedLocalEpochTime)
            .toCharArray(tempBuffer, 37);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
        txBuffer[strlen(txBuffer)] = '"';
        txBuffer[strlen(txBuffer)] = ',';

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(outClient);

            txBuffer[strlen(txBuffer)] = '"';
            _baseLogger->getVarUUIDAtI(i).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            txBuffer[strlen(txBuffer)] = '"';
            txBuffer[strlen(txBuffer)] = ':';
            _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            if (i + 1 != _baseLogger->getArrayVarCount()) {
                txBuffer[strlen(txBuffer)] = ',';
            } else {
                txBuffer[strlen(txBuffer)] = '}';
            }
        }

        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(outClient, true);

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
        PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data "
                   "Portal --"));
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

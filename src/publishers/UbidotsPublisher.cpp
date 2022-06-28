/**
 * @file UbidotsPublisher.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Greg Cutrell <gcutrell@limno.com>
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the UbidotsPublisher class.
 */

#include "UbidotsPublisher.h"


// ============================================================================
//  Functions for the Ubidots API.
// ============================================================================

// Constant values for post requests
// I want to refer to these more than once while ensuring there is only one copy
// in memory
const char* UbidotsPublisher::postEndpoint = "/api/v1.6/devices/";
const char* UbidotsPublisher::ubidotsHost  = "industrial.api.ubidots.com";
const int   UbidotsPublisher::ubidotsPort  = 80;
const char* UbidotsPublisher::tokenHeader  = "\r\nX-Auth-Token: ";
//
//
//
const char* UbidotsPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* UbidotsPublisher::contentTypeHeader =
    "\r\nContent-Type: application/json\r\n\r\n";

const char* UbidotsPublisher::payload = "{";


// Constructors
UbidotsPublisher::UbidotsPublisher() : dataPublisher() {}
UbidotsPublisher::UbidotsPublisher(Logger& baseLogger, uint8_t sendEveryX,
                                   uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {}
UbidotsPublisher::UbidotsPublisher(Logger& baseLogger, Client* inClient,
                                   uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {}
UbidotsPublisher::UbidotsPublisher(Logger&     baseLogger,
                                   const char* authentificationToken,
                                   const char* deviceID, uint8_t sendEveryX,
                                   uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    setToken(authentificationToken);
    _baseLogger->setSamplingFeatureUUID(deviceID);
    MS_DBG(F("dataPublisher object created"));
}
UbidotsPublisher::UbidotsPublisher(Logger& baseLogger, Client* inClient,
                                   const char* authentificationToken,
                                   const char* deviceID, uint8_t sendEveryX,
                                   uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    setToken(authentificationToken);
    _baseLogger->setSamplingFeatureUUID(deviceID);
    MS_DBG(F("dataPublisher object created"));
}
// Destructor
UbidotsPublisher::~UbidotsPublisher() {}


void UbidotsPublisher::setToken(const char* authentificationToken) {
    _authentificationToken = authentificationToken;
    MS_DBG(F("Registration token set!"));
}


// Calculates how long the JSON will be
uint16_t UbidotsPublisher::calculateJsonSize() {
    uint16_t jsonLength = 1;  // {
    // jsonLength += 36;          // sampling feature UUID
    // jsonLength += 15;          // ","timestamp":"
    // jsonLength += 25;          // markedISO8601Time
    // jsonLength += 2;           //  ",
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        jsonLength += 1;  //  "
        jsonLength +=
            _baseLogger->getVarUUIDAtI(i).length();  // parameter ID length
        jsonLength += 11;                            //  ":{"value":
        jsonLength += _baseLogger->getValueStringAtI(i).length();
        jsonLength += 13;  // ,"timestamp":
        jsonLength += 13;  // epoch time in milliseconds
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            jsonLength += 2;  // ,
        }
    }
    jsonLength += 2;  // }

    return jsonLength;
}


// This prints a properly formatted JSON for EnviroDIY to an Arduino stream
void UbidotsPublisher::printSensorDataJSON(Stream* stream) {
    stream->print(payload);

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        stream->print('"');
        stream->print(_baseLogger->getVarUUIDAtI(i));
        stream->print(F("\":{'value':"));
        stream->print(_baseLogger->getValueStringAtI(i));
        stream->print(",'timestamp':");
        stream->print(Logger::markedUTCEpochTime);
        stream->print(
            F("000}"));  // Convert microseconds to milliseconds for ubidots
        if (i + 1 != _baseLogger->getArrayVarCount()) { stream->print(','); }
    }

    stream->print(F("}}"));
}


// This prints a fully structured post request for Ubidots to the
// specified stream.
void UbidotsPublisher::printUbidotsRequest(Stream* stream) {
    // Stream the HTTP headers for the post request
    stream->print(postHeader);
    stream->print(postEndpoint);
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(ubidotsHost);
    stream->print(tokenHeader);
    stream->print(_authentificationToken);
    stream->print(contentLengthHeader);
    stream->print(calculateJsonSize());
    stream->print(contentTypeHeader);

    // Stream the JSON itself
    printSensorDataJSON(stream);
}


// A way to begin with everything already set
void UbidotsPublisher::begin(Logger& baseLogger, Client* inClient,
                             const char* authentificationToken,
                             const char* deviceID) {
    setToken(authentificationToken);
    dataPublisher::begin(baseLogger, inClient);
    _baseLogger->setSamplingFeatureUUID(deviceID);
}
void UbidotsPublisher::begin(Logger&     baseLogger,
                             const char* authentificationToken,
                             const char* deviceID) {
    setToken(authentificationToken);
    dataPublisher::begin(baseLogger);
    _baseLogger->setSamplingFeatureUUID(deviceID);
}


// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
// int16_t EnviroDIYPublisher::postDataEnviroDIY(void)
int16_t UbidotsPublisher::publishData(Client* outClient) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(ubidotsHost, ubidotsPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));

        // copy the initial post header into the tx buffer
        snprintf(txBuffer, sizeof(txBuffer), "%s", postHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", postEndpoint);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s",
                 _baseLogger->getSamplingFeatureUUID());
        txBuffer[strlen(txBuffer)] = '/';
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        // before adding each line/chunk to the outgoing buffer, we make sure
        // there is space for that line, sending out buffer if not
        if (bufferFree() < 28) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", hostHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", ubidotsHost);

        if (bufferFree() < 47) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", tokenHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s",
                 _authentificationToken);

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
                 sizeof(txBuffer) - strlen(txBuffer), "%s", payload);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(outClient);

            txBuffer[strlen(txBuffer)] = '"';
            _baseLogger->getVarUUIDAtI(i).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            txBuffer[strlen(txBuffer)] = '"';
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", ":{");
            txBuffer[strlen(txBuffer)] = '"';
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", "value");
            txBuffer[strlen(txBuffer)] = '"';
            txBuffer[strlen(txBuffer)] = ':';
            _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            txBuffer[strlen(txBuffer)] = ',';
            txBuffer[strlen(txBuffer)] = '"';
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", "timestamp");
            txBuffer[strlen(txBuffer)] = '"';
            txBuffer[strlen(txBuffer)] = ':';
            ltoa(Logger::markedUTCEpochTime, tempBuffer, 10);  // BASE 10
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            snprintf(txBuffer + strlen(txBuffer),
                     sizeof(txBuffer) - strlen(txBuffer), "%s", "000");
            if (i + 1 != _baseLogger->getArrayVarCount()) {
                txBuffer[strlen(txBuffer)] = '}';
                txBuffer[strlen(txBuffer)] = ',';
            } else {
                txBuffer[strlen(txBuffer)] = '}';
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
        PRINTOUT(F("\n -- Unable to Establish Connection to Ubiots --"));
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

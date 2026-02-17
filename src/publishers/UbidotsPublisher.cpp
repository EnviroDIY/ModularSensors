/**
 * @file UbidotsPublisher.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
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

const char* UbidotsPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* UbidotsPublisher::contentTypeHeader =
    "\r\nContent-Type: application/json\r\n\r\n";

const char* UbidotsPublisher::payload = "{";


// Constructors
UbidotsPublisher::UbidotsPublisher() : dataPublisher() {}
UbidotsPublisher::UbidotsPublisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {}
UbidotsPublisher::UbidotsPublisher(Logger& baseLogger, Client* inClient,
                                   int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}
UbidotsPublisher::UbidotsPublisher(Logger&     baseLogger,
                                   const char* authenticationToken,
                                   const char* deviceID, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setToken(authenticationToken);
    _baseLogger->setSamplingFeatureUUID(deviceID);
    MS_DBG(F("dataPublisher object created"));
}
UbidotsPublisher::UbidotsPublisher(Logger& baseLogger, Client* inClient,
                                   const char* authenticationToken,
                                   const char* deviceID, int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setToken(authenticationToken);
    _baseLogger->setSamplingFeatureUUID(deviceID);
    MS_DBG(F("dataPublisher object created"));
}
// Destructor
UbidotsPublisher::~UbidotsPublisher() {}


void UbidotsPublisher::setToken(const char* authenticationToken) {
    _authenticationToken = authenticationToken;
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
            strlen(_baseLogger->getVarUUIDAtI(i));  // parameter ID length
        jsonLength += 11;                           //  ":{"value":
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


// A way to set members in the begin to use with a bare constructor
void UbidotsPublisher::begin(Logger& baseLogger, Client* inClient,
                             const char* authenticationToken,
                             const char* deviceID) {
    setToken(authenticationToken);
    dataPublisher::begin(baseLogger, inClient);
    _baseLogger->setSamplingFeatureUUID(deviceID);
}
void UbidotsPublisher::begin(Logger&     baseLogger,
                             const char* authenticationToken,
                             const char* deviceID) {
    setToken(authenticationToken);
    dataPublisher::begin(baseLogger);
    _baseLogger->setSamplingFeatureUUID(deviceID);
}


// This utilizes an attached modem to make a TCP connection to Ubidots and then
// streams out a post request over that connection. The return is the http
// status code of the response.
int16_t UbidotsPublisher::publishData(Client* outClient, bool) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[12] = "";
    uint16_t did_respond    = 0;
    int16_t  responseCode   = 0;
    if (_baseLogger->getSamplingFeatureUUID() == nullptr ||
        strlen(_baseLogger->getSamplingFeatureUUID()) == 0) {
        PRINTOUT(F("A sampling feature UUID must be set before publishing data "
                   "to Ubidots!"));
        return 0;
    }

    MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());

    // Open a TCP/IP connection to Ubidots
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(ubidotsHost, ubidotsPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
        txBufferInit(outClient);

        // copy the initial post header into the tx buffer
        txBufferAppend(postHeader);
        txBufferAppend(postEndpoint);
        txBufferAppend(_baseLogger->getSamplingFeatureUUID());
        txBufferAppend('/');
        txBufferAppend(HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        txBufferAppend(hostHeader);
        txBufferAppend(ubidotsHost);
        txBufferAppend(tokenHeader);
        txBufferAppend(_authenticationToken);

        txBufferAppend(contentLengthHeader);
        itoa(calculateJsonSize(), tempBuffer, 10);  // BASE 10
        txBufferAppend(tempBuffer);

        txBufferAppend(contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        txBufferAppend(payload);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            txBufferAppend('"');
            txBufferAppend(_baseLogger->getVarUUIDAtI(i));
            txBufferAppend("\":{\"value\":");
            txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
            txBufferAppend(",\"timestamp\":");
            ltoa(Logger::markedUTCUnixTime, tempBuffer, 10);  // BASE 10
            txBufferAppend(tempBuffer);
            txBufferAppend("000}");
            if (i + 1 != _baseLogger->getArrayVarCount()) {
                txBufferAppend(',');
            } else {
                txBufferAppend('}');
            }
        }

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
        PRINTOUT(F("\n -- Unable to Establish Connection to Ubidots --"));
    }

    return responseCode;
}

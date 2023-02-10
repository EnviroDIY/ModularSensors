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
        txBufferAppend(_authentificationToken);

        txBufferAppend(contentLengthHeader);
        itoa(calculateJsonSize(), tempBuffer, 10);  // BASE 10
        txBufferAppend(tempBuffer);

        txBufferAppend(contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        txBufferAppend(payload);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            txBufferAppend('"');
            txBufferAppend(_baseLogger->getVarUUIDAtI(i).c_str());
            txBufferAppend("\":{\"value\":");
            txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
            txBufferAppend(",\"timestamp\":");
            ltoa(Logger::markedUTCEpochTime, tempBuffer, 10);  // BASE 10
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
        responseCode_char[3] = 0;
        for (uint8_t i = 0; i < 3; i++) {
            responseCode_char[i] = tempBuffer[i + 9];
        }
        responseCode = atoi(responseCode_char);
    } else {
        responseCode = 504;
    }

    PRINTOUT(F("\n-- Response Code --"));
    PRINTOUT(responseCode);

    return responseCode;
}

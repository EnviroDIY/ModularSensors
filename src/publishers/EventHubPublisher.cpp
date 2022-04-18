/**
 * @file EventHubPublisher.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Implements the EventHubPublisher class.
 */

#include "EventHubPublisher.h"


// ============================================================================
//  Functions for the Azure Event Hub REST API.
// ============================================================================

// Constant values for post requests
// I want to refer to these more than once while ensuring there is only one copy
// in memory
const char* EventHubPublisher::postEndpoint  = "https://event-hub-data-logger.servicebus.windows.net/devices/messages?timeout=60";
const char* EventHubPublisher::eventHubHost = "event-hub-data-logger.servicebus.windows.net";
const int   EventHubPublisher::eventHubPort = 443;  // 443 for HTTPS; 80 for HTTP
const char* EventHubPublisher::tokenHeader   = "\r\nAuthorization: ";
// const char* EventHubPublisher::cacheHeader = 
//     "\r\nCache-Control: no-cache"; 
// const char* EventHubPublisher::connectionHeader = 
//     "\r\nConnection: close";
// const char* EventHubPublisher::transferEncodingHeader =
//     "\r\nTransfer-Encoding: chunked";
const char* EventHubPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* EventHubPublisher::contentTypeHeader =
    "\r\nContent-Type: application/json; charset=utf-8\r\n\r\n";

const char* EventHubPublisher::samplingFeatureTag = "{\"id\":\"";
const char* EventHubPublisher::timestampTag       = "\",\"timestamp\":\"";


// Constructors
EventHubPublisher::EventHubPublisher() : dataPublisher() {
    // MS_DBG(F("dataPublisher object created"));
    _registrationToken = NULL;
}
EventHubPublisher::EventHubPublisher(Logger& baseLogger, uint8_t sendEveryX,
                                       uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    // MS_DBG(F("dataPublisher object created"));
    _registrationToken = NULL;
}
EventHubPublisher::EventHubPublisher(Logger& baseLogger, Client* inClient,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    // MS_DBG(F("dataPublisher object created"));
}
EventHubPublisher::EventHubPublisher(Logger&     baseLogger,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    // MS_DBG(F("dataPublisher object created"));
}
EventHubPublisher::EventHubPublisher(Logger& baseLogger, Client* inClient,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    // MS_DBG(F("dataPublisher object created"));
}
// Destructor
EventHubPublisher::~EventHubPublisher() {}


void EventHubPublisher::setToken(const char* registrationToken) {
    _registrationToken = registrationToken;
    // MS_DBG(F("Registration token set!"));
}


// Calculates how long the JSON will be
uint16_t EventHubPublisher::calculateJsonSize() {
    uint16_t jsonLength = 7;  // {"id":"
    jsonLength += 36;          // sampling feature UUID
    jsonLength += 15;          // ","timestamp":"
    jsonLength += 25;          // markedISO8601Time
    jsonLength += 2;           //  ",
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        jsonLength += 1;   //  "
        jsonLength += 11;  // variable code (i.e. "measurement")
        jsonLength += 2;   //  ":
        jsonLength += _baseLogger->getValueStringAtI(i).length();
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            jsonLength += 1;  // ,
        }
    }
    jsonLength += 1;  // }

    return jsonLength;
}


/*
// Calculates how long the full post request will be, including headers
uint16_t EventHubPublisher::calculatePostSize()
{
    uint16_t postLength = 31;  // "POST /api/data-stream/ HTTP/1.1"
    postLength += 28;  // "\r\nHost: event-hub-data-logger.servicebus.windows.net"
    postLength += 11;  // "\r\nTOKEN: "
    postLength += 36;  // registrationToken
    // postLength += 27;  // "\r\nCache-Control: no-cache"
    // postLength += 21;  // "\r\nConnection: close"
    postLength += 20;  // "\r\nContent-Length: "
    postLength += String(calculateJsonSize()).length();
    postLength += 42;  // "\r\nContent-Type: application/json\r\n\r\n"
    postLength += calculateJsonSize();
    return postLength;
}
*/


// This prints a properly formatted JSON for EventHub to an Arduino stream
void EventHubPublisher::printSensorDataJSON(Stream* stream) {
    stream->print(samplingFeatureTag);
    stream->print(_baseLogger->getSamplingFeatureUUID());
    stream->print(timestampTag);
    stream->print(
        _baseLogger->formatDateTime_ISO8601(Logger::markedLocalEpochTime));
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


// This prints a fully structured post request for EventHub to the
// specified stream.
void EventHubPublisher::printEventHubRequest(Stream* stream) {
    // Stream the HTTP headers for the post request
    stream->print(postHeader);
    stream->print(postEndpoint);
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(eventHubHost);
    stream->print(tokenHeader);
    stream->print(_registrationToken);
    // stream->print(cacheHeader);
    // stream->print(connectionHeader);
    // stream->print(transferEncodingHeader);
    stream->print(contentLengthHeader);
    stream->print(calculateJsonSize());
    stream->print(contentTypeHeader);

    // Stream the JSON itself
    printSensorDataJSON(stream);
}


// A way to begin with everything already set
void EventHubPublisher::begin(Logger& baseLogger, Client* inClient,
                               const char* registrationToken,
                               const char* samplingFeatureUUID) {
    setToken(registrationToken);
    dataPublisher::begin(baseLogger, inClient);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
void EventHubPublisher::begin(Logger&     baseLogger,
                               const char* registrationToken,
                               const char* samplingFeatureUUID) {
    setToken(registrationToken);
    dataPublisher::begin(baseLogger);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}


// This utilizes an attached modem to make a TCP connection to the
// Azure EventHub and then streams out a post request
// over that connection.
// The return is the http status code of the response.
int16_t EventHubPublisher::publishData(Client* outClient) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    char     respondBuffer[500] = "";
    uint16_t did_respond    = 0;

    MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(eventHubHost, eventHubPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));

        // copy the initial post header into the tx buffer
        strcpy(txBuffer, postHeader);
        strcat(txBuffer, postEndpoint);
        strcat(txBuffer, HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        // before adding each line/chunk to the outgoing buffer, we make sure
        // there is space for that line, sending out buffer if not
        if (bufferFree() < 28) printTxBuffer(outClient);
        strcat(txBuffer, hostHeader);
        strcat(txBuffer, eventHubHost);

        if (bufferFree() < 47) printTxBuffer(outClient);
        strcat(txBuffer, tokenHeader);
        strcat(txBuffer, _registrationToken);

        // if (bufferFree() < 27) printTxBuffer(outClient);
        // strcat(txBuffer, cacheHeader);

        // if (bufferFree() < 21) printTxBuffer(outClient);
        // strcat(txBuffer, connectionHeader);

        // if (bufferFree() < 26) printTxBuffer(outClient);
        // strcat(txBuffer, transferEncodingHeader);

        if (bufferFree() < 26) printTxBuffer(outClient);
        strcat(txBuffer, contentLengthHeader);
        itoa(calculateJsonSize(), tempBuffer, 10);  // BASE 10
        strcat(txBuffer, tempBuffer);

        if (bufferFree() < 42) printTxBuffer(outClient);
        strcat(txBuffer, contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        if (bufferFree() < 21) printTxBuffer(outClient);
        strcat(txBuffer, samplingFeatureTag);

        if (bufferFree() < 36) printTxBuffer(outClient);
        strcat(txBuffer, _baseLogger->getSamplingFeatureUUID());

        if (bufferFree() < 42) printTxBuffer(outClient);
        strcat(txBuffer, timestampTag);
        _baseLogger->formatDateTime_ISO8601(Logger::markedLocalEpochTime)
            .toCharArray(tempBuffer, 37);
        strcat(txBuffer, tempBuffer);
        txBuffer[strlen(txBuffer)] = '"';
        txBuffer[strlen(txBuffer)] = ',';

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(outClient);

            txBuffer[strlen(txBuffer)] = '"';
            _baseLogger->getVarUUIDAtI(i).toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
            txBuffer[strlen(txBuffer)] = '"';
            txBuffer[strlen(txBuffer)] = ':';
            _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
            if (i + 1 != _baseLogger->getArrayVarCount()) {
                txBuffer[strlen(txBuffer)] = ',';
            } else {
                txBuffer[strlen(txBuffer)] = '}';
            }
        }

        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(outClient, true);

        // Wait 10 seconds for a response from the server, up to 500 characters
        MS_DBG(F("Waiting for response from server"));       
        uint32_t start = millis();
        while ((millis() - start) < 10000L && outClient->available() < 500) {
            delay(100);
            Serial.print(F("."));       
        }

        // Read only the first 500 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = outClient->readBytes(respondBuffer, 500);

        // Close the TCP/IP connection
        MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        outClient->stop();
        MS_DBG(F("Client stopped after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("\n -- Unable to Establish Connection to EventHub REST API "
                   "Portal --"));
    }

    // Print entire response
    MS_DBG(F("\n-- Response Header & Body  --\n"),did_respond, respondBuffer);

    // Process the HTTP response code
    int16_t responseCode = 0;
    if (did_respond > 0) {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++) {
            responseCode_char[i] = respondBuffer[i + 9];
        }
        responseCode = atoi(responseCode_char);
    } else {
        responseCode = 504;
    }

    PRINTOUT(F("\n-- Response Code --"));
    PRINTOUT(responseCode);

    return responseCode;
}

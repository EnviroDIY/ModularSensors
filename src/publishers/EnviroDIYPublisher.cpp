/**
 * @file EnviroDIYPublisher.cpp
 * @copyright 2020 Stroud Water Research Center
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
const char* EnviroDIYPublisher::postEndpoint  = "/api/data-stream/";
const char* EnviroDIYPublisher::enviroDIYHost = "data.envirodiy.org";
const int   EnviroDIYPublisher::enviroDIYPort = 80;
const char* EnviroDIYPublisher::tokenHeader   = "\r\nTOKEN: ";
// const unsigned char *EnviroDIYPublisher::cacheHeader = "\r\nCache-Control:
// no-cache"; const unsigned char *EnviroDIYPublisher::connectionHeader =
// "\r\nConnection: close";
const char* EnviroDIYPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* EnviroDIYPublisher::contentTypeHeader =
    "\r\nContent-Type: application/json\r\n\r\n";

const char* EnviroDIYPublisher::samplingFeatureTag = "{\"sampling_feature\":\"";
const char* EnviroDIYPublisher::timestampTag       = "\",\"timestamp\":\"";


// Constructors
EnviroDIYPublisher::EnviroDIYPublisher() : dataPublisher() {
    // MS_DBG(F("dataPublisher object created"));
    _registrationToken = NULL;
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, uint8_t sendEveryX,
                                       uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    // MS_DBG(F("dataPublisher object created"));
    _registrationToken = NULL;
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    // MS_DBG(F("dataPublisher object created"));
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger&     baseLogger,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    // MS_DBG(F("dataPublisher object created"));
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    // MS_DBG(F("dataPublisher object created"));
}
// Destructor
EnviroDIYPublisher::~EnviroDIYPublisher() {}


void EnviroDIYPublisher::setToken(const char* registrationToken) {
    _registrationToken = registrationToken;
    // MS_DBG(F("Registration token set!"));
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


/*
// Calculates how long the full post request will be, including headers
uint16_t EnviroDIYPublisher::calculatePostSize()
{
    uint16_t postLength = 31;  // "POST /api/data-stream/ HTTP/1.1"
    postLength += 28;  // "\r\nHost: data.envirodiy.org"
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


// This prints a properly formatted JSON for EnviroDIY to an Arduino stream
void EnviroDIYPublisher::printSensorDataJSON(Stream* stream) {
    stream->print(samplingFeatureTag);
    stream->print(_baseLogger->getSamplingFeatureUUID());
    stream->print(timestampTag);
    stream->print(_baseLogger->formatDateTime_ISO8601(Logger::markedEpochTime));
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
    // stream->print(cacheHeader);
    // stream->print(connectionHeader);
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

#define TEMP_BUFFER_SZ 37
void EnviroDIYPublisher::mmwPostHeader(char* tempBuffer) {
    // char tempBuffer[TEMP_BUFFER_SZ] = "";
    // copy the initial post header into the tx buffer
    strcpy(txBuffer, postHeader);
    strcat(txBuffer, postEndpoint);
    strcat(txBuffer, HTTPtag);

    // add the rest of the HTTP POST headers to the outgoing buffer
    // before adding each line/chunk to the outgoing buffer, we make sure
    // there is space for that line, sending out buffer if not
    // if (bufferFree() < 28) printTxBuffer(_outClient);
    strcat(txBuffer, hostHeader);
    strcat(txBuffer, enviroDIYHost);

    // if (bufferFree() < 47) printTxBuffer(_outClient);
    strcat(txBuffer, tokenHeader);
    strcat(txBuffer, _registrationToken);

    // if (bufferFree() < 27) printTxBuffer(_outClient);
    // strcat(txBuffer, cacheHeader);

    // if (bufferFree() < 21) printTxBuffer(_outClient);
    // strcat(txBuffer, connectionHeader);

    // if (bufferFree() < 26) printTxBuffer(_outClient);
    strcat(txBuffer, contentLengthHeader);
    itoa(calculateJsonSize(), tempBuffer, 10);  // BASE 10
    strcat(txBuffer, tempBuffer);

    // if (bufferFree() < 42) printTxBuffer(_outClient);
    strcat(txBuffer, contentTypeHeader);

    // put the start of the JSON into the outgoing response_buffer
    // if (bufferFree() < 21) printTxBuffer(_outClient);
    strcat(txBuffer, samplingFeatureTag);

    // if (bufferFree() < 36) printTxBuffer(_outClient);
    strcat(txBuffer, _baseLogger->getSamplingFeatureUUID());
}

void EnviroDIYPublisher::mmwPostDataArray(char* tempBuffer) {
    // Fill the body
    MS_DBG(F("Filling from Array"));
    strcat(txBuffer, timestampTag);
    _baseLogger->formatDateTime_ISO8601(Logger::markedEpochTime)
        .toCharArray(tempBuffer, TEMP_BUFFER_SZ);
    strcat(txBuffer, tempBuffer);
    txBuffer[strlen(txBuffer)] = '"';
    txBuffer[strlen(txBuffer)] = ',';

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        // Fill with variables
        txBuffer[strlen(txBuffer)] = '"';
        _baseLogger->getVarUUIDAtI(i).toCharArray(tempBuffer, TEMP_BUFFER_SZ);
        strcat(txBuffer, tempBuffer);
        txBuffer[strlen(txBuffer)] = '"';
        txBuffer[strlen(txBuffer)] = ':';
        _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer,
                                                      TEMP_BUFFER_SZ);
        strcat(txBuffer, tempBuffer);
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            txBuffer[strlen(txBuffer)] = ',';
        } else {
            txBuffer[strlen(txBuffer)] = '}';
        }
    }
}
void EnviroDIYPublisher::mmwPostDataQued(char* tempBuffer) {
    // Fill the body - format is per MMW requirements
    MS_START_DEBUG_TIMER;
    strcat(txBuffer, timestampTag);
    _baseLogger->formatDateTime_ISO8601(_baseLogger->queFile_epochTime)
        .toCharArray(tempBuffer, TEMP_BUFFER_SZ);
    strcat(txBuffer, tempBuffer);
    txBuffer[strlen(txBuffer)] = '"';
    txBuffer[strlen(txBuffer)] = ',';

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        // Fill with variables
        txBuffer[strlen(txBuffer)] = '"';
        _baseLogger->getVarUUIDAtI(i).toCharArray(tempBuffer, TEMP_BUFFER_SZ);
        strcat(txBuffer, tempBuffer);
        txBuffer[strlen(txBuffer)] = '"';
        txBuffer[strlen(txBuffer)] = ':';
        strncat(txBuffer, _baseLogger->queFile_nextChar,
                _baseLogger->nextStr_sz);
        //_baseLogger-> getValueStringAtI(i).toCharArray(tempBuffer, 37);
        // strcat(txBuffer, tempBuffer);
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            txBuffer[strlen(txBuffer)] = ',';
        } else {
            txBuffer[strlen(txBuffer)] = '}';
        }
        if (!_baseLogger->deSerializeReadingsNext()) break;
    }
    MS_DBG(F("Filled from SD in "), MS_PRINT_DEBUG_TIMER, F("ms\n"));
}
// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
// int16_t EnviroDIYPublisher::postDataEnviroDIY(void)

int16_t EnviroDIYPublisher::publishData(Client* _outClient) {
    char     tempBuffer[TEMP_BUFFER_SZ] = "";
    uint16_t did_respond                = 0;
    // int16_t  msg_sz                     = calculateJsonSize();
    // MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());
    // Following is record specific - start with space in buffer
    uint16_t bufferSz = bufferFree();
    if (bufferSz < (MS_SEND_BUFFER_SIZE - 50)) printTxBuffer(_outClient);

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (_outClient->connect(enviroDIYHost, enviroDIYPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));

        mmwPostHeader(tempBuffer);
        if (useQueDataSource) {
            mmwPostDataQued(tempBuffer);
        } else {
            mmwPostDataArray(tempBuffer);
        }
        MS_DEEP_DBG(F("SZEND "), bufferFree());
        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(_outClient, true);

// Poll for a response from the server with timeout
#if !defined TIMER_MMW_POST_TIMEOUT_MSEC
#define TIMER_MMW_POST_TIMEOUT_MSEC 5000L
#endif  // TIMER_MMW_POST_TIMEOUT_MSEC
#define REQUIRED_MIN_RSP_SZ 12
        uint32_t start      = millis();
        uint32_t elapsed_ms = 0;
        while ((elapsed_ms < TIMER_MMW_POST_TIMEOUT_MSEC) &&
               (did_respond < REQUIRED_MIN_RSP_SZ)) {
            delay(10);  // mS delay to poll
            did_respond = _outClient->available();
            elapsed_ms  = millis() - start;
        }
        MS_DBG(F("Rsp avl "), did_respond, F("in mS "), elapsed_ms);
        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = _outClient->readBytes(tempBuffer, REQUIRED_MIN_RSP_SZ);
        MS_DBG(F("Rsp read "), did_respond);
        // Close the TCP/IP connection
        // MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        _outClient->stop();
        MS_DBG(F("Client waited"), elapsed_ms, F("ms. Stopped after"),
               MS_PRINT_DEBUG_TIMER, F("ms"));
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
    tempBuffer[TEMP_BUFFER_SZ - 1] = 0;
    MS_DBG(F("Rsp:'"), tempBuffer, F("'"));
    PRINTOUT(F("-- Response Code --"), responseCode);
    // PRINTOUT(responseCode);

    return responseCode;
}

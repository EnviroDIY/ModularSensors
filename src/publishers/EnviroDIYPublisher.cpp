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
EnviroDIYPublisher::EnviroDIYPublisher() : dataPublisher() {
    // MS_DBG(F("dataPublisher object created"));
    _registrationToken = NULL;
    setDIYHost(enviroDIYHost);
    setTimerPostTimeout_mS(TIMER_EDP_POST_TIMEOUT_DEF_MSEC);
    setTimerPostPacing_mS(TIMER_EDP_POSTED_PACING_DEF_MSEC);
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger)
    : dataPublisher(baseLogger) {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient)
    : dataPublisher(baseLogger, inClient) {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger&     baseLogger,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID)
    : dataPublisher(baseLogger) {
    setToken(registrationToken);
    setDIYHost(enviroDIYHost);
    setTimerPostTimeout_mS(TIMER_EDP_POST_TIMEOUT_DEF_MSEC);
    setTimerPostPacing_mS(TIMER_EDP_POSTED_PACING_DEF_MSEC);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID)
    : dataPublisher(baseLogger, inClient) {
    setToken(registrationToken);
    setTimerPostTimeout_mS(TIMER_EDP_POST_TIMEOUT_DEF_MSEC);
    setTimerPostPacing_mS(TIMER_EDP_POSTED_PACING_DEF_MSEC);
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
        if (!useQueueDataSource) {
            // Actively calculate the length
            jsonLength += _baseLogger->getValueStringAtI(i).length();
            if (i + 1 != _baseLogger->getArrayVarCount()) {
                jsonLength += 1;  // ,
            }
        }
    }
    if (useQueueDataSource) {
        // Get precalculated length
        jsonLength += _baseLogger->deszq_timeVariant_sz;
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
    stream->print(_enviroDIYHost);
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
#define REQUIRED_MIN_RSP_SZ 12
#define TEMP_BUFFER_SZ (REQUIRED_MIN_RSP_SZ + 25)
#define RESPONSE_UNINIT 0xFFFE
    char     tempBuffer[TEMP_BUFFER_SZ] = "";
    uint16_t did_respond                = RESPONSE_UNINIT;

    // Following is record specific - start with space in buffer
    uint32_t elapsed_ms = 0;
    uint32_t gatewayStart_ms;
    uint16_t bufferSz = bufferFree();
    if (bufferSz < (MS_SEND_BUFFER_SIZE - 50)) printTxBuffer(outClient);

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    const int32_t CONNECT_TIMEOUT_SEC = 7;
    MS_DBG(F("Connecting client. Timer(sec)"), CONNECT_TIMEOUT_SEC);
    MS_START_DEBUG_TIMER;
    outClient->setTimeout(CONNECT_TIMEOUT_SEC);
    if (outClient->connect(_enviroDIYHost, (uint16_t)enviroDIYPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms to "),
               _enviroDIYHost, ':', enviroDIYPort);

        mmwPostHeader(tempBuffer);
        if (useQueueDataSource) {
            mmwPostDataQueued(tempBuffer);
        } else {
            mmwPostDataArray(tempBuffer);
        }
        MS_DEEP_DBG(F("SZEND "), bufferFree());
        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(outClient, true);

        // Poll for a response from the server with timeout
        gatewayStart_ms = millis();

        did_respond = 0;
        while ((elapsed_ms < _timerPostTimeout_ms) &&
               (did_respond < REQUIRED_MIN_RSP_SZ)) {
            delay(10);  // mS delay to poll
            did_respond = outClient->available();
            elapsed_ms  = millis() - gatewayStart_ms;
        }
        // MS_DBG(F("Rsp avl,"), did_respond, F("bytes in"), elapsed_ms,
        // F("mS"));
        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        memset(tempBuffer, 0, TEMP_BUFFER_SZ);
        did_respond = outClient->readBytes(tempBuffer, REQUIRED_MIN_RSP_SZ);
        // MS_DBG(F("Rsp read,"), did_respond, F("bytes in"), elapsed_ms,
        // F("mS"));
        // Close the TCP/IP connection
        // MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        outClient->stop();
        MS_DBG(F("Client stopped after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data "
                   "Portal --"));
    }

    // Process the HTTP response
    int16_t responseCode = 0;
    if (RESPONSE_UNINIT == did_respond) {
        // 901 Outside HTTP Status, No Connection to server
        responseCode = HTTPSTATUS_NC_901;
    } else if (did_respond >= REQUIRED_MIN_RSP_SZ) {
        char responseCode_char[4];
        // Put in monitor check on actual size received
        if ((did_respond + 5) >= TEMP_BUFFER_SZ) {
            PRINTOUT(F(" -- Gateway Timeout warning buffer sz small"),
                     did_respond, TEMP_BUFFER_SZ);
        }
        for (uint8_t i = 0; i < 3; i++) {
            responseCode_char[i] = tempBuffer[i + 9];
        }
        responseCode = atoi(responseCode_char);
    } else {
        // 504 Gateway Timeout
        responseCode = HTTPSTATUS_GT_504;
    }

    tempBuffer[TEMP_BUFFER_SZ - 1] = 0;
    MS_DBG(F("Rsp:'"), tempBuffer, F("'"));
    PRINTOUT(F("-- Response Code --"), responseCode, F("waited "), elapsed_ms,
             F("mS Timeout"), _timerPostTimeout_ms);

    return responseCode;
}

void EnviroDIYPublisher::setDIYHost(const char* enviroDIYHost) {
    _enviroDIYHost = enviroDIYHost;
    // MS_DBG(F("DIY Host set to "), _enviroDIYHost);
}

/* FUT: void EnviroDIYPublisher::setDIYPort(const int enviroDIYPort) {
    _enviroDIYPort = enviroDIYPort;
    MS_DBG(F("DIY Port set to "), _enviroDIYPort);
}*/

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
    strcat(txBuffer, _enviroDIYHost);

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
    _baseLogger->formatDateTime_ISO8601(Logger::markedLocalEpochTime)
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
void EnviroDIYPublisher::mmwPostDataQueued(char* tempBuffer) {
    // Fill the body - format is per MMW requirements
    //  MS_DBG(F("Filling from Queue"));
    MS_START_DEBUG_TIMER;
    strcat(txBuffer, timestampTag);
    _baseLogger->formatDateTime_ISO8601(_baseLogger->deszq_epochTime)
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
        strncat(txBuffer, _baseLogger->deszq_nextChar,
                _baseLogger->deszq_nextCharSz);
        //_baseLogger-> getValueStringAtI(i).toCharArray(tempBuffer, 37);
        // strcat(txBuffer, tempBuffer);
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            txBuffer[strlen(txBuffer)] = ',';
        } else {
            txBuffer[strlen(txBuffer)] = '}';
        }
        // Read the Next Stored character of SD
        if (!_baseLogger->deszqNextCh()) break;
    }
    MS_DBG(F("Filled from SD QUE in "), MS_PRINT_DEBUG_TIMER, F("ms"));
}

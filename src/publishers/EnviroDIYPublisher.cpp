/**
 * @file EnviroDIYPublisher.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Thomas Watson <twatson52@icloud.com>
 *
 * @brief Implements the EnviroDIYPublisher class.
 */

#include "EnviroDIYPublisher.h"

char EnviroDIYPublisher::dataBuffer[MS_DATA_BUFFER_SIZE];
int  EnviroDIYPublisher::dataBufferNumRecords;

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
const char* EnviroDIYPublisher::timestampTag       = "\",\"timestamp\":[";


// Constructors
EnviroDIYPublisher::EnviroDIYPublisher() : dataPublisher() {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}
EnviroDIYPublisher::EnviroDIYPublisher(Logger&     baseLogger,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       int         sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       int         sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
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
    char*  ptr        = (char*)&dataBuffer;
    size_t recordSize = sizeof(uint32_t) +
        sizeof(float) * _baseLogger->getArrayVarCount();

    uint16_t jsonLength = strlen(samplingFeatureTag);
    jsonLength += 36;  // sampling feature UUID
    jsonLength += strlen(timestampTag);
    // markedISO8601Time + quotes and commas
    jsonLength += dataBufferNumRecords * (25 + 2) + dataBufferNumRecords - 1;
    jsonLength += 2;  // ],
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        jsonLength += 1;   //  "
        jsonLength += 36;  // variable UUID
        jsonLength += 4;   //  ":[]

        ptr = (char*)&dataBuffer;
        ptr += sizeof(uint32_t);   // skip timestamp
        ptr += i * sizeof(float);  // and previous variables
        for (int j = 0; j < dataBufferNumRecords; j++) {
            float value;
            memcpy((void*)&value, ptr, sizeof(float));
            ptr += recordSize;

            jsonLength += _baseLogger->formatValueStringAtI(i, value).length();
            if (j + 1 != dataBufferNumRecords) {
                jsonLength += 1;  // ,
            }
        }
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            jsonLength += 1;  // ,
        }
    }
    jsonLength += 1;  // }

    return jsonLength;
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
int16_t EnviroDIYPublisher::publishData(Client* outClient) {
    // record timestamp and variable values into the data buffer

    // compute where we will record to
    size_t recordSize = sizeof(uint32_t) +
        sizeof(float) * _baseLogger->getArrayVarCount();
    char* ptr = &dataBuffer[recordSize * dataBufferNumRecords];

    memcpy(ptr, (void*)&Logger::markedLocalEpochTime, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        float value = _baseLogger->getValueAtI(i);
        memcpy(ptr, (void*)&value, sizeof(float));
        ptr += sizeof(float);
    }

    dataBufferNumRecords += 1;

    if (dataBufferNumRecords == 2) { return flushDataBuffer(outClient); }

    return 201;  // pretend everything went okay?
}

int16_t EnviroDIYPublisher::flushDataBuffer(Client* outClient) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(enviroDIYHost, enviroDIYPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
        txBufferInit(outClient);

        // copy the initial post header into the tx buffer
        txBufferAppend(postHeader);
        txBufferAppend(postEndpoint);
        txBufferAppend(HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        txBufferAppend(hostHeader);
        txBufferAppend(enviroDIYHost);
        txBufferAppend(tokenHeader);
        txBufferAppend(_registrationToken);

        txBufferAppend(contentLengthHeader);
        itoa(calculateJsonSize(), tempBuffer, 10);  // BASE 10
        txBufferAppend(tempBuffer);

        txBufferAppend(contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        txBufferAppend(samplingFeatureTag);
        txBufferAppend(_baseLogger->getSamplingFeatureUUID());

        txBufferAppend(timestampTag);

        // write out list of timestamps
        char*  ptr        = (char*)&dataBuffer;
        size_t recordSize = sizeof(uint32_t) +
            sizeof(float) * _baseLogger->getArrayVarCount();
        for (int i = 0; i < dataBufferNumRecords; i++) {
            uint32_t value;
            memcpy((void*)&value, ptr, sizeof(uint32_t));
            ptr += recordSize;

            txBufferAppend('"');
            txBufferAppend(Logger::formatDateTime_ISO8601(value).c_str());
            txBufferAppend('"');
            if (i + 1 != dataBufferNumRecords) { txBufferAppend(','); }
        }
        txBufferAppend(']');
        txBufferAppend(',');

        // write out a list of the values of each variable
        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            txBufferAppend('"');
            txBufferAppend(_baseLogger->getVarUUIDAtI(i).c_str());
            txBufferAppend('"');
            txBufferAppend(':');
            txBufferAppend('[');

            ptr = (char*)&dataBuffer;
            ptr += sizeof(uint32_t);   // skip timestamp
            ptr += i * sizeof(float);  // and previous variables
            for (int j = 0; j < dataBufferNumRecords; j++) {
                float value;
                memcpy((void*)&value, ptr, sizeof(float));
                ptr += recordSize;

                txBufferAppend(
                    _baseLogger->formatValueStringAtI(i, value).c_str());
                if (j + 1 != dataBufferNumRecords) { txBufferAppend(','); }
            }
            txBufferAppend(']');

            if (i + 1 != _baseLogger->getArrayVarCount()) {
                txBufferAppend(',');
            } else {
                txBufferAppend('}');
            }
        }

        // Write out the complete request
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
        PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data "
                   "Portal --"));
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

    if (responseCode == 201) {
        // data was successfully transmitted, we can discard it from the buffer
        dataBufferNumRecords = 0;
    }

    return responseCode;
}

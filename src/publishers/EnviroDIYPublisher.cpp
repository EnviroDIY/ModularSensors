/**
 * @file EnviroDIYPublisher.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * @copyright 2023 Thomas Watson
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Thomas Watson <twatson52@icloud.com>
 *
 * @brief Implements the EnviroDIYPublisher class.
 */

#include "EnviroDIYPublisher.h"

LogBuffer EnviroDIYPublisher::_logBuffer;

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
    : dataPublisher(baseLogger, sendEveryX) {
    _logBuffer.setNumVariables(_baseLogger->getArrayVarCount());
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    _logBuffer.setNumVariables(_baseLogger->getArrayVarCount());
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger&     baseLogger,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       int         sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    _logBuffer.setNumVariables(_baseLogger->getArrayVarCount());
}
EnviroDIYPublisher::EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                                       const char* registrationToken,
                                       const char* samplingFeatureUUID,
                                       int         sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    _logBuffer.setNumVariables(_baseLogger->getArrayVarCount());
}
// Destructor
EnviroDIYPublisher::~EnviroDIYPublisher() {}


void EnviroDIYPublisher::setToken(const char* registrationToken) {
    _registrationToken = registrationToken;
}


// Calculates how long the JSON will be
uint16_t EnviroDIYPublisher::calculateJsonSize() {
    uint8_t variables = _logBuffer.getNumVariables();
    int     records   = _logBuffer.getNumRecords();
    MS_DBG(F("Number of records in log buffer:"), records);
    MS_DBG(F("Number of variables in log buffer:"), variables);
    MS_DBG(F("Number of variables in base logger:"),
           _baseLogger->getArrayVarCount());

    uint16_t jsonLength = strlen(samplingFeatureTag);
    jsonLength += 36;  // sampling feature UUID
    jsonLength += strlen(timestampTag);
    // markedISO8601Time + quotes and commas
    jsonLength += records * (25 + 2) + records - 1;
    jsonLength += 2;  // ],
    for (uint8_t var = 0; var < variables; var++) {
        jsonLength += 1;   //  "
        jsonLength += 36;  // variable UUID
        jsonLength += 4;   //  ":[]

        for (int rec = 0; rec < records; rec++) {
            float value = _logBuffer.getRecordValue(rec, var);
            jsonLength +=
                _baseLogger->formatValueStringAtI(var, value).length();
            if (rec + 1 != records) {
                jsonLength += 1;  // ,
            }
        }
        if (var + 1 != variables) {
            jsonLength += 1;  // ,
        }
    }
    jsonLength += 1;  // }
    MS_DBG(F("Outgoing JSON size:"), jsonLength);

    return jsonLength;
}


// A way to begin with everything already set
void EnviroDIYPublisher::begin(Logger& baseLogger, Client* inClient,
                               const char* registrationToken,
                               const char* samplingFeatureUUID) {
    setToken(registrationToken);
    dataPublisher::begin(baseLogger, inClient);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    _logBuffer.setNumVariables(_baseLogger->getArrayVarCount());
}
void EnviroDIYPublisher::begin(Logger&     baseLogger,
                               const char* registrationToken,
                               const char* samplingFeatureUUID) {
    setToken(registrationToken);
    dataPublisher::begin(baseLogger);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    _logBuffer.setNumVariables(_baseLogger->getArrayVarCount());
}

bool EnviroDIYPublisher::connectionNeeded(void) {
    // compute the send interval, reducing it as the buffer gets more full so we
    // have less of a chance of losing data
    int     interval = _sendEveryX;
    uint8_t percent  = _logBuffer.getPercentFull();
    MS_DBG(F("Buffer is"), percent, F("percent full"));
    if (percent >= 50) {
        interval /= 2;
    } else if (percent >= 75) {
        interval /= 4;
    } else if (percent >= 90) {
        interval = 1;
    }

    // the programmed interval is about to be reached by the next record, or it
    // was just reached and we are trying again
    bool atSendInterval = false;
    if (interval <= 1) {
        atSendInterval = true;
    } else {
        int numRecords = _logBuffer.getNumRecords();
        // where we are relative to the interval
        int relative = (numRecords % interval);
        if (relative == (interval - 1)) {
            // the next sample will put us right at the interval
            atSendInterval = true;
        } else if (numRecords >= interval) {  // don't send the first sample
            if (relative == 0) {
                // the last sample was the interval, this is the first retry
                atSendInterval = true;
            } else if (relative == 1) {
                // two samples ago was the interval, this is the second retry
                atSendInterval = true;
            }
        }
    }

    // the initial log transmissions have not completed (we send every one
    // of the first five data points immediately for field validation)
    bool initialTransmission = _initialTransmissionsRemaining > 0;

    return atSendInterval || initialTransmission;
}

// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
int16_t EnviroDIYPublisher::publishData(Client* outClient, bool forceFlush) {
    // do we intend to flush this call? if so, we have just returned true from
    // connectionNeeded() and the internet is connected and waiting. check what
    // that function said so we know to do it after we record this data point.
    // we also flush if requested (in which case the internet is connected too)
    bool willFlush = connectionNeeded() || forceFlush;
    MS_DBG(F("Publishing record to buffer.  Will flush:"), willFlush);

    // create record to hold timestamp and variable values in the log buffer
    int record = _logBuffer.addRecord(Logger::markedLocalEpochTime);

    // write record data if the record was successfully created
    if (record >= 0) {
        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            _logBuffer.setRecordValue(record, i, _baseLogger->getValueAtI(i));
        }
    }

    if (_initialTransmissionsRemaining > 0) {
        _initialTransmissionsRemaining -= 1;
    }

    // do the data buffer flushing if we previously planned to
    if (willFlush) {
        return flushDataBuffer(outClient);
    } else {
        return 201;  // pretend everything went okay?
    }
}

int16_t EnviroDIYPublisher::flushDataBuffer(Client* outClient) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(enviroDIYHost, enviroDIYPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));
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
        int records = _logBuffer.getNumRecords();
        for (int rec = 0; rec < records; rec++) {
            txBufferAppend('"');
            uint32_t timestamp = _logBuffer.getRecordTimestamp(rec);
            txBufferAppend(Logger::formatDateTime_ISO8601(timestamp).c_str());
            txBufferAppend('"');
            if (rec + 1 != records) { txBufferAppend(','); }
        }
        txBufferAppend(']');
        txBufferAppend(',');

        // write out a list of the values of each variable
        uint8_t variables = _logBuffer.getNumVariables();
        for (uint8_t var = 0; var < variables; var++) {
            txBufferAppend('"');
            txBufferAppend(_baseLogger->getVarUUIDAtI(var).c_str());
            txBufferAppend('"');
            txBufferAppend(':');
            txBufferAppend('[');

            for (int rec = 0; rec < records; rec++) {
                float value = _logBuffer.getRecordValue(rec, var);
                txBufferAppend(
                    _baseLogger->formatValueStringAtI(var, value).c_str());
                if (rec + 1 != records) { txBufferAppend(','); }
            }
            txBufferAppend(']');

            if (var + 1 != variables) {
                txBufferAppend(',');
            } else {
                txBufferAppend('}');
            }
        }

        // Flush the complete request
        txBufferFlush();

        // Wait 30 seconds for a response from the server
        uint32_t start = millis();
        while ((millis() - start) < 30000L && outClient->available() < 12) {
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
        _logBuffer.clear();
    }

    return responseCode;
}

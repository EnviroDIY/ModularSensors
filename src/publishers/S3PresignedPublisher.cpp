/**
 * @file S3PresignedPublisher.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the S3PresignedPublisher class.
 */

#include "S3PresignedPublisher.h"

// ============================================================================
//  Functions for the S3 by way of a pre-signed URL.
// ============================================================================
// Constant values for put requests
const char* S3PresignedPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* S3PresignedPublisher::contentTypeHeader   = "\r\nContent-Type: ";

// Constructors
S3PresignedPublisher::S3PresignedPublisher() : dataPublisher() {}

S3PresignedPublisher::S3PresignedPublisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {}

S3PresignedPublisher::S3PresignedPublisher(Logger& baseLogger, Client* inClient,
                                           int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}

S3PresignedPublisher::S3PresignedPublisher(
    Logger&     baseLogger, const char* (*getUrlFxn)(const char*),
    const char* filename, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setURLUpdateFunction(getUrlFxn);
    setFilename(filename);
}
S3PresignedPublisher::S3PresignedPublisher(
    Logger&     baseLogger, const char* (*getUrlFxn)(const char*),
    const char* filename, const char* caCertName, int sendEveryX = 1)
    : dataPublisher(baseLogger, sendEveryX) {
    setURLUpdateFunction(getUrlFxn);
    setFilename(filename);
    setCACertName(caCertName);
}
S3PresignedPublisher::S3PresignedPublisher(
    Logger& baseLogger, Client* inClient, const char* (*getUrlFxn)(const char*),
    const char* filename, int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setURLUpdateFunction(getUrlFxn);
    setFilename(filename);
}
// Destructor
S3PresignedPublisher::~S3PresignedPublisher() {}


// Functions for private SWRC server
void S3PresignedPublisher::setURLUpdateFunction(
    const char* (*getUrlFxn)(const char*)) {
    _getUrlFxn = getUrlFxn;
}

void S3PresignedPublisher::setPreSignedURL(const char* s3Url) {
    _PreSignedURL = s3Url;
}


void S3PresignedPublisher::setFilename(const char* filename) {
    _filename = filename;
}


void S3PresignedPublisher::setCACertName(const char* caCertName) {
    _caCertName = caCertName;
}


// A way to set members in the begin to use with a bare constructor
void S3PresignedPublisher::begin(Logger& baseLogger, Client* inClient,
                                 const char* (*getUrlFxn)(const char*),
                                 const char* filename) {
    setURLUpdateFunction(getUrlFxn);
    setFilename(filename);
    dataPublisher::begin(baseLogger, inClient);
}
void S3PresignedPublisher::begin(Logger& baseLogger,
                                 const char* (*getUrlFxn)(const char*),
                                 const char* filename, const char* caCertName) {
    setURLUpdateFunction(getUrlFxn);
    setFilename(filename);
    setCACertName(caCertName);
    dataPublisher::begin(baseLogger);
}

Client* S3PresignedPublisher::createClient() {
    if (_baseModem == nullptr) {
        PRINTOUT(F("ERROR! No web client assigned and cannot access a "
                   "logger modem to create one!"));
        return nullptr;
    }
    if (_caCertName == nullptr) {
        PRINTOUT(F("Cannot create a new S3 secure client without a CA "
                   "certificate name!"));
        return nullptr;
    }
    MS_DBG(F("Creating new secure client with default socket number."));
    Client* newClient = _baseModem->createSecureClient(
        SSLAuthMode::CA_VALIDATION, SSLVersion::TLS1_3, _caCertName);
    if (newClient == nullptr) {
        PRINTOUT(F("Failed to create a new secure client!"));
        return nullptr;
    }
    return newClient;
}


// Post the data to S3.
int16_t S3PresignedPublisher::publishData(Client* outClient, bool) {
    // Initialise the SD card
    // skip everything else if there's no SD card, otherwise it might hang
    if (!_baseLogger->initializeSDCard()) return -2;

    // Open the file in read mode, bail if it doesn't open
    if (putFile.open(_filename, O_READ)) {
        MS_DBG(F("Opened file on SD card:"), filename);
    } else {
        MS_DBG(F("Failed to open the file to put on S3"), filename);
        return -2;
    }
    uint32_t file_size = static_cast<uint32_t>(putFile.size());

    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    // https://YOUR-BUCKET-NAME.s3.amazonaws.com/file.jpg
    //  ?AWSAccessKeyId=ACCESS-KEY-ID
    //  &Signature=SIGNATURE-VALUE
    //  &content-type=image%2Fjpeg
    //  &x-amz-security-token=A-REALLY-REALLY-REALLY-LONG-STRING
    //  &Expires=unix_timestamp

    char* start_file    = strstr(_PreSignedURL, _filename);
    char* start_content = strstr(start_file, "&content-type=");
    char* end_content   = strstr(start_content, "&");
    char* s3host        = {'\0'};
    char* content_type  = {'\0'};
    // add 8 for 'https://'
    memcpy(s3host, _PreSignedURL + 8, start_file - _PreSignedURL - 8);
    MS_DBG(F("S3 Host: "), s3host);
    // add 14 for '&content-type='
    memcpy(content_type, start_content + 14, end_content - start_content - 14);
    MS_DBG(F("Content Type: "), content_type);
    String ct_str = String(content_type);
    ct_str.replace("%2F", "/");
    MS_DBG(F("Content Type (STR): "), ct_str);

    // Open a TLS/TCP/IP connection to S3
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    // NOTE: always use port 443 for SSL connections to S3
    if (outClient->connect(s3host, 443)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));
        txBufferInit(outClient);

        // copy the initial post header into the tx buffer
        txBufferAppend(putHeader);

        // add in the file/query portion of the URL
        txBufferAppend(start_file);

        // add the rest of the HTTP GET headers to the outgoing buffer
        txBufferAppend(HTTPtag);
        txBufferAppend(hostHeader);
        txBufferAppend(s3host);
        txBufferAppend(contentLengthHeader);
        txBufferAppend(file_size);
        txBufferAppend("\r\n");
        txBufferAppend(contentTypeHeader);
        txBufferAppend(ct_str.c_str());
        txBufferAppend("\r\n\r\n");

        // Flush the complete header
        txBufferFlush();

        // Send the file
        // Take advantage of the txBuffer's flush logic to prevent typewritter
        // style writes from the modem-send command deep in TinyGSM
        for (uint32_t i = 0; i < file_size; i++) {
            txBufferAppend(putFile.read());
        }
        putFile.close();

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
        PRINTOUT(F("\n -- Unable to Establish Connection to S3 --"));
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

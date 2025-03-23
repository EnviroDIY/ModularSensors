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
const char* S3PresignedPublisher::s3_parent_host      = "s3.amazonaws.com";
const char* S3PresignedPublisher::contentLengthHeader = "\r\nContent-Length: ";
const char* S3PresignedPublisher::contentTypeHeader   = "\r\nContent-Type: ";

// Constructors
S3PresignedPublisher::S3PresignedPublisher() : dataPublisher() {}
S3PresignedPublisher::S3PresignedPublisher(Logger&     baseLogger,
                                           const char* caCertName,
                                           String (*getUrlFxn)(String),
                                           String (*getFileNameFxn)(void),
                                           int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setCACertName(caCertName);
    setURLUpdateFunction(getUrlFxn);
    setFileUpdateFunction(getFileNameFxn);
}
S3PresignedPublisher::S3PresignedPublisher(Logger& baseLogger, Client* inClient,
                                           String (*getUrlFxn)(String),
                                           String (*getFileNameFxn)(void),
                                           int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setURLUpdateFunction(getUrlFxn);
    setFileUpdateFunction(getFileNameFxn);
}
// Destructor
S3PresignedPublisher::~S3PresignedPublisher() {}


void S3PresignedPublisher::setURLUpdateFunction(String (*getUrlFxn)(String)) {
    _getUrlFxn = getUrlFxn;
}
void S3PresignedPublisher::setFileUpdateFunction(
    String (*getFileNameFxn)(void)) {
    _getFileNameFxn = getFileNameFxn;
}

void S3PresignedPublisher::setPreSignedURL(String s3Url) {
    // MS_DBG(F("Setting S3 URL to:"), s3Url);
    _PreSignedURL = s3Url;
}


void S3PresignedPublisher::setFileName(String filename) {
    // MS_DBG(F("Setting filename to:"), filename);
    _filename = filename;
}
void S3PresignedPublisher::setFileParams(const char* extension,
                                         const char* filePrefix) {
    _fileExtension = extension;
    _filePrefix    = filePrefix;
}


void S3PresignedPublisher::setCACertName(const char* caCertName) {
    _caCertName = caCertName;
}


// A way to set members in the begin to use with a bare constructor
void S3PresignedPublisher::begin(Logger& baseLogger, Client* inClient,
                                 String (*getUrlFxn)(String)) {
    setURLUpdateFunction(getUrlFxn);
    dataPublisher::begin(baseLogger, inClient);
}
void S3PresignedPublisher::begin(Logger& baseLogger,
                                 String (*getUrlFxn)(String),
                                 const char* caCertName) {
    setURLUpdateFunction(getUrlFxn);
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
    MS_DBG(F("Creating a new TinyGsmSecureClient with default socket number."));
    Client* newClient = _baseModem->createSecureClient(
        SSLAuthMode::CA_VALIDATION, SSLVersion::TLS1_3, _caCertName);
    if (newClient == nullptr) {
        PRINTOUT(F("Failed to create a new secure client!"));
        return nullptr;
    }
    return newClient;
}
void S3PresignedPublisher::deleteClient(Client* _client) {
    if (_baseModem != nullptr) {
        MS_DBG(F("Attempting to delete the client"));
        return _baseModem->deleteSecureClient(_client);
    }
}

// Post the data to S3.
int16_t S3PresignedPublisher::publishData(Client* outClient, bool) {
    // if no-one gave us a filename, assume it's a jpg and generate one based on
    // loggername + timestamp
    String filename = _filename;
    if (_getFileNameFxn != nullptr) { filename = _getFileNameFxn(); }
    if (filename.length() == 0) {
        filename = _baseLogger->generateFileName(
            true,
            _fileExtension != nullptr ? _fileExtension
                                      : S3_DEFAULT_FILE_EXTENSION,
            _filePrefix);
    }
    if (filename.length() == 0) {
        PRINTOUT(F("No filename to post to S3!"));
        return -2;
    }

    // Initialise the SD card and make sure we can get to the file
    if (!_baseLogger->initializeSDCard()) return -2;
    // Test opening the file in read mode, bail if it doesn't open
    if (putFile.open(filename.c_str(), O_READ)) {
        MS_DBG(F("Opened file on SD card:"), filename);
    } else {
        MS_DBG(F("Failed to open the file to put on S3"), filename);
        return -2;
    }
    // close the file again so we're not leaving it hanging open while we we
    // fetch the pre-signed URL
    // I'm worried about the file becoming corrupted if we leave it open and
    // something bad happens while we're waiting for the URL.
    putFile.close();

    // Run whatever function we need to get a new URL
    // Run the function any time the pre-signed URL pointer has become null
    // (after use) or if the current filename isn't in the URL (which means it's
    // not valid).
    if (_PreSignedURL.length() > 0 && _PreSignedURL.indexOf(filename) == 0) {
        PRINTOUT(F("The provided S3 URL is not valid for the current file!"));
        PRINTOUT(F("Current URL:"), _PreSignedURL);
        PRINTOUT(F("Current Filename:"), _filename);
        _PreSignedURL = "";
    }
    if (_PreSignedURL.length() == 0) {
        if (_getUrlFxn == nullptr) {
            PRINTOUT(F("No valid URL and no function to get one!"));
            return -2;
        }
        setPreSignedURL(_getUrlFxn(_filename));
        if (_PreSignedURL.length() == 0) {
            PRINTOUT(F("No URL returned to post to!"));
            return -2;
        }
    }

    // Now that we have a URL, re-itialise the SD card and re-open the file
    if (!_baseLogger->initializeSDCard()) return -2;
    if (putFile.open(filename.c_str(), O_READ)) {
        MS_DBG(F("Opened file on SD card:"), filename);
    } else {
        MS_DBG(F("Failed to open the file to put on S3"), filename);
        return -2;
    }
    // check the file size
    uint32_t file_size = static_cast<uint32_t>(putFile.size());

    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    // S3 pre-signed URL's follow the "virtual-hosted style" and have the form:
    // https://YOUR-BUCKET-NAME.s3.amazonaws.com/file_name.extension
    //  ?AWSAccessKeyId=ACCESS-KEY-ID
    //  &Signature=SIGNATURE-VALUE
    //  &content-type=image%2Fjpeg
    //  &x-amz-security-token=A-REALLY-REALLY-REALLY-LONG-STRING
    //  &Expires=unix_timestamp

    const char* url_str = _PreSignedURL.c_str();
    MS_DBG(F("Parsing S3 URL"));
    MS_DBG(F("Full S3 URL:"), _PreSignedURL);
    char* start_bucket = const_cast<char*>(url_str) + 8;
    MS_DBG(F("From start of bucket name:"), start_bucket);
    char* end_bucket = strstr(url_str, s3_parent_host) - 1;
    MS_DBG(F("From end of bucket name:"), end_bucket);
    char* start_file = strstr(url_str + 8, "/") + 1;
    MS_DBG(F("From start of object name:"), start_file);
    char* end_file = strstr(start_file, "?");
    MS_DBG(F("From end of object name:"), end_file);
    char* start_content = strstr(start_file, "&content-type=") + 1;
    // ^^ Add 1 to start at the character after the '&' so we don't include it
    MS_DBG(F("From start of content type:"), start_content);
    char* end_content = strstr(start_content, "&");
    MS_DBG(F("From end of content type:"), end_content);
    char* start_expiration = strstr(start_file, "&Expires=");
    MS_DBG(F("From start of expiration tag:"), start_expiration);

    uint32_t expiration = atoll(start_expiration + 9);
    MS_DBG(F("Expiration:"), expiration);
    if (expiration < _baseLogger->getNowUTCEpoch()) {
        PRINTOUT(F("The S3 URL has expired:"), expiration,
                 F("is less than the current time"),
                 _baseLogger->getNowUTCEpoch());
        return -2;
    }

    char s3host[81] = {'\0'};
    /// ^^ Bucket names can be 3-63 characters long; the '.s3.amazonaws.com'
    /// adds 17 characters and we add 1 more for the null terminator. This gives
    /// a total maximum length of 63 + 17 + 1 = 81
    char content_type[128] = {'\0'};
    /// ^^ Content types can be up to 128 characters long (from
    /// https://stackoverflow.com/questions/19852/maximum-length-of-a-mime-content-type-header-field)

    memcpy(s3host, url_str + 8, start_file - url_str - 9);
    // ^^ add 8 to the start for 'https://'
    // Subtract 8+1=9 from the length for the 'https://' at the beginning and
    // the final '/' before the filename
    memset(s3host + (start_file - url_str - 9), '\0',
           81 - (start_file - url_str - 9));
    // ^^ Fill the rest of the buffer with null terminators
    MS_DBG(F("S3 Host: "), s3host);

    memcpy(content_type, start_content, end_content - start_content);
    memset(content_type + (end_content - start_content), '\0',
           100 - (end_content - start_content));
    MS_DBG(F("Content Type: "), content_type);
    String ct_str = String(content_type);
    ct_str.replace("%2F", "/");
    MS_DBG(F("Content Type (STR): "), ct_str);

    // Open a TLS/TCP/IP connection to S3
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    // NOTE: always use port 443 for SSL connections to S3
    if (outClient->connect(s3_parent_host, 443)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));
        txBufferInit(outClient);

        // copy the initial post header into the tx buffer
        txBufferAppend(putHeader);

        // add in the file/query portion of the URL
        txBufferAppend(start_file);

        char file_size_str[10] = {0};
        itoa(file_size, file_size_str, 10);

        // add the rest of the HTTP GET headers to the outgoing buffer
        txBufferAppend(HTTPtag);
        txBufferAppend(hostHeader);
        txBufferAppend(s3host);
        txBufferAppend(contentLengthHeader);
        txBufferAppend(file_size_str);
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

    // After any attempt, clear the filename to force the user to set a new
    // filename before trying again
    _filename = "";

    // After a successful post or an error response, clear the URL so it's not
    // used again S3 pre-signed URL's are only valid for the post of a single
    // file. If you call the same URL repeatedly, it will overwrite the file
    // each time.
    if (responseCode == 201) { _PreSignedURL = ""; }

    return responseCode;
}

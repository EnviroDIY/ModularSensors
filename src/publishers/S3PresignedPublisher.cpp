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
const int   S3PresignedPublisher::s3Port              = 443;
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
    MS_DBG(F("Using CA cert:"), _caCertName);
    Client* newClient = _baseModem->createSecureClient(
        SSLAuthMode::CA_VALIDATION, SSLVersion::TLS1_3, _caCertName);
    if (newClient == nullptr) {
        PRINTOUT(F("Failed to create a new secure client!"));
        return nullptr;
    }
    return newClient;
}
void S3PresignedPublisher::deleteClient(Client* client) {
    if (_baseModem != nullptr) {
        MS_DBG(F("Attempting to delete the client"));
        return _baseModem->deleteSecureClient(client);
    }
}

bool S3PresignedPublisher::validateS3URL(String& s3url, char* s3host,
                                         char* s3resource, char* content_type) {
    // S3 pre-signed URL's follow the "virtual-hosted style" and have the form:
    // https://YOUR-BUCKET-NAME.s3.amazonaws.com/file_name.extension
    //  ?AWSAccessKeyId=ACCESS-KEY-ID
    //  &Signature=SIGNATURE-VALUE
    //  &content-type=image%2Fjpeg
    //  &x-amz-security-token=A-REALLY-REALLY-REALLY-LONG-STRING
    //  &Expires=unix_timestamp

    const char* url_str = s3url.c_str();  // should be null terminated!

    MS_DBG(F("Full S3 URL:"), url_str);

    if (strstr(url_str, s3_parent_host) == nullptr) {
        PRINTOUT(F("The S3 URL does not contain the S3 host name:"),
                 s3_parent_host);
        return false;
    }

#if defined MS_S3PRESIGNEDPUBLISHER_DEBUG
    char* start_bucket = const_cast<char*>(url_str) + 8;
    char* end_bucket   = strstr(url_str, s3_parent_host) - 1;
#endif
    char* start_file = strstr(url_str + 8, "/") + 1;
#if defined MS_S3PRESIGNEDPUBLISHER_DEBUG
    char* end_file = strstr(start_file, "?");
#endif
    char* start_content      = strstr(start_file, "&content-type=");
    char* start_content_type = strstr(start_content, "=") + 1;
    // ^^ Add 1 to start at the character after the '=' so we don't include it
    char* end_content      = strstr(start_content_type, "&");
    char* start_expiration = strstr(start_file, "&Expires=") + 9;

#if defined MS_S3PRESIGNEDPUBLISHER_DEBUG
    MS_SERIAL_OUTPUT.print(F("Virtual Host Name:"));
    MS_SERIAL_OUTPUT.write(url_str + 8, start_file - url_str - 9);
    MS_SERIAL_OUTPUT.print(F("\nBucket Name: "));
    MS_SERIAL_OUTPUT.write(start_bucket, end_bucket - start_bucket);
    MS_SERIAL_OUTPUT.print(F("\nObject Name: "));
    MS_SERIAL_OUTPUT.write(start_file, end_file - start_file);
    MS_SERIAL_OUTPUT.print(F("\nContent Type: "));
    MS_SERIAL_OUTPUT.write(start_content_type,
                           end_content - start_content_type);
    MS_SERIAL_OUTPUT.print(F("\nExpiration Timestamp: "));
    MS_SERIAL_OUTPUT.print(start_expiration);
    MS_SERIAL_OUTPUT.println();
#endif

    uint32_t expiration = atol(start_expiration);
    // Check basic validity of the timestamp
    if (expiration < EARLIEST_SANE_UNIX_TIMESTAMP ||
        expiration > LATEST_SANE_UNIX_TIMESTAMP) {
        PRINTOUT(F("The S3 URL timestamp outside of sane ranges:"), expiration);
        return false;
    }
    // If expiration is in the past, it's not valid
    if (expiration < static_cast<uint32_t>(_baseLogger->getNowUTCEpoch())) {
        PRINTOUT(F("The S3 URL has expired:"), expiration,
                 F("is less than the current time"),
                 static_cast<uint32_t>(_baseLogger->getNowUTCEpoch()));
        return false;
    }

    // Put the full virtual host into the provided buffer
    memcpy(s3host, url_str + 8, start_file - url_str - 9);
    // ^^ add 8 to the start for 'https://'
    // Subtract 8+1=9 from the length for the 'https://' at the beginning and
    // the final '/' before the filename
    memset(s3host + (start_file - url_str - 9), '\0', 1);
    // ^^ Null terminate, just in case

    // Put rest of the URL into the provided resource buffer
    strcpy(s3resource, start_file - 1);
    // ^^ subtract 1 for the '/'
    // I hope the buffer they gave was big enough!

    // make a temporary buffer for the de-escaping the content type
    char ct_str[128] = {'\0'};
    memcpy(ct_str, start_content_type, end_content - start_content_type);
    memset(ct_str + (end_content - start_content_type), '\0', 1);
    String ct_esc_str = String(ct_str);
    ct_esc_str.replace("%2F", "/");
    uint8_t ct_str_len = ct_esc_str.length();
    MS_DBG(F("De-Escaped Content Type:"), ct_esc_str);

    memcpy(content_type, ct_esc_str.c_str(), ct_str_len);
    memset(content_type + (ct_str_len), '\0', 1);
    // ^^ Null terminate, just in case

    return true;
}

// Post the data to S3.
int16_t S3PresignedPublisher::publishData(Client* outClient, bool) {
    // Create a buffer for the portions of the request and response
    char     tempBuffer[12] = "";
    uint16_t did_respond    = 0;
    int16_t  responseCode   = 0;

    // if no-one gave us a filename, assume it's a jpg and generate one based on
    // logger name + timestamp
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

#if defined(MS_S3PRESIGNED_VALIDATE_URL_FILENAME)
    if (_PreSignedURL.length() > 0 && _PreSignedURL.indexOf(filename) == 0) {
        PRINTOUT(F("The provided S3 URL is not valid for the current file!"));
        PRINTOUT(F("Current URL:"), _PreSignedURL);
        PRINTOUT(F("Current Filename:"), _filename);
        _PreSignedURL = "";
    }
#endif

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

    // Now that we have a URL, re-initialise the SD card and re-open the file
    if (!_baseLogger->initializeSDCard()) return -2;
    if (putFile.open(filename.c_str(), O_READ)) {
        MS_DBG(F("Opened file on SD card:"), filename);
    } else {
        MS_DBG(F("Failed to open the file to put on S3"), filename);
        return -2;
    }
    // check the file size
    uint32_t file_size = static_cast<uint32_t>(putFile.size());


    char s3host[81] = {'\0'};
    /// ^^ Bucket names can be 3-63 characters long; the '.s3.amazonaws.com'
    /// adds 17 characters and we add 1 more for the null terminator. This gives
    /// a total maximum length of 63 + 17 + 1 = 81
    char s3resource[_PreSignedURL.length()] = {'\0'};
    /// ^^ Allow up to the full length of the URL for the resource
    char content_type[128] = {'\0'};
    /// ^^ Content types can be up to 128 characters long (from
    /// https://stackoverflow.com/questions/19852/maximum-length-of-a-mime-content-type-header-field)

    if (!validateS3URL(_PreSignedURL, s3host, s3resource, content_type)) {
        return -2;
    }

    // Open a TLS/TCP/IP connection to S3
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    // NOTE: always use port 443 for SSL connections to S3
    if (outClient->connect(s3_parent_host, s3Port)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
        txBufferInit(outClient);

        // copy the initial post header into the tx buffer
        txBufferAppend(putHeader);

        // add in the file/query portion of the URL
        txBufferAppend(s3resource);

        char file_size_str[10] = {0};
        itoa(file_size, file_size_str, 10);

        // add the rest of the HTTP GET headers to the outgoing buffer
        txBufferAppend(HTTPtag);
        txBufferAppend(hostHeader);
        txBufferAppend(s3host);
        txBufferAppend(contentTypeHeader);
        txBufferAppend(content_type);
        txBufferAppend(contentLengthHeader);
        txBufferAppend(file_size_str);
        txBufferAppend("\r\n\r\n");

        // Flush the complete header
        txBufferFlush();

        // Send the file
        // Take advantage of the txBuffer's flush logic to prevent typewriter
        // style writes from the modem-send command deep in TinyGSM
        // Disable the watch-dog timer to reduce interrupts during transfer
        // MS_DBG(F("Disabling the watchdog during file transfer"));
        extendedWatchDog::disableWatchDog();
        for (uint32_t i = 0; i < file_size; i++) {
            txBufferAppend(putFile.read(), false);
        }
        txBufferFlush(false);
        // Re-enable the watchdog
        // MS_DBG(F("Re-enabling the watchdog after file transfer"));
        extendedWatchDog::enableWatchDog();
        // close the file now that we're done with it
        putFile.close();

        // Wait 60 seconds for a response from the server
        uint32_t start = millis();
        while ((millis() - start) < 60000L && outClient->connected() &&
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
        PRINTOUT(F("\n -- Unable to Establish Connection to S3 --"));
    }

#if defined(MS_S3PRESIGNED_PREVENT_REUSE)
    // After any attempt, clear the filename to force the user to set a new
    // filename before trying again
    _filename = "";

    // After a successful post or an error response, clear the URL so it's not
    // used again S3 pre-signed URL's are only valid for the post of a single
    // file. If you call the same URL repeatedly, it will overwrite the file
    // each time.
    if (responseCode == 200) { _PreSignedURL = ""; }
#endif

    return responseCode;
}

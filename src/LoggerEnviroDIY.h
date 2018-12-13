/*
 *LoggerEnviroDIY.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

// Header Guards
#ifndef LoggerEnviroDIY_h
#define LoggerEnviroDIY_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Send Buffer
// This determines how many characters to set out at once over the TCP/UDP
// connection.  Increasing this may decrease data use by a loger, while
// decreasing it will save memory.  Do not make it smaller than 47 (to keep all
// variable values with their UUID's) or bigger than 1500 (a typical TCP/UDP
// Maximum Transmission Unit).
#define LOGGER_SEND_BUFFER_SIZE 750

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerBase.h"
#include "LoggerModem.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class LoggerEnviroDIY : public Logger
{
public:
    // Constructor
    LoggerEnviroDIY(const char *loggerID, uint16_t loggingIntervalMinutes,
                    int8_t SDCardPin, int8_t mcuWakePin,
                    VariableArray *inputArray);
    virtual ~LoggerEnviroDIY();

    // Adds a loggerModem objct to the logger
    // loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
    void attachModem(loggerModem& modem);

    // Adds the site registration token
    void setToken(const char *registrationToken);

    // Adds the sampling feature UUID
    void setSamplingFeatureUUID(const char *samplingFeature);

    // Calculates how long the JSON will be
    uint16_t calculateJsonSize();
    // Calculates how long the full post request will be, including headers
    // uint16_t calculatePostSize();

    // This fills the TX buffer with nulls ('\0')
    static void emptyTxBuffer(void);
    // This writes the TX buffer to a stream and also to the debugging port
    static void printTxBuffer(Stream *stream);

    // This adds extra data to the datafile header
    void printFileHeader(Stream *stream) override;

    // This generates a properly formatted JSON for EnviroDIY
    void printSensorDataJSON(Stream *stream);

    // This prints a fully structured post request for WikiWatershed/EnviroDIY
    // to the specified stream.
    void printEnviroDIYRequest(Stream *stream);

    // This writes the post request to a "queue" file for later
    bool queueDataEnviroDIY(void);

    // This utilizes an attached modem to make a TCP connection to the
    // EnviroDIY/ODM2DataSharingPortal and then streams out a post request
    // over that connection.
    // The return is the http status code of the response.
    int16_t postDataEnviroDIY(void);

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //

    // This defines what to do in the testing mode
    // This version particularly highlights the modem signal strength!
    virtual void testingMode() override;

    // This calls all of the setup functions - must be run AFTER init
    // This version syncs the clock!
    virtual void beginAndSync(void);

    // This is a one-and-done to log data
    virtual void logDataAndSend(void);

    // The internal modem instance
    loggerModem *_logModem;
    // NOTE:  The internal _logModem must be a POINTER not a reference because
    // it is possible for no modem to be attached (and thus the pointer could
    // be null).  It is not possible to have a null reference.

protected:
    static char txBuffer[LOGGER_SEND_BUFFER_SIZE];
    static int bufferFree(void);

    // portions POST request
    static const char *postHeader;
    static const char *HTTPtag;
    static const char *hostHeader;
    static const char *enviroDIYHost;
    static const char *tokenHeader;
    // static const char *cacheHeader;
    // static const char *connectionHeader;
    static const char *contentLengthHeader;
    static const char *contentTypeHeader;

    // portions of the JSON
    static const char *samplingFeatureTag;
    static const char *timestampTag;

private:
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
};

#endif  // Header Guard

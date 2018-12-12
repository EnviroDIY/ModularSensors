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
#ifdef LoggerEnviroDIY_DBG
#define DEBUGGING_SERIAL_OUTPUT Serial
#endif //
// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerBase.h"
#include "LoggerModem.h"
typedef int (*ini_handler)( const char* section,
                           const char* name, const char* value);
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

    // This adds extra data to the datafile header
    void printFileHeader(Stream *stream) override;

    // This generates a properly formatted JSON for EnviroDIY
    uint16_t printSensorDataJSON(Stream *stream);

    // This prints a fully structured post request for WikiWatershed/EnviroDIY
    // to the specified stream.
    uint16_t printEnviroDIYRequest(Stream *stream);

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
    virtual void beginRtc(void);
    virtual void beginLogger(void);
    virtual void timeSync(void);
    virtual bool parseIniSd(const char *ini_fn,ini_handler handler_fn);

    // This is a one-and-done to log data
    virtual void logDataAndSend(void);

    // The internal modem instance
    loggerModem *_logModem;
    // NOTE:  The internal _logModem must be a POINTER not a reference because
    // it is possible for no modem to be attached (and thus the pointer could
    // be null).  It is not possible to have a null reference.
    uint32_t tx_chars=0;
    uint32_t rx_chars=0;


private:
    virtual int8_t inihParseFile(ini_handler handler_fn);
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
};

#endif  // Header Guard

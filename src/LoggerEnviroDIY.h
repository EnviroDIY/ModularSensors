/*
 *LoggerEnviroDIY.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#ifndef LoggerEnviroDIY_h
#define LoggerEnviroDIY_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "LoggerModem.h"  // To communicate with the internet
#include "LoggerBase.h"

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
    virtual ~LoggerEnviroDIY(){}

    // Adds a loggerModem objct to the logger
    // loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
    void attachModem(loggerModem& modem);

    // Adds the site registration token
    void setToken(const char *registrationToken);

    // Adds the sampling feature UUID
    void setSamplingFeatureUUID(const char *samplingFeature);

    // This adds extra data to the datafile header
    String generateFileHeader(void) override;
    // This prints a header onto a stream - this removes need to pass around
    // very long string objects which can crash the logger
    void streamFileHeader(Stream *stream) override;

    // This generates a properly formatted JSON for EnviroDIY
    String generateSensorDataJSON(void);
    void streamSensorDataJSON(Stream *stream);
    void streamSensorDataJSON(Stream& stream);

    // // This generates a fully structured POST request for EnviroDIY
    // String generateEnviroDIYPostRequest(String enviroDIYjson);
    // String generateEnviroDIYPostRequest(void);

    // This prints a fully structured post request for EnviroDIY to the
    // specified stream using the specified json.
    // This may be necessary to work around very long strings for the post request.
    void streamEnviroDIYRequest(Stream *stream, String& enviroDIYjson);
    void streamEnviroDIYRequest(Stream& stream, String& enviroDIYjson);
    // This prints a fully structured post request for EnviroDIY to the
    // specified stream with the default json.
    void streamEnviroDIYRequest(Stream *stream);
    void streamEnviroDIYRequest(Stream& stream);

    // This utilizes an attached modem to make a TCP connection to the
    // EnviroDIY/ODM2DataSharingPortal and then streams out a post request
    // over that connection.
    // The return is the http status code of the response.
    int postDataEnviroDIY(String& enviroDIYjson = LOGGER_EMPTY);

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
    virtual void logAndSend(void);

    // The internal modem instance
    loggerModem *_logModem;
    // NOTE:  The internal _logModem must be a POINTER not a reference because
    // it is possible for no modem to be attached (and thus the pointer could
    // be null).  It is not possible to have a null reference.


private:
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
};

#endif

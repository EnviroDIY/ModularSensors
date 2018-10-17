/*
 *LoggerDreamHost.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

// Header Guards
#ifndef LoggerDreamHost_h
#define LoggerDreamHost_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerEnviroDIY.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

class LoggerDreamHost : public LoggerEnviroDIY
{

public:
    // Constructor
    LoggerDreamHost(const char *loggerID, uint16_t loggingIntervalMinutes,
                    int8_t SDCardPin, int8_t mcuWakePin,
                    VariableArray *inputArray);
    virtual ~LoggerDreamHost();

    // Functions for private SWRC server
    void setDreamHostPortalRX(const char *URL);

    // This creates all of the URL parameters
    void printSensorDataDreamHost(Stream *stream);

    // This prints a fully structured GET request for DreamHost to the
    // specified stream.
    void printDreamHostRequest(Stream *stream);

    // This utilizes an attached modem to make a TCP connection to the
    // DreamHost URL and then streams out a get request
    // over that connection.
    // The return is the http status code of the response.
    int postDataDreamHost(void);

    // This prevents the logging function from dual-posting to EnviroDIY
    void disableDualPost(void);

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //

    // This is a one-and-done to log data
    virtual void logAndSend(void) override;

private:
    const char *_DreamHostPortalRX;
    bool _dualPost = true;
};

#endif  // Header Guard

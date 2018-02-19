/*
 *LoggerDreamHost.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

#ifndef LoggerDreamHost_h
#define LoggerDreamHost_h

#include <Arduino.h>

#define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "LoggerEnviroDIY.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

class LoggerDreamHost : public LoggerEnviroDIY
{

public:
    // Functions for private SWRC server
    void setDreamHostPortalRX(const char *URL);

    // This creates all of the URL parameters
    String generateSensorDataDreamHost(void);

    // Communication functions
    void streamDreamHostRequest(Stream *stream);
    
    // Post the data to dream host.
    int postDataDreamHost(void);

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //

    // This is a one-and-done to log data
    virtual void log(void) override;

private:
    const char *_DreamHostPortalRX;
};

#endif

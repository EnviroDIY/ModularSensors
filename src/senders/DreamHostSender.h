/*
 *DreamHostSender.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

// Header Guards
#ifndef DreamHostSender_h
#define DreamHostSender_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "senders/EnviroDIYSender.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

class DreamHostSender : public dataSender
{

public:
    // Constructor
    DreamHostSender(Logger& baseLogger,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    DreamHostSender(Logger& baseLogger, const char *URL,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    virtual ~DreamHostSender();

    // Returns the data destination
    virtual String getEndpoint(void){return String(dreamhostHost);}

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
    // int16_t postDataDreamHost(void);
    int16_t sendData(Client *_outClient);

protected:
    // portions of the GET request
    static const char *dreamhostHost;
    static const char *loggerTag;
    static const char *timestampTagDH;


private:
    const char *_DreamHostPortalRX;
    bool _dualPost = true;
};

#endif  // Header Guard

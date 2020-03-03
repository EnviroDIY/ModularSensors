/*
 *DreamHostPublisher.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to
 *DreamHost
 */

// Header Guards
#ifndef DreamHostPublisher_h
#define DreamHostPublisher_h

// Debugging Statement
// #define MS_DREAMHOSTPUBLISHER_DEBUG

#ifdef MS_DREAMHOSTPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "DreamHostPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

class DreamHostPublisher : public dataPublisher {
 public:
    // Constructors
    DreamHostPublisher();
    DreamHostPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                       uint8_t sendOffset = 0);
    DreamHostPublisher(Logger& baseLogger, Client* inClient,
                       uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    DreamHostPublisher(Logger& baseLogger, const char* dhUrl,
                       uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    DreamHostPublisher(Logger& baseLogger, Client* inClient, const char* dhUrl,
                       uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~DreamHostPublisher();

    // Returns the data destination
    virtual String getEndpoint(void) {
        return String(dreamhostHost);
    }

    // Functions for private SWRC server
    void setDreamHostPortalRX(const char* dhUrl);

    // This creates all of the URL parameters
    void printSensorDataDreamHost(Stream* stream);

    // This prints a fully structured GET request for DreamHost to the
    // specified stream.
    void printDreamHostRequest(Stream* stream);

    // A way to begin with everything already set
    void begin(Logger& baseLogger, Client* inClient, const char* dhUrl);
    void begin(Logger& baseLogger, const char* dhUrl);

    // This utilizes an attached modem to make a TCP connection to the
    // DreamHost URL and then streams out a get request
    // over that connection.
    // The return is the http status code of the response.
    // int16_t postDataDreamHost(void);
    int16_t publishData(Client* _outClient);

 protected:
    // portions of the GET request
    static const char* dreamhostHost;
    static const int   dreamhostPort;
    static const char* loggerTag;
    static const char* timestampTagDH;


 private:
    const char* _DreamHostPortalRX;
    bool        _dualPost = true;
};

#endif  // Header Guard

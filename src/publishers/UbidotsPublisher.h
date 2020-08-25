/*
 *EnviroDIYPublisher.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

// Header Guards
#ifndef UbidotsPublisher_h
#define UbidotsPublisher_h

// Debugging Statement
#define UBIDOTSPUBLISHER_DEBUG

#ifdef MS_UBIDOTSPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "UbidotsPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class UbidotsPublisher : public dataPublisher
{
public:
    // Constructors
    UbidotsPublisher();
    UbidotsPublisher(Logger& baseLogger,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    UbidotsPublisher(Logger& baseLogger, Client *inClient,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    UbidotsPublisher(Logger& baseLogger,
                    const char *authentificationToken,
                    const char *deviceID,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    UbidotsPublisher(Logger& baseLogger, Client *inClient,
                    const char *authentificationToken,
                    const char *deviceID,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~UbidotsPublisher();

    // Returns the data destination
    virtual String getEndpoint(void){return String(ubidotsHost);}

    // Adds the site registration token
    void setToken(const char *authentificationToken);

    // Calculates how long the JSON will be
    uint16_t calculateJsonSize();
    // Calculates how long the full post request will be, including headers
    // uint16_t calculatePostSize();

    // This generates a properly formatted JSON for EnviroDIY
    void printSensorDataJSON(Stream *stream);

    // This prints a fully structured post request for WikiWatershed/EnviroDIY
    // to the specified stream.
    void printUbidotsRequest(Stream *stream);

    // A way to begin with everything already set
    void begin(Logger& baseLogger, Client *inClient,
               const char *authentificationToken,
               const char *deviceID);
    void begin(Logger& baseLogger,
              const char *authentificationToken,
              const char *deviceID);

    // This utilizes an attached modem to make a TCP connection to the
    // EnviroDIY/ODM2DataSharingPortal and then streams out a post request
    // over that connection.
    // The return is the http status code of the response.
    // int16_t postDataEnviroDIY(void);
    virtual int16_t publishData(Client *_outClient);

protected:

    // portions of the POST request
    static const char *postEndpoint;
    static const char *ubidotsHost;
    static const int ubidotsPort;
    static const char *tokenHeader;
    // static const char *cacheHeader;
    // static const char *connectionHeader;
    static const char *contentLengthHeader;
    static const char *contentTypeHeader;

    // portions of the JSON
    static const char *payload;

private:
    // Tokens and UUID's for EnviroDIY
    const char *_authentificationToken;
};

#endif  // Header Guard

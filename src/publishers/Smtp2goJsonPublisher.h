/*
 *Smtp2goJsonPublisher.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
 */

// Header Guards
#ifndef Smtp2goJsonPublisher_h
#define Smtp2goJsonPublisher_h

// Debugging Statement
// #define MS_ENVIRODIYPUBLISHER_DEBUG

#ifdef MS_SMTP2GOJSONPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "Smtp2goJsonPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"

// ============================================================================
//  Functions for the Smtp2goJson data portal receivers.
// ============================================================================
class Smtp2goJsonPublisher : public dataPublisher {
 public:
    // Constructors
    Smtp2goJsonPublisher();
    Smtp2goJsonPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                         uint8_t sendOffset = 0);
    Smtp2goJsonPublisher(Logger& baseLogger, Client* inClient,
                         uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    Smtp2goJsonPublisher(Logger& baseLogger, const char* registrationToken,
                         // const char *samplingFeatureUUID,
                         uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    Smtp2goJsonPublisher(Logger& baseLogger, Client* inClient,
                         const char* registrationToken,
                         // const char *samplingFeatureUUID,
                         uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~Smtp2goJsonPublisher();

    // Returns the data destination
    virtual String getEndpoint(void) {
        return String(Smtp2goJsonHost);
    }

    // Adds the site registration token
    void setToken(const char* registrationToken);

    // Calculates how long the JSON will be
    uint16_t calculateJsonSize();
    // Calculates how long the full post request will be, including headers
    // uint16_t calculatePostSize();

    // This generates a properly formatted JSON for Smtp2goJson
    void printSensorDataJSON(Stream* stream);

    // This prints a fully structured post request for WikiWatershed/Smtp2goJson
    // to the specified stream.
    void printSmtp2goJsonRequest(Stream* stream);

    // A way to begin with everything already set
    void begin(Logger& baseLogger, Client* inClient,
               const char* registrationToken);
    // const char *samplingFeatureUUID);
    void begin(Logger& baseLogger, const char* registrationToken);
    // const char *samplingFeatureUUID);

    // This utilizes an attached modem to make a TCP connection to the
    // Smtp2goJson/ODM2DataSharingPortal and then streams out a post request
    // over that connection.
    // The return is the http status code of the response.
    // int16_t postDataSmtp2goJson(void);
    virtual int16_t publishData(Client* _outClient);

 protected:
    // portions of the POST request
    static const char* postEndpoint;
    static const char* Smtp2goJsonHost;
    static const int   Smtp2goJsonPort;
    // static const char *tokenHeader;
    // static const char *cacheHeader;
    // static const char *connectionHeader;
    static const char* contentLengthHeader;
    static const char* contentTypeHeader;

    // portions of the JSON
    // static const char *samplingFeatureTag;
    // static const char *timestampTag;

 private:
    // Tokens and UUID's for Smtp2goJson
    const char* _registrationToken;
};

#endif  // Header Guard

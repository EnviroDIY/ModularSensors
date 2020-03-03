/*
 *EnviroDIYPublisher.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *Copyright 2020 Stroud Water Research Center
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
 */

// Header Guards
#ifndef SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_
#define SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

// Debugging Statement
// #define MS_ENVIRODIYPUBLISHER_DEBUG

#ifdef MS_ENVIRODIYPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "EnviroDIYPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class EnviroDIYPublisher : public dataPublisher {
 public:
    // Constructors
    EnviroDIYPublisher();
    explicit EnviroDIYPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                                uint8_t sendOffset = 0);
    EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                       uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    EnviroDIYPublisher(Logger& baseLogger, const char* registrationToken,
                       const char* samplingFeatureUUID, uint8_t sendEveryX = 1,
                       uint8_t sendOffset = 0);
    EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                       const char* registrationToken,
                       const char* samplingFeatureUUID, uint8_t sendEveryX = 1,
                       uint8_t sendOffset = 0);
    // Destructor
    virtual ~EnviroDIYPublisher();

    // Returns the data destination
    virtual String getEndpoint(void) {
        return String(enviroDIYHost);
    }

    // Adds the site registration token
    void setToken(const char* registrationToken);

    // Calculates how long the JSON will be
    uint16_t calculateJsonSize();
    // Calculates how long the full post request will be, including headers
    // uint16_t calculatePostSize();

    // This generates a properly formatted JSON for EnviroDIY
    void printSensorDataJSON(Stream* stream);

    // This prints a fully structured post request for WikiWatershed/EnviroDIY
    // to the specified stream.
    void printEnviroDIYRequest(Stream* stream);

    // A way to begin with everything already set
    void begin(Logger& baseLogger, Client* inClient,
               const char* registrationToken, const char* samplingFeatureUUID);
    void begin(Logger& baseLogger, const char* registrationToken,
               const char* samplingFeatureUUID);

    // This utilizes an attached modem to make a TCP connection to the
    // EnviroDIY/ODM2DataSharingPortal and then streams out a post request
    // over that connection.
    // The return is the http status code of the response.
    // int16_t postDataEnviroDIY(void);
    virtual int16_t publishData(Client* _outClient);

 protected:
    // portions of the POST request
    static const char* postEndpoint;
    static const char* enviroDIYHost;
    static const int   enviroDIYPort;
    static const char* tokenHeader;
    // static const char *cacheHeader;
    // static const char *connectionHeader;
    static const char* contentLengthHeader;
    static const char* contentTypeHeader;

    // portions of the JSON
    static const char* samplingFeatureTag;
    static const char* timestampTag;

 private:
    // Tokens and UUID's for EnviroDIY
    const char* _registrationToken;
};

#endif  // SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

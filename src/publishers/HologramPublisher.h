/**
 * @file HologramPublisher.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the HologramPublisher subclass of dataPublisher for
 * publishing data to the Hologram API
 */

// Header Guards
#ifndef SRC_PUBLISHERS_HOLOGRAMPUBLISHER_H_
#define SRC_PUBLISHERS_HOLOGRAMPUBLISHER_H_

// Debugging Statement
// #define MS_HOLOGRAMPUBLISHER_DEBUG

#ifdef MS_HOLOGRAMPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "HologramPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class HologramPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new Hologram Publisher object with no members set.
     */
    HologramPublisher();
    HologramPublisher(Logger& baseLogger,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    HologramPublisher(Logger& baseLogger, Client *inClient,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    HologramPublisher(Logger& baseLogger,
                    const char *registrationToken,
                    const char *samplingFeatureUUID,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    HologramPublisher(Logger& baseLogger, Client *inClient,
                    const char *registrationToken,
                    const char *samplingFeatureUUID,
                    uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~HologramPublisher();

    // Returns the data destination
    virtual String getEndpoint(void){return String(hologramHost);}

    // Adds the site registration token
    void setToken(const char *registrationToken);

    // Calculates how long the JSON will be
    uint16_t calculateJsonSize();
    // Calculates how long the full post request will be, including headers
    // uint16_t calculatePostSize();

    // This generates a properly formatted JSON for EnviroDIY
    void printSensorDataJSON(Stream *stream);

    // This prints a fully structured post request for WikiWatershed/EnviroDIY
    // to the specified stream.
    void printEnviroDIYRequest(Stream *stream);

    // A way to begin with everything already set
    void begin(Logger& baseLogger, Client *inClient,
               const char *registrationToken,
               const char *samplingFeatureUUID);
    void begin(Logger& baseLogger,
              const char *registrationToken,
              const char *samplingFeatureUUID);

    // This utilizes an attached modem to make a TCP connection to the
    // EnviroDIY/ODM2DataSharingPortal and then streams out a post request
    // over that connection.
    // The return is the http status code of the response.
    // int16_t postDataEnviroDIY(void);
    virtual int16_t publishData(Client *_outClient);

protected:

    // portions of the POST request
    static const char* postEndpoint;  ///< The endpoint
    static const char* hologramHost;  ///< The host name
    static const int   hologramPort;  ///< The host port
    static const char* authHeader;    ///< The authorization header text
    // static const char *cacheHeader;  ///< The cache header text
    // static const char *connectionHeader;  ///< The keep alive header text
    static const char* contentLengthHeader;  ///< The content length header text
    static const char* contentTypeHeader;    ///< The content type header text

    // portions of the JSON
    static const char* deviceIdTag; ///< The Hologram device ID tag
    static const char* bodyTag;     ///< The Hologram body content tag
    static const char* deviceId;    ///< The Hologram device ID

 private:
    // Tokens and UUID's for EnviroDIY
    const char* _registrationToken;
};

#endif  // SRC_PUBLISHERS_HOLOGRAMPUBLISHER_H_

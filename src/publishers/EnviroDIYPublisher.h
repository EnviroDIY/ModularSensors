/**
 * @file EnviroDIYPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Thomas Watson <twatson52@icloud.com>
 *
 * @brief Contains the EnviroDIYPublisher subclass of dataPublisher for
 * publishing data to the Monitor My Watershed/EnviroDIY data portal at
 * http://data.enviroDIY.org
 */

// Header Guards
#ifndef SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_
#define SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

// Debugging Statement
// #define MS_ENVIRODIYPUBLISHER_DEBUG

#ifndef MS_DATA_BUFFER_SIZE
#define MS_DATA_BUFFER_SIZE 8192
#endif

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
/**
 * @brief The EnviroDIYPublisher subclass of dataPublisher for publishing data
 * to the Monitor My Watershed/EnviroDIY data portal at
 * http://data.enviroDIY.org
 *
 * @ingroup the_publishers
 */
class EnviroDIYPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new EnviroDIY Publisher object with no members set.
     */
    EnviroDIYPublisher();
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    explicit EnviroDIYPublisher(Logger& baseLogger, int sendEveryX = 1);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                       int sendEveryX = 1);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param registrationToken The registration token for the site on the
     * Monitor My Watershed data portal.
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    EnviroDIYPublisher(Logger& baseLogger, const char* registrationToken,
                       const char* samplingFeatureUUID, int sendEveryX = 1);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param registrationToken The registration token for the site on the
     * Monitor My Watershed data portal.
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                       const char* registrationToken,
                       const char* samplingFeatureUUID, int sendEveryX = 1);
    /**
     * @brief Destroy the EnviroDIY Publisher object
     */
    virtual ~EnviroDIYPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(enviroDIYHost);
    }

    // Adds the site registration token
    /**
     * @brief Set the site registration token
     *
     * @param registrationToken The registration token for the site on the
     * Monitor My Watershed data portal.
     */
    void setToken(const char* registrationToken);

    /**
     * @brief Calculates how long the outgoing JSON will be
     *
     * @return uint16_t The number of characters in the JSON object.
     */
    uint16_t calculateJsonSize();


    // A way to begin with everything already set
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param registrationToken The registration token for the site on the
     * Monitor My Watershed data portal.
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* registrationToken, const char* samplingFeatureUUID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param registrationToken The registration token for the site on the
     * Monitor My Watershed data portal.
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     */
    void begin(Logger& baseLogger, const char* registrationToken,
               const char* samplingFeatureUUID);

    /**
     * @brief Utilize an attached modem to open a a TCP connection to the
     * EnviroDIY/ODM2DataSharingPortal and then stream out a post request over
     * that connection.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @return **int16_t** The http status code of the response.
     */
    int16_t publishData(Client* outClient) override;

 protected:
    /**
     * @anchor envirodiy_post_vars
     * @name Portions of the POST request to EnviroDIY
     *
     * @{
     */
    static const char* postEndpoint;         ///< The endpoint
    static const char* enviroDIYHost;        ///< The host name
    static const int   enviroDIYPort;        ///< The host port
    static const char* tokenHeader;          ///< The token header text
    static const char* contentLengthHeader;  ///< The content length header text
    static const char* contentTypeHeader;    ///< The content type header text
    /**@}*/

    /**
     * @anchor envirodiy_json_vars
     * @name Portions of the JSON object for EnviroDIY
     *
     * @{
     */
    static const char* samplingFeatureTag;  ///< The JSON feature UUID tag
    static const char* timestampTag;        ///< The JSON feature timestamp tag
                                            /**@}*/

    // queued sensor data buffer
    static char dataBuffer[MS_DATA_BUFFER_SIZE];

 private:
    // Tokens and UUID's for EnviroDIY
    const char* _registrationToken = nullptr;
};

#endif  // SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

/**
 * @file EnviroDIYPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
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
        return String(enviroDIYHost) + String(enviroDIYPath);
    }

    /**
     * @brief Get the EnviroDIY/Monitor My Watershed web host
     *
     * @return The EnviroDIY/Monitor My Watershed web host
     */
    String getHost(void);

    /**
     * @brief Set the EnviroDIY/Monitor My Watershed web host
     *
     * @param host The EnviroDIY/Monitor My Watershed web host
     */
    void setHost(const char* host);

    /**
     * @brief Get the EnviroDIY/Monitor My Watershed API path
     *
     * @return The EnviroDIY/Monitor My Watershed API path
     */
    String getPath(void);
    /**
     * @brief Set the EnviroDIY/Monitor My Watershed API path
     *
     * @param endpoint The EnviroDIY/Monitor My Watershed API path
     */
    void setPath(const char* endpoint);

    /**
     * @brief Get the EnviroDIY/Monitor My Watershed API port
     *
     * @return The EnviroDIY/Monitor My Watershed API port
     */
    int getPort(void);
    /**
     * @brief Set the EnviroDIY/Monitor My Watershed API port
     *
     * @param port The EnviroDIY/Monitor My Watershed API port
     */
    void setPort(int port);

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
     * @return The number of characters in the JSON object.
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
     * @return The http status code of the response.
     */
    int16_t publishData(Client* outClient) override;

 protected:
    /**
     * @anchor envirodiy_post_vars
     * @name Portions of the POST request to EnviroDIY
     *
     * @{
     */
    const char*        enviroDIYPath;        ///< The api path
    const char*        enviroDIYHost;        ///< The host name
    int                enviroDIYPort;        ///< The host port
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

 private:
    /**
     * @brief Internal reference to the EnviroDIY/Monitor My Watershed
     * registration token.
     */
    const char* _registrationToken = nullptr;
};

#endif  // SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

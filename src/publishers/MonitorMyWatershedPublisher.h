/**
 * @file MonitorMyWatershedPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Thomas Watson <twatson52@icloud.com>
 *
 * @brief Contains the MonitorMyWatershedPublisher subclass of dataPublisher for
 * publishing data to Monitor My Watershed at https://monitormywatershed.org/
 */

// Header Guards
#ifndef SRC_PUBLISHERS_MONITORMYWATERSHEDPUBLISHER_H_
#define SRC_PUBLISHERS_MONITORMYWATERSHEDPUBLISHER_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_MONITORMYWATERSHEDPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "MonitorMyWatershedPublisher"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "dataPublisherBase.h"
#include "LogBuffer.h"


// ============================================================================
//  Functions for Monitor My Watershed
// ============================================================================
/**
 * @brief The MonitorMyWatershedPublisher subclass of dataPublisher for
 * publishing data to Monitor My Watershed at https://monitormywatershed.org/
 * (formerly at http://data.enviroDIY.org).
 *
 * @ingroup the_publishers
 */
class MonitorMyWatershedPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new Monitor My Watershed Publisher object with only
     * default values for the host, path, and port set..
     */
    MonitorMyWatershedPublisher();
    /**
     * @brief Construct a new Monitor My Watershed Publisher object
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions.
     */
    explicit MonitorMyWatershedPublisher(Logger& baseLogger,
                                         int     sendEveryX = 1);
    /**
     * @brief Construct a new Monitor My Watershed Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions.
     */
    MonitorMyWatershedPublisher(Logger& baseLogger, Client* inClient,
                                int sendEveryX = 1);
    /**
     * @brief Construct a new Monitor My Watershed Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     * @param samplingFeatureUUID The sampling feature UUID for the site on
     * Monitor My Watershed.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions.
     */
    MonitorMyWatershedPublisher(Logger&     baseLogger,
                                const char* registrationToken,
                                const char* samplingFeatureUUID,
                                int         sendEveryX = 1);
    /**
     * @brief Construct a new Monitor My Watershed Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions.
     */
    MonitorMyWatershedPublisher(Logger&     baseLogger,
                                const char* registrationToken,
                                int         sendEveryX = 1);
    /**
     * @brief Construct a new Monitor My Watershed Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     * @param samplingFeatureUUID The sampling feature UUID for the site on
     * Monitor My Watershed.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions.
     */
    MonitorMyWatershedPublisher(Logger& baseLogger, Client* inClient,
                                const char* registrationToken,
                                const char* samplingFeatureUUID,
                                int         sendEveryX = 1);
    /**
     * @brief Construct a new Monitor My Watershed Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions.
     */
    MonitorMyWatershedPublisher(Logger& baseLogger, Client* inClient,
                                const char* registrationToken,
                                int         sendEveryX = 1);
    /**
     * @brief Destroy the Monitor My Watershed Publisher object
     */
    virtual ~MonitorMyWatershedPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(monitorMWHost) + String(monitorMWPath);
    }

    /**
     * @brief Get the Monitor My Watershed web host
     *
     * @return The Monitor My Watershed web host
     */
    String getHost(void);

    /**
     * @brief Set the Monitor My Watershed web host
     *
     * @param host The Monitor My Watershed web host
     */
    void setHost(const char* host);

    /**
     * @brief Get the Monitor My Watershed API path
     *
     * @return The Monitor My Watershed API path
     */
    String getPath(void);
    /**
     * @brief Set the Monitor My Watershed API path
     *
     * @param endpoint The Monitor My Watershed API path
     */
    void setPath(const char* endpoint);

    /**
     * @brief Get the Monitor My Watershed API port
     *
     * @return The Monitor My Watershed API port
     */
    int getPort(void);
    /**
     * @brief Set the Monitor My Watershed API port
     *
     * @param port The Monitor My Watershed API port
     */
    void setPort(int port);

    // Adds the site registration token
    /**
     * @brief Set the site registration token
     *
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     */
    void setToken(const char* registrationToken);

    /**
     * @brief Calculates how long the outgoing JSON will be
     *
     * @return The number of characters in the JSON object.
     */
    uint16_t calculateJsonSize();


    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     * @param samplingFeatureUUID The sampling feature UUID for the site on
     * Monitor My Watershed.
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* registrationToken, const char* samplingFeatureUUID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     * @param samplingFeatureUUID The sampling feature UUID for the site on
     * Monitor My Watershed.
     */
    void begin(Logger& baseLogger, const char* registrationToken,
               const char* samplingFeatureUUID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param registrationToken The registration token for the site on Monitor
     * My Watershed.
     */
    void begin(Logger& baseLogger, const char* registrationToken);

    /**
     * @brief Checks if the publisher needs an Internet connection for the next
     * publishData call (as opposed to just buffering data internally).
     *
     * @return True if an internet connection is needed for the next publish.
     */
    bool connectionNeeded(void) override;

    /**
     * @brief Utilize an attached modem to open a a TCP connection to Monitor My
     * Watershed and then stream out a post request over that connection.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     * @return The http status code of the response.
     */
    int16_t publishData(Client* outClient,
                        bool forceFlush = MS_ALWAYS_FLUSH_PUBLISHERS) override;

 protected:
    /**
     * @anchor monitormw_post_vars
     * @name Portions of the POST request to Monitor My Watershed
     *
     * @{
     */
    const char*        monitorMWPath;        ///< The api path
    const char*        monitorMWHost;        ///< The host name
    int                monitorMWPort;        ///< The host port
    static const char* tokenHeader;          ///< The token header text
    static const char* contentLengthHeader;  ///< The content length header text
    static const char* contentTypeHeader;    ///< The content type header text
    /**@}*/

    /**
     * @anchor monitormw_json_vars
     * @name Portions of the JSON object for Monitor My Watershed
     *
     * @{
     */
    static const char* samplingFeatureTag;  ///< The JSON feature UUID tag
    static const char* timestampTag;        ///< The JSON feature timestamp tag

    /**@}*/


    LogBuffer _logBuffer;  ///< Internal reference to the logger buffer

    // actually transmit rather than just buffer data
    /**
     * @brief Transmit data from the data buffer to an external site
     *
     * @param outClient The client to publish the data over
     * @return The HTTP response code from the publish attempt
     *
     * @note A 504 will be returned automatically if the server does not
     * respond within 30 seconds.
     */
    int16_t flushDataBuffer(Client* outClient);

    /**
     * @brief The number of transmissions remaining at the single minute
     * intervals
     *
     * We send every one of the first five data points at only one minute
     * intervals for faster in-field validation.
     *
     * @todo This should be a user-settable parameter.
     */
    uint8_t _initialTransmissionsRemaining = 5;

 private:
    /**
     * @brief Internal reference to the Monitor My Watershed registration token.
     */
    const char* _registrationToken = nullptr;
};

#endif  // SRC_PUBLISHERS_MONITORMYWATERSHEDPUBLISHER_H_

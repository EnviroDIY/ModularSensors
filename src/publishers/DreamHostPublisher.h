/**
 * @file DreamHostPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DreamHostPublisher subclass of dataPublisher for
 * publishing data to the Stroud Center's now-deprecated DreamHost based live
 * sensor data system.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_DREAMHOSTPUBLISHER_H_
#define SRC_PUBLISHERS_DREAMHOSTPUBLISHER_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_DREAMHOSTPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "DreamHostPublisher"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================
/**
 * @brief The DreamHostPublisher subclass of dataPublisher is for publishing
 * data to the Stroud Center's now-deprecated DreamHost based live sensor data
 * system.
 *
 * @ingroup the_publishers
 */
class DreamHostPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new DreamHost Publisher object with no members set.
     */
    DreamHostPublisher();
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    explicit DreamHostPublisher(Logger& baseLogger, int sendEveryX = 1);
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    DreamHostPublisher(Logger& baseLogger, Client* inClient,
                       int sendEveryX = 1);
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param dhUrl The URL for sending data to DreamHost
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    DreamHostPublisher(Logger& baseLogger, const char* dhUrl,
                       int sendEveryX = 1);
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param dhUrl The URL for sending data to DreamHost
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    DreamHostPublisher(Logger& baseLogger, Client* inClient, const char* dhUrl,
                       int sendEveryX = 1);
    /**
     * @brief Destroy the DreamHost Publisher object
     */
    virtual ~DreamHostPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(dreamhostHost);
    }

    // Functions for private SWRC server
    /**
     * @brief Set the url of the DreamHost data receiver
     *
     * @param dhUrl The URL for sending data to DreamHost
     */
    void setDreamHostPortalRX(const char* dhUrl);

    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param dhUrl The URL for sending data to DreamHost
     */
    void begin(Logger& baseLogger, Client* inClient, const char* dhUrl);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param dhUrl The URL for sending data to DreamHost
     */
    void begin(Logger& baseLogger, const char* dhUrl);

    /**
     * @brief Utilizes an attached modem to make a TCP connection to the
     * DreamHost URL and then stream out a get request over that connection.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     *
     * @return The http status code of the response.
     */
    int16_t publishData(Client* outClient,
                        bool forceFlush = MS_ALWAYS_FLUSH_PUBLISHERS) override;

 protected:
    // portions of the GET request
    /**
     * @anchor dreamhost_protected_vars
     * @name Portions of the GET request to DreamHost
     *
     * @{
     */
    static const char* dreamhostHost;   ///< The host name
    static const int   dreamhostPort;   ///< The host port
    static const char* loggerTag;       ///< The Stroud logger number
    static const char* timestampTagDH;  ///< The timestamp
                                        /**@}*/


 private:
    /**
     * @brief A pointer to the base URL for the dreamhost portal.
     */
    const char* _DreamHostPortalRX = nullptr;
};

#endif  // SRC_PUBLISHERS_DREAMHOSTPUBLISHER_H_

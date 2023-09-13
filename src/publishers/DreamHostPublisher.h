/**
 * @file DreamHostPublisher.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DreamHostPublisher subclass of dataPublisher for
 * publishing data to the Stroud Center's now-deprecated DreamHost based live
 * sensor data system.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_DREAMHOSTPUBLISHER_H_
#define SRC_PUBLISHERS_DREAMHOSTPUBLISHER_H_

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
     */
    explicit DreamHostPublisher(Logger& baseLogger);
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     */
    DreamHostPublisher(Logger& baseLogger, Client* inClient);
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param dhUrl The URL for sending data to DreamHost
     */
    DreamHostPublisher(Logger& baseLogger, const char* dhUrl);
    /**
     * @brief Construct a new DreamHost Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param dhUrl The URL for sending data to DreamHost
     */
    DreamHostPublisher(Logger& baseLogger, Client* inClient, const char* dhUrl);
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
     * @brief This creates all of the URL parameter tags and values and writes
     * the result to an Arduino stream.
     *
     * HTML headers are not included.
     *
     * @param stream The Arduino stream to write out the URL and parameters to.
     */
    void printSensorDataDreamHost(Stream* stream);

    /**
     * @brief This prints a fully structured GET request for DreamHost to the
     * specified stream.
     *
     * This includes the HTML headers.
     *
     * @param stream The Arduino stream to write out the URL and parameters to.
     */
    void printDreamHostRequest(Stream* stream);

    // A way to begin with everything already set
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

    // int16_t postDataDreamHost(void);
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
     *
     * @return **int16_t** The http status code of the response.
     */
    int16_t publishData(Client* outClient) override;

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
    const char* _DreamHostPortalRX = nullptr;
    bool        _dualPost          = true;
};

#endif  // SRC_PUBLISHERS_DREAMHOSTPUBLISHER_H_

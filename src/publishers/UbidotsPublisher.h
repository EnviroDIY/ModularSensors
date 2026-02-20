/**
 * @file UbidotsPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Greg Cutrell <gcutrell@limno.com>
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the UbidotsPublisher subclass of dataPublisher for
 * publishing data to the Ubidots IoT platform at https://ubidots.com
 */

// Header Guards
#ifndef SRC_PUBLISHERS_UBIDOTSPUBLISHER_H_
#define SRC_PUBLISHERS_UBIDOTSPUBLISHER_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_UBIDOTSPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "UbidotsPublisher"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for Ubidots
// ============================================================================
/**
 * @brief The UbidotsPublisher subclass of dataPublisher for publishing data
 * to the Ubidots data portal at https://ubidots.com
 *
 * @ingroup the_publishers
 */
class UbidotsPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new Ubidots Publisher object with no members set.
     */
    UbidotsPublisher();
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    explicit UbidotsPublisher(Logger& baseLogger, int sendEveryX = 1);
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    UbidotsPublisher(Logger& baseLogger, Client* inClient, int sendEveryX = 1);
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param authenticationToken The authentication token from Ubidots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    UbidotsPublisher(Logger& baseLogger, const char* authenticationToken,
                     const char* deviceID, int sendEveryX = 1);
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param authenticationToken The authentication token from Ubidots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    UbidotsPublisher(Logger& baseLogger, Client* inClient,
                     const char* authenticationToken, const char* deviceID,
                     int sendEveryX = 1);
    /**
     * @brief Destroy the Ubidots Publisher object
     */
    virtual ~UbidotsPublisher();

    // Returns the data destination
    /**
     * @brief Get the destination for published data - generally the host name
     * of the data receiver.
     *
     * @return The URL or HOST to receive published data
     */
    String getEndpoint(void) override {
        return String(ubidotsHost);
    }

    // Adds the device authentication token
    /**
     * @brief Set the device authentication token
     *
     * @param authenticationToken The authentication token from Ubidots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     */
    void setToken(const char* authenticationToken);

    /**
     * @brief Calculates how long the outgoing JSON will be
     *
     * @return The number of characters in the JSON object.
     */
    uint16_t calculateJsonSize();

    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param authenticationToken The authentication token from Ubidots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* authenticationToken, const char* deviceID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param authenticationToken The authentication token from Ubidots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     */
    void begin(Logger& baseLogger, const char* authenticationToken,
               const char* deviceID);

    // Post Data to Ubidots
    /**
     * @brief Utilize an attached modem to open a TCP connection to the
     * Ubidots API and then stream out a post request over
     * that connection.
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
    int16_t publishData(Client* outClient, bool forceFlush) override;

 protected:
    /**
     * @anchor ubidots_post_vars
     * @name Portions of the POST request to Ubidots
     *
     * @{
     */
    static const char* postEndpoint;         ///< The endpoint
    static const char* ubidotsHost;          ///< The host name
    static const int   ubidotsPort;          ///< The host port
    static const char* tokenHeader;          ///< The token header text
    static const char* contentLengthHeader;  ///< The content length header text
    static const char* contentTypeHeader;    ///< The content type header text
    /**@}*/

    /**
     * @anchor ubidots_json_vars
     * @name The begining of the JSON object for Ubidots
     *
     * @{
     */
    static const char* payload;  ///< The JSON initial characters
                                 /**@}*/

 private:
    // Tokens for Ubidots
    /**
     * @brief The authentication token from Ubidots, either the Organization's
     * Integration Token (under Users > Organization menu, visible by Admin
     * only) OR the STEM User's Device Token (under the specific device's setup
     * panel).
     */
    const char* _authenticationToken = nullptr;
};

#endif  // SRC_PUBLISHERS_UBIDOTSPUBLISHER_H_

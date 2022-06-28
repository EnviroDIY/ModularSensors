/**
 * @file UbidotsPublisher.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Greg Cutrell <gcutrell@limno.com>
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the UbidotsPublisher subclass of dataPublisher for
 * publishing data to the Ubidots IoT platform at https://ubidots.com
 */

// Header Guards
#ifndef SRC_PUBLISHERS_UBIDOTSPUBLISHER_H_
#define SRC_PUBLISHERS_UBIDOTSPUBLISHER_H_

// Debugging Statement
// #define UBIDOTSPUBLISHER_DEBUG

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
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     *
     * @note It is possible (though very unlikey) that using this constructor
     * could cause errors if the compiler attempts to initialize the publisher
     * instance before the logger instance.  If you suspect you are seeing that
     * issue, use the null constructor and a populated begin(...) within your
     * set-up function.
     */
    explicit UbidotsPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                              uint8_t sendOffset = 0);
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     *
     * @note It is possible (though very unlikey) that using this constructor
     * could cause errors if the compiler attempts to initialize the publisher
     * instance before the logger instance.  If you suspect you are seeing that
     * issue, use the null constructor and a populated begin(...) within your
     * set-up function.
     */
    UbidotsPublisher(Logger& baseLogger, Client* inClient,
                     uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param authentificationToken The authentication token from Ubdots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    UbidotsPublisher(Logger& baseLogger, const char* authentificationToken,
                     const char* deviceID, uint8_t sendEveryX = 1,
                     uint8_t sendOffset = 0);
    /**
     * @brief Construct a new Ubidots Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param authentificationToken The authentication token from Ubdots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    UbidotsPublisher(Logger& baseLogger, Client* inClient,
                     const char* authentificationToken, const char* deviceID,
                     uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Destroy the EnviroDIY Publisher object
     */
    virtual ~UbidotsPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(ubidotsHost);
    }

    // Adds the device authentication token
    /**
     * @brief Set the device authentication token
     *
     * @param authentificationToken The authentication token from Ubdots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     */
    void setToken(const char* authentificationToken);

    /**
     * @brief Calculates how long the outgoing JSON will be
     *
     * @return uint16_t The number of characters in the JSON object.
     */
    uint16_t calculateJsonSize();
    // /**
    //  * @brief Calculates how long the full post request will be, including
    //  * headers
    //  *
    //  * @return uint16_t The length of the full request including HTTP
    //  headers.
    //  */
    // uint16_t calculatePostSize();

    /**
     * @brief This generates a properly formatted JSON for Ubidots and prints
     * it to the input Arduino stream object.
     *
     * @param stream The Arduino stream to write out the JSON to.
     */
    void printSensorDataJSON(Stream* stream);

    /**
     * @brief This prints a fully structured post request for the Ubidots API
     * to the specified stream.
     *
     * @param stream The Arduino stream to write out the request to.
     */
    void printUbidotsRequest(Stream* stream);

    // A way to begin with everything already set
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param authentificationToken The authentication token from Ubdots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* authentificationToken, const char* deviceID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param authentificationToken The authentication token from Ubdots, either
     * the Organization's Integration Token (under Users > Organization menu,
     * visible by Admin only) OR the STEM User's Device Token (under the
     * specific device's setup panel).
     * @param deviceID The device API Label from Ubidots, derived from the
     * user-specified device name.
     */
    void begin(Logger& baseLogger, const char* authentificationToken,
               const char* deviceID);

    // Post Data to Ubidots
    /**
     * @brief Utilize an attached modem to open a a TCP connection to the
     * Ubidots API and then stream out a post request over
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
     * @anchor ubidots_post_vars
     * @name Portions of the POST request to Ubidots
     *
     * @{
     */
    static const char* postEndpoint;  ///< The endpoint
    static const char* ubidotsHost;   ///< The host name
    static const int   ubidotsPort;   ///< The host port
    static const char* tokenHeader;   ///< The token header text
    // static const char *cacheHeader;  ///< The cache header text
    // static const char *connectionHeader;  ///< The keep alive header text
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
    const char* _authentificationToken = nullptr;
};

#endif  // SRC_PUBLISHERS_UBIDOTSPUBLISHER_H_

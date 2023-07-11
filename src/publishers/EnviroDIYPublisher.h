/**
 * @file EnviroDIYPublisher.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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

// #define MS_ENVIRODIYPUBLISHER_DEBUG_DEEP
#ifdef MS_ENVIRODIYPUBLISHER_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "EnviroDIYPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#undef MS_ENVIRODIYPUBLISHER_DEBUG_DEEP
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
    explicit EnviroDIYPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                                uint8_t sendOffset = 0);
    /**
     * @brief Construct a new EnviroDIY Publisher object
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
    EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                       uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param registrationToken The registration token for the site on the
     * Monitor My Watershed data portal.
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    EnviroDIYPublisher(Logger& baseLogger, const char* registrationToken,
                       const char* samplingFeatureUUID, uint8_t sendEveryX = 1,
                       uint8_t sendOffset = 0);
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
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    EnviroDIYPublisher(Logger& baseLogger, Client* inClient,
                       const char* registrationToken,
                       const char* samplingFeatureUUID, uint8_t sendEveryX = 1,
                       uint8_t sendOffset = 0);
    /**
     * @brief Destroy the EnviroDIY Publisher object
     */
    virtual ~EnviroDIYPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(_enviroDIYHost)+':'+String(enviroDIYPort);
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
    // /**
    //  * @brief Calculates how long the full post request will be, including
    //  * headers
    //  *
    //  * @return uint16_t The length of the full request including HTTP
    //  headers.
    //  */
    // uint16_t calculatePostSize();

    /**
     * @brief This generates a properly formatted JSON for EnviroDIY and prints
     * it to the input Arduino stream object.
     *
     * @param stream The Arduino stream to write out the JSON to.
     */
    void printSensorDataJSON(Stream* stream);

    /**
     * @brief This prints a fully structured post request for Monitor My
     * Watershed/EnviroDIY to the specified stream.
     *
     * @param stream The Arduino stream to write out the request to.
     */
    void printEnviroDIYRequest(Stream* stream);

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

    // int16_t postDataEnviroDIY(void);
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
    static const char* postEndpoint;   ///< The endpoint
    static const char* enviroDIYHost;  ///< The host name
    static const int   enviroDIYPort;  ///< The host port
    static const char* tokenHeader;    ///< The token header text
    // static const char *cacheHeader;  ///< The cache header text
    // static const char *connectionHeader;  ///< The keep alive header text
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
    // Tokens and UUID's for EnviroDIY
    const char* _registrationToken = nullptr;
    const char* _enviroDIYHost     = enviroDIYHost;
    //FUT: int   _enviroDIYPort;

 public:
     /**
     * @brief Set the destination Host URL
     *
     * @param enviroDIYHost The Host URL for the site on the
     * Monitor My Watershed data portal.
     */
    void setDIYHost(const char* enviroDIYHost);

    /* FUT:
     * @brief Set the destination Port
     *
     * @param enviroDIYPort The Port on Host URL for the site on the
     * Monitor My Watershed data portal.
     */
    //void setDIYPort(const int enviroDIYPort);

 protected:
    /**
     * @brief This constructs a POST header for MMW
     *
     * @param tempBuffer - place for the POST.
     */
    void mmwPostHeader(char* tempBuffer);

    /**
     * @brief This constructs a POST body EnviroDIY
     *
     * @param tempBuffer - place for the POST.
     */
    void mmwPostDataArray(char* tempBuffer);
    void mmwPostDataQueued(char* tempBuffer);

 public:
    /**
     * @brief This routes subsequent POST construction
     *
     * @param state - true for Queued, false for standard
     */
    bool setQueuedState(bool state, char uniqueId = '0') override {
        MS_DBG(F("EnvrDIYPub setQueued "), state);
        return useQueueDataSource = state;
    }
    bool getQueuedStatus() override {
        MS_DBG(F("EnvrDIYPub gQS "), useQueueDataSource);
        return useQueueDataSource;
    }

#if !defined TIMER_EDP_POST_TIMEOUT_DEF_MSEC
#define TIMER_EDP_POST_TIMEOUT_DEF_MSEC 10000L
#endif  // TIMER_EDP_POST_TIMEOUT_DEF_MSEC
    uint16_t _timerPostTimeout_ms = TIMER_EDP_POST_TIMEOUT_DEF_MSEC;
    uint16_t virtual setTimerPostTimeout_mS(uint16_t tpt_ms) {
        MS_DBG(F("setTPT(mS)"), tpt_ms);
        return _timerPostTimeout_ms = tpt_ms;  // Default for not supported.
    }

    uint16_t getTimerPostTimeout_mS() {
        MS_DBG(F("getTPT(mS)"), _timerPostTimeout_ms);
        return _timerPostTimeout_ms;  // Default for not supported.
    }

#if !defined TIMER_EDP_POSTED_PACING_DEF_MSEC
#define TIMER_EDP_POSTED_PACING_DEF_MSEC 2000L
#endif  // TIMER_EDP_POSTED_PACING_DEF_MSEC
    uint16_t _timerPostPacing_ms = TIMER_EDP_POSTED_PACING_DEF_MSEC;
    uint16_t virtual setTimerPostPacing_mS(uint16_t tpp_ms) {
        MS_DBG(F("setTPP(mS)"), tpp_ms);
        return _timerPostPacing_ms = tpp_ms;
    }

    uint16_t getTimerPostPacing_mS() {
        MS_DBG(F("getTPP(mS)"), _timerPostPacing_ms);
        return _timerPostPacing_ms;
    }    

};

#endif  // SRC_PUBLISHERS_ENVIRODIYPUBLISHER_H_

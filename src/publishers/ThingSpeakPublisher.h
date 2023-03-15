/**
 * @file ThingSpeakPublisher.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ThingSpeakPublisher subclass of dataPublisher for
 * publishing data to ThingSpeak using the MQTT protocol.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_
#define SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_

// Debugging Statement
// #define MS_THINGSPEAKPUBLISHER_DEBUG

#ifdef MS_THINGSPEAKPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "ThingSpeakPublisher"
#endif

/**
 * @brief The MQTT User Name
 *
 * @note The user name doesn't actually mean anything for ThingSpeak
 */
#define THING_SPEAK_USER_NAME "MS"

/**
 * @brief The MQTT Client Name
 *
 * @note The client name doesn't actually mean anything for ThingSpeak
 */
#define THING_SPEAK_CLIENT_NAME "MS"

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"
#include "PubSubClient.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
/**
 * @brief The ThingSpeakPublisher subclass of dataPublisher for publishing data
 * to ThingSpeak using the MQTT protocol.
 *
 * When sending data to ThingSpeak the order of the variables in the variable
 * array attached to your logger is __crucial__.  The results from the variables
 * in the VariableArray will be sent to ThingSpeak in the order they are in the
 * array; that is, the first variable in the array will be sent as Field1, the
 * second as Field2, etc.  Any UUID's or custom variable codes are ignored for
 * ThingSpeak.  They will only appear in the header of your file on the SD card.
 * Giving a variable a custom variable code like "Field3" will **NOT** make that
 * variable field 3 on ThingSpeak.  The third variable in the array will always
 * be "Field3".  Any text names you have given to your fields in ThingSpeak are
 * also irrelevant.
 *
 * @ingroup the_publishers
 */
class ThingSpeakPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new ThingSpeak Publisher object with no members
     * initialized.
     */
    ThingSpeakPublisher();
    /**
     * @brief Construct a new ThingSpeak Publisher object
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
    explicit ThingSpeakPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                                 uint8_t sendOffset = 0);
    /**
     * @brief Construct a new ThingSpeak Publisher object
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
    ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                        uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Construct a new ThingSpeak Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param thingSpeakMQTTKey Your MQTT API Key from Account > MyProfile.
     * @param thingSpeakChannelID The numeric channel id for your channel
     * @param thingSpeakChannelKey The write API key for your channel
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     */
    ThingSpeakPublisher(Logger& baseLogger, const char* thingSpeakMQTTKey,
                        const char* thingSpeakChannelID,
                        const char* thingSpeakChannelKey,
                        uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Construct a new ThingSpeak Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param thingSpeakMQTTKey Your MQTT API Key from Account > MyProfile.
     * @param thingSpeakChannelID The numeric channel id for your channel
     * @param thingSpeakChannelKey The write API key for your channel
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                        const char* thingSpeakMQTTKey,
                        const char* thingSpeakChannelID,
                        const char* thingSpeakChannelKey,
                        uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Destroy the ThingSpeak Publisher object
     */
    virtual ~ThingSpeakPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(mqttServer);
    }

    /**
     * @brief Set the MQTT API Key from Account > MyProfile
     *
     * @param thingSpeakMQTTKey Your MQTT API Key from Account > MyProfile.
     */
    void setMQTTKey(const char* thingSpeakMQTTKey);

    /**
     * @brief Set the ThingSpeak channel ID
     *
     * @param thingSpeakChannelID The numeric channel id for your channel
     */
    void setChannelID(const char* thingSpeakChannelID);

    /**
     * @brief Set the channel's write API key.
     *
     * @param thingSpeakChannelKey The write API key for your channel
     */
    void setChannelKey(const char* thingSpeakChannelKey);

    /**
     * @brief Sets all 3 ThingSpeak parameters
     *
     * @param MQTTKey Your MQTT API Key from Account > MyProfile.
     * @param channelID The numeric channel id for your channel
     * @param channelKey The write API key for your channel
     */
    void setThingSpeakParams(const char* MQTTKey, const char* channelID,
                             const char* channelKey);

    // A way to begin with everything already set
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param thingSpeakMQTTKey Your MQTT API Key from Account > MyProfile.
     * @param thingSpeakChannelID The numeric channel id for your channel
     * @param thingSpeakChannelKey The write API key for your channel
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* thingSpeakMQTTKey, const char* thingSpeakChannelID,
               const char* thingSpeakChannelKey);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param thingSpeakMQTTKey Your MQTT API Key from Account > MyProfile.
     * @param thingSpeakChannelID The numeric channel id for your channel
     * @param thingSpeakChannelKey The write API key for your channel
     */
    void begin(Logger& baseLogger, const char* thingSpeakMQTTKey,
               const char* thingSpeakChannelID,
               const char* thingSpeakChannelKey);

    // This sends the data to ThingSpeak
    // bool mqttThingSpeak(void);
    int16_t publishData(Client* outClient) override;

 protected:
    /**
     * @anchor ts_mqqt_vars
     * @name Portions of the MQTT data publication
     *
     * @{
     */
    static const char* mqttServer;      ///< The MQTT server
    static const int   mqttPort;        ///< The MQTT port
    static const char* mqttClientName;  ///< The MQTT client name
    static const char* mqttUser;        ///< The MQTT user name
                                        /**@}*/

 private:
    // Keys for ThingSpeak
    const char*  _thingSpeakMQTTKey    = nullptr;
    const char*  _thingSpeakChannelID  = nullptr;
    const char*  _thingSpeakChannelKey = nullptr;
    PubSubClient _mqttClient;
};

#endif  // SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_

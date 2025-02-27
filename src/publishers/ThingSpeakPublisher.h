/**
 * @file ThingSpeakPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ThingSpeakPublisher subclass of dataPublisher for
 * publishing data to ThingSpeak using the MQTT protocol.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_
#define SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_

// Include config before anything else
#include "ModSensorConfig.h"

// Debugging Statement
// #define MS_THINGSPEAKPUBLISHER_DEBUG

#ifdef MS_THINGSPEAKPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "ThingSpeakPublisher"
#endif

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
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    explicit ThingSpeakPublisher(Logger& baseLogger, int sendEveryX = 1);
    /**
     * @brief Construct a new ThingSpeak Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                        int sendEveryX = 1);
    /**
     * @brief Construct a new ThingSpeak Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param thingSpeakClientName The client name for your MQTT device. This is
     * probably the same as your MQTT device's user name.
     * @param thingSpeakMQTTUser The user name for your MQTT device. This is
     * probably the same as your MQTT device's client name.
     * @param thingSpeakMQTTPassword The password for your MQTT device.
     * @param thingSpeakChannelID The numeric channel id for your channel.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    ThingSpeakPublisher(Logger& baseLogger, const char* thingSpeakClientName,
                        const char* thingSpeakMQTTUser,
                        const char* thingSpeakMQTTPassword,
                        const char* thingSpeakChannelID, int sendEveryX = 1);
    /**
     * @brief Construct a new ThingSpeak Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param thingSpeakClientName The client name for your MQTT device. This is
     * probably the same as your MQTT device's user name.
     * @param thingSpeakMQTTUser The user name for your MQTT device. This is
     * probably the same as your MQTT device's client name.
     * @param thingSpeakMQTTPassword The password for your MQTT device.
     * @param thingSpeakChannelID The numeric channel id for your channel.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                        const char* thingSpeakClientName,
                        const char* thingSpeakMQTTUser,
                        const char* thingSpeakMQTTPassword,
                        const char* thingSpeakChannelID, int sendEveryX = 1);
    /**
     * @brief Destroy the ThingSpeak Publisher object
     */
    virtual ~ThingSpeakPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(mqttServer);
    }

    /**
     * @brief The client name for your MQTT device. This is probably the same as
     * your MQTT device's user name.
     *
     * @param thingSpeakClientName The client name for your MQTT device. This is
     * probably the same as your MQTT device's user name.
     */
    void setMQTTClient(const char* thingSpeakClientName);

    /**
     * @brief Set the user name for your MQTT device.
     *
     * @param thingSpeakMQTTUser The user name for your MQTT device. This is
     * probably the same as your MQTT device's client name.
     */
    void setUserName(const char* thingSpeakMQTTUser);

    /**
     * @brief Set the password for your MQTT device.
     *
     * @param thingSpeakMQTTPassword The password for your MQTT device.
     */
    void setPassword(const char* thingSpeakMQTTPassword);

    /**
     * @brief Set the ThingSpeak channel ID
     *
     * @param thingSpeakChannelID The numeric channel id for your channel
     */
    void setChannelID(const char* thingSpeakChannelID);

    /**
     * @brief Sets all 4 ThingSpeak parameters
     *
     * @param thingSpeakClientName The client name for your MQTT device. This is
     * probably the same as your MQTT device's user name.
     * @param thingSpeakMQTTUser The user name for your MQTT device. This is
     * probably the same as your MQTT device's client name.
     * @param thingSpeakMQTTPassword The password for your MQTT device.
     * @param thingSpeakChannelID The numeric channel id for your channel.
     */
    void setThingSpeakParams(const char* thingSpeakClientName,
                             const char* thingSpeakMQTTUser,
                             const char* thingSpeakMQTTPassword,
                             const char* thingSpeakChannelID);

    // A way to begin with everything already set
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param thingSpeakClientName The client name for your MQTT device. This is
     * probably the same as your MQTT device's user name.
     * @param thingSpeakMQTTUser The user name for your MQTT device. This is
     * probably the same as your MQTT device's client name.
     * @param thingSpeakMQTTPassword The password for your MQTT device.
     * @param thingSpeakChannelID The numeric channel id for your channel.
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* thingSpeakClientName, const char* thingSpeakMQTTUser,
               const char* thingSpeakMQTTPassword,
               const char* thingSpeakChannelID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param thingSpeakClientName The client name for your MQTT device. This is
     * probably the same as your MQTT device's user name.
     * @param thingSpeakMQTTUser The user name for your MQTT device. This is
     * probably the same as your MQTT device's client name.
     * @param thingSpeakMQTTPassword The password for your MQTT device.
     * @param thingSpeakChannelID The numeric channel id for your channel.
     */
    void begin(Logger& baseLogger, const char* thingSpeakClientName,
               const char* thingSpeakMQTTUser,
               const char* thingSpeakMQTTPassword,
               const char* thingSpeakChannelID);

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
    static const char* mqttServer;  ///< The MQTT server
    static const int   mqttPort;    ///< The MQTT port
                                    /**@}*/

 private:
    // Keys for ThingSpeak
    /**
     * @brief The client name for your MQTT device. This is probably the same as
     * your MQTT device's user name.
     */
    const char* _thingSpeakClientName = nullptr;
    /**
     * @brief The user name for your MQTT device. This is probably the same as
     * your MQTT device's client name.
     */
    const char* _thingSpeakMQTTUser = nullptr;
    /**
     * @brief The password for your MQTT device
     */
    const char* _thingSpeakMQTTPassword = nullptr;
    /**
     * @brief The channel ID for ThingSpeak
     */
    const char* _thingSpeakChannelID = nullptr;
    /**
     * @brief Internal reference to the PubSubClient instance for MQTT
     * communication.
     */
    PubSubClient _mqttClient;
};

#endif  // SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_

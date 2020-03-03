/*
 *ThingSpeakPublisher.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the ThingSpeak logging functions - ie, sending MQTT data
 */

// Header Guards
#ifndef SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_
#define SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_

// Debugging Statement
// #define MS_THINGSPEAKPUBLISHER_DEBUG

#ifdef MS_THINGSPEAKPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "ThingSpeakPublisher"
#endif

// MQTT User Name
// The user name doesn't actually mean anything for ThingSpeak
#define THING_SPEAK_USER_NAME "MS"

// MQTT Client Name
// The client name doesn't actually mean anything for ThingSpeak
#define THING_SPEAK_CLIENT_NAME "MS"

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"
#include <PubSubClient.h>


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class ThingSpeakPublisher : public dataPublisher {
 public:
    // Constructors
    ThingSpeakPublisher();
    explicit ThingSpeakPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                                 uint8_t sendOffset = 0);
    ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                        uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    ThingSpeakPublisher(Logger& baseLogger, const char* thingSpeakMQTTKey,
                        const char* thingSpeakChannelID,
                        const char* thingSpeakChannelKey,
                        uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                        const char* thingSpeakMQTTKey,
                        const char* thingSpeakChannelID,
                        const char* thingSpeakChannelKey,
                        uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~ThingSpeakPublisher();

    // Returns the data destination
    virtual String getEndpoint(void) {
        return String(mqttServer);
    }

    // Adds the MQTT API Key from Account > MyProfile
    void setMQTTKey(const char* thingSpeakMQTTKey);

    // Adds the channel ID
    void setChannelID(const char* thingSpeakChannelID);

    // Adds the channel Write API Key.
    void setChannelKey(const char* thingSpeakChannelKey);

    // Sets all 3 ThingSpeak parameters
    void setThingSpeakParams(const char* MQTTKey, const char* channelID,
                             const char* channelKey);

    // A way to begin with everything already set
    void begin(Logger& baseLogger, Client* inClient,
               const char* thingSpeakMQTTKey, const char* thingSpeakChannelID,
               const char* thingSpeakChannelKey);
    void begin(Logger& baseLogger, const char* thingSpeakMQTTKey,
               const char* thingSpeakChannelID,
               const char* thingSpeakChannelKey);

    // This sends the data to ThingSpeak
    // bool mqttThingSpeak(void);
    virtual int16_t publishData(Client* _outClient);

 protected:
    static const char* mqttServer;
    static const int   mqttPort;
    static const char* mqttClientName;
    static const char* mqttUser;

 private:
    // Keys for ThingSpeak
    const char*  _thingSpeakMQTTKey;
    const char*  _thingSpeakChannelID;
    const char*  _thingSpeakChannelKey;
    PubSubClient _mqttClient;
};

#endif  // SRC_PUBLISHERS_THINGSPEAKPUBLISHER_H_

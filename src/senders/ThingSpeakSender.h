/*
 *ThingSpeakSender.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the ThingSpeak logging functions - ie, sending MQTT data
*/

// Header Guards
#ifndef ThingSpeakSender_h
#define ThingSpeakSender_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// MQTT User Name
// The user name doesn't actually mean anything for ThingSpeak
#define THING_SPEAK_USER_NAME "MS"

// MQTT Client Name
// The client name doesn't actually mean anything for ThingSpeak
#define THING_SPEAK_CLIENT_NAME "MS"

// Included Dependencies
#include "ModSensorDebugger.h"
#include "dataSenderBase.h"
#include <PubSubClient.h>


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class ThingSpeakSender : public dataSender
{
public:
    // Constructor
    ThingSpeakSender(Logger& baseLogger, Client& inClient,
                     uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    ThingSpeakSender(Logger& baseLogger, Client& inClient,
                     const char *thingSpeakMQTTKey,
                     const char *thingSpeakChannelID,
                     const char *thingSpeakChannelKey,
                     uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    virtual ~ThingSpeakSender();

    // Adds the MQTT API Key from Account > MyProfile
    void setMQTTKey(const char *thingSpeakMQTTKey);

    // Adds the channel ID
    void setChannelID(const char *thingSpeakChannelID);

    // Adds the channel Write API Key.
    void setChannelKey(const char *thingSpeakChannelKey);

    // Sets all 3 ThingSpeak parameters
    void setThingSpeakParams(const char *MQTTKey, const char *channelID,
                             const char *channelKey);

    // This sends the data to ThingSpeak
    // bool mqttThingSpeak(void);
    virtual int16_t sendData();

protected:
    static const char *mqttServer;
    static const char *mqttClient;
    static const char *mqttUser;

private:
    // Keys for ThingSpeak
    const char *_thingSpeakMQTTKey;
    const char *_thingSpeakChannelID;
    const char *_thingSpeakChannelKey;
    PubSubClient _mqttClient;
};

#endif  // Header Guard

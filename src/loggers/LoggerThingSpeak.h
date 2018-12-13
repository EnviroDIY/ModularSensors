/*
 *LoggerThingSpeak.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the ThingSpeak logging functions - ie, sending MQTT data
*/

// Header Guards
#ifndef LoggerThingSpeak_h
#define LoggerThingSpeak_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Send Buffer
#define MS_TS_SEND_BUFFER_SIZE 200

// MQTT User Name
// The user name doesn't actually mean anything for ThingSpeak
#define THING_SPEAK_USER_NAME "MS"

// MQTT Client Name
// The client name doesn't actually mean anything for ThingSpeak
#define THING_SPEAK_CLIENT_NAME "MS"

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerBase.h"
#include "LoggerModem.h"
#include <PubSubClient.h>


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class LoggerThingSpeak : public Logger
{
public:
    // Constructor
    LoggerThingSpeak(const char *loggerID, uint16_t loggingIntervalMinutes,
                    int8_t SDCardPin, int8_t mcuWakePin,
                    VariableArray *inputArray);
    virtual ~LoggerThingSpeak();

    // Adds a loggerModem objct to the logger
    // loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
    void attachModem(loggerModem& modem);

    // Takes advantage of the modem to synchronize the clock
    bool syncRTC();

    // Adds the MQTT API Key from Account > MyProfile
    void setMQTTKey(const char *thingSpeakMQTTKey);

    // Adds the channel ID
    void setChannelID(const char *thingSpeakChannelID);

    // Adds the channel Write API Key.
    void setChannelKey(const char *thingSpeakChannelKey);

    // Sets all 3 ThingSpeak parameters
    void setThingSpeakParams(const char *MQTTKey, const char *channelID,
                             const char *channelKey);

    // This fills the TX buffer with nulls ('\0')
    static void emptyMsgBuffer(void);

    // This sends the data to ThingSpeak
    bool mqttThingSpeak(void);

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //

    // This is a one-and-done to log data
    virtual void logDataAndSend(void);

    // The internal modem instance
    loggerModem *_logModem;
    // NOTE:  The internal _logModem must be a POINTER not a reference because
    // it is possible for no modem to be attached (and thus the pointer could
    // be null).  It is not possible to have a null reference.

protected:
    static char msgBuffer[MS_TS_SEND_BUFFER_SIZE];
    static int bufferFree(void);

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

/**
 * @file ThingSpeakPublisher.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ThingSpeakPublisher class.
 */

#include "ThingSpeakPublisher.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constant values for MQTT publish
// I want to refer to these more than once while ensuring there is only one copy
// in memory
const char* ThingSpeakPublisher::mqttServer     = "mqtt.thingspeak.com";
const int   ThingSpeakPublisher::mqttPort       = 1883;
const char* ThingSpeakPublisher::mqttClientName = THING_SPEAK_CLIENT_NAME;
const char* ThingSpeakPublisher::mqttUser       = THING_SPEAK_USER_NAME;


// Constructors
ThingSpeakPublisher::ThingSpeakPublisher() : dataPublisher() {}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                                         int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}
ThingSpeakPublisher::ThingSpeakPublisher(Logger&     baseLogger,
                                         const char* thingSpeakMQTTKey,
                                         const char* thingSpeakChannelID,
                                         const char* thingSpeakChannelKey,
                                         int         sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setMQTTKey(thingSpeakMQTTKey);
    setChannelID(thingSpeakChannelID);
    setChannelKey(thingSpeakChannelKey);
}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                                         const char* thingSpeakMQTTKey,
                                         const char* thingSpeakChannelID,
                                         const char* thingSpeakChannelKey,
                                         int         sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setMQTTKey(thingSpeakMQTTKey);
    setChannelID(thingSpeakChannelID);
    setChannelKey(thingSpeakChannelKey);
}
// Destructor
ThingSpeakPublisher::~ThingSpeakPublisher() {}


void ThingSpeakPublisher::setMQTTKey(const char* thingSpeakMQTTKey) {
    _thingSpeakMQTTKey = thingSpeakMQTTKey;
}


void ThingSpeakPublisher::setChannelID(const char* thingSpeakChannelID) {
    _thingSpeakChannelID = thingSpeakChannelID;
}


void ThingSpeakPublisher::setChannelKey(const char* thingSpeakChannelKey) {
    _thingSpeakChannelKey = thingSpeakChannelKey;
}

// Sets all 3 ThingSpeak parameters
void ThingSpeakPublisher::setThingSpeakParams(const char* MQTTKey,
                                              const char* channelID,
                                              const char* channelKey) {
    setMQTTKey(MQTTKey);
    setChannelID(channelID);
    setChannelKey(channelKey);
}


// A way to begin with everything already set
void ThingSpeakPublisher::begin(Logger& baseLogger, Client* inClient,
                                const char* thingSpeakMQTTKey,
                                const char* thingSpeakChannelID,
                                const char* thingSpeakChannelKey) {
    setMQTTKey(thingSpeakMQTTKey);
    setChannelID(thingSpeakChannelID);
    setChannelKey(thingSpeakChannelKey);
    dataPublisher::begin(baseLogger, inClient);
}
void ThingSpeakPublisher::begin(Logger&     baseLogger,
                                const char* thingSpeakMQTTKey,
                                const char* thingSpeakChannelID,
                                const char* thingSpeakChannelKey) {
    setMQTTKey(thingSpeakMQTTKey);
    setChannelID(thingSpeakChannelID);
    setChannelKey(thingSpeakChannelKey);
    dataPublisher::begin(baseLogger);
}


// This sends the data to ThingSpeak
// bool ThingSpeakPublisher::mqttThingSpeak(void)
int16_t ThingSpeakPublisher::publishData(Client* outClient, bool) {
    bool retVal = false;

    // Make sure we don't have too many fields
    // A channel can have a max of 8 fields
    if (_baseLogger->getArrayVarCount() > 8) {
        MS_DBG(F("No more than 8 fields of data can be sent to a single "
                 "ThingSpeak channel!"));
        MS_DBG(F("Only the first 8 fields worth of data will be sent."));
    }
    uint8_t numChannels = min(_baseLogger->getArrayVarCount(), 8);
    MS_DBG(numChannels, F("fields will be sent to ThingSpeak"));

    // Create a buffer for the portions of the request and response
    char tempBuffer[26] = "";

    char topicBuffer[42] = "channels/";
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s",
             _thingSpeakChannelID);
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s", "/publish/");
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s",
             _thingSpeakChannelKey);
    MS_DBG(F("Topic ["), strlen(topicBuffer), F("]:"), String(topicBuffer));

    // buffer is used only locally, it does not transmit
    txBufferInit(nullptr);

    txBufferAppend("created_at=");
    txBufferAppend(
        Logger::formatDateTime_ISO8601(Logger::markedLocalEpochTime).c_str());

    for (uint8_t i = 0; i < numChannels; i++) {
        txBufferAppend("&field");
        itoa(i + 1, tempBuffer, 10);  // BASE 10
        txBufferAppend(tempBuffer);
        txBufferAppend('=');
        txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
    }
    MS_DBG(F("Message ["), strlen(txBuffer), F("]:"), String(txBuffer));

    // Set the client connection parameters
    _mqttClient.setClient(*outClient);
    _mqttClient.setServer(mqttServer, mqttPort);

    // Make sure any previous TCP connections are closed
    // NOTE:  The PubSubClient library used for MQTT connect assumes that as
    // long as the client is connected, it must be connected to the right place.
    // Closing any stray client sockets here ensures that a new client socket
    // is opened to the right place.
    // client is connected when a different socket is open
    if (outClient->connected()) { outClient->stop(); }

    // Make the MQTT connection
    // Note:  the client id and the user name do not mean anything for
    // ThingSpeak
    MS_DBG(F("Opening MQTT Connection"));
    MS_START_DEBUG_TIMER;
    if (_mqttClient.connect(mqttClientName, mqttUser, _thingSpeakMQTTKey)) {
        MS_DBG(F("MQTT connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));

        if (_mqttClient.publish(topicBuffer, txBuffer)) {
            PRINTOUT(F("ThingSpeak topic published!  Current state:"),
                     parseMQTTState(_mqttClient.state()));
            retVal = true;
        } else {
            PRINTOUT(F("MQTT publish failed with state:"),
                     parseMQTTState(_mqttClient.state()));
            retVal = false;
        }
    } else {
        PRINTOUT(F("MQTT connection failed with state:"),
                 parseMQTTState(_mqttClient.state()));
        delay(1000);
        retVal = false;
    }

    // Disconnect from MQTT
    MS_DBG(F("Disconnecting from MQTT"));
    MS_RESET_DEBUG_TIMER
    _mqttClient.disconnect();
    MS_DBG(F("Disconnected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    return retVal;
}

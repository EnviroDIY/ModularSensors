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
const char* ThingSpeakPublisher::mqttServer = "mqtt3.thingspeak.com";
const int   ThingSpeakPublisher::mqttPort   = 1883;


// Constructors
ThingSpeakPublisher::ThingSpeakPublisher() : dataPublisher() {}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                                         int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}
ThingSpeakPublisher::ThingSpeakPublisher(Logger&     baseLogger,
                                         const char* thingSpeakClientName,
                                         const char* thingSpeakMQTTUser,
                                         const char* thingSpeakMQTTPassword,
                                         const char* thingSpeakChannelID,
                                         int         sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setMQTTClient(thingSpeakClientName);
    setUserName(thingSpeakMQTTUser);
    setPassword(thingSpeakMQTTPassword);
    setChannelID(thingSpeakChannelID);
}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, Client* inClient,
                                         const char* thingSpeakClientName,
                                         const char* thingSpeakMQTTUser,
                                         const char* thingSpeakMQTTPassword,
                                         const char* thingSpeakChannelID,
                                         int         sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setMQTTClient(thingSpeakClientName);
    setUserName(thingSpeakMQTTUser);
    setPassword(thingSpeakMQTTPassword);
    setChannelID(thingSpeakChannelID);
}
// Destructor
ThingSpeakPublisher::~ThingSpeakPublisher() {}


void ThingSpeakPublisher::setMQTTClient(const char* thingSpeakClientName) {
    _thingSpeakClientName = thingSpeakClientName;
}


void ThingSpeakPublisher::setUserName(const char* thingSpeakMQTTUser) {
    _thingSpeakMQTTUser = thingSpeakMQTTUser;
}


void ThingSpeakPublisher::setPassword(const char* thingSpeakMQTTPassword) {
    _thingSpeakMQTTPassword = thingSpeakMQTTPassword;
}


void ThingSpeakPublisher::setChannelID(const char* thingSpeakChannelID) {
    _thingSpeakChannelID = thingSpeakChannelID;
}

// Sets all ThingSpeak parameters
void ThingSpeakPublisher::setThingSpeakParams(
    const char* thingSpeakClientName, const char* thingSpeakMQTTUser,
    const char* thingSpeakMQTTPassword, const char* thingSpeakChannelID) {
    setMQTTClient(thingSpeakClientName);
    setUserName(thingSpeakMQTTUser);
    setPassword(thingSpeakMQTTPassword);
    setChannelID(thingSpeakChannelID);
}


// A way to set members in the begin to use with a bare constructor
void ThingSpeakPublisher::begin(Logger& baseLogger, Client* inClient,
                                const char* thingSpeakClientName,
                                const char* thingSpeakMQTTUser,
                                const char* thingSpeakMQTTPassword,
                                const char* thingSpeakChannelID) {
    setMQTTClient(thingSpeakClientName);
    setChannelID(thingSpeakChannelID);
    setPassword(thingSpeakMQTTPassword);
    setUserName(thingSpeakMQTTUser);
    dataPublisher::begin(baseLogger, inClient);
}
void ThingSpeakPublisher::begin(Logger&     baseLogger,
                                const char* thingSpeakClientName,
                                const char* thingSpeakMQTTUser,
                                const char* thingSpeakMQTTPassword,
                                const char* thingSpeakChannelID) {
    setMQTTClient(thingSpeakClientName);
    setChannelID(thingSpeakChannelID);
    setPassword(thingSpeakMQTTPassword);
    setUserName(thingSpeakMQTTUser);
    dataPublisher::begin(baseLogger);
}


// This sends the data to ThingSpeak
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

    // Create a new buffer for the **topic**
    char topicBuffer[42] = "channels/";
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s",
             _thingSpeakChannelID);
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s", "/publish");
    MS_DBG(F("Topic ["), strlen(topicBuffer), F("]:"), String(topicBuffer));

    // The txBuffer is used for the **payload** only
    txBufferInit(outClient);

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
    if (_mqttClient.connect(_thingSpeakClientName, _thingSpeakMQTTUser,
                            _thingSpeakMQTTPassword)) {
        MS_DBG(F("MQTT connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));

        if (_mqttClient.beginPublish(topicBuffer, txBufferLen, false)) {
            MS_DBG(F("Successfully started publish to topic"),
                   String(topicBuffer));

            txBufferAppend("created_at=");
            txBufferAppend(
                Logger::formatDateTime_ISO8601(Logger::markedLocalUnixTime)
                    .c_str());

            char tempBuffer[2] = "";  // for the channel number
            for (uint8_t i = 0; i < numChannels; i++) {
                txBufferAppend("&field");
                itoa(i + 1, tempBuffer, 10);  // BASE 10
                txBufferAppend(tempBuffer);
                txBufferAppend('=');
                txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
            }
            MS_DBG(F("Message length:"), txBufferLen);
            txBufferFlush();  // NOTE: the PubSubClient library has a write
                              // method, which is call to the underlying
                              // client's write method. This flush does the same
                              // thing.
            _mqttClient.endPublish();
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

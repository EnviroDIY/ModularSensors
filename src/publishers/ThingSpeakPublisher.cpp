/*
 *ThingSpeakPublisher.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include "ThingSpeakPublisher.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constant values for MQTT publish
// I want to refer to these more than once while ensuring there is only one copy in memory
const char *ThingSpeakPublisher::mqttServer = "mqtt.thingspeak.com";
const int ThingSpeakPublisher::mqttPort = 1883;
const char *ThingSpeakPublisher::mqttClient = THING_SPEAK_CLIENT_NAME;
const char *ThingSpeakPublisher::mqttUser = THING_SPEAK_USER_NAME;


// Constructors
ThingSpeakPublisher::ThingSpeakPublisher()
  : dataPublisher()
{
    // MS_DBG(F("ThingSpeakPublisher object created"));
}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger,
                                   uint8_t sendEveryX, uint8_t sendOffset)
  : dataPublisher(baseLogger, sendEveryX, sendOffset)
{
    // MS_DBG(F("ThingSpeakPublisher object created"));
}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, Client *inClient,
                                   uint8_t sendEveryX, uint8_t sendOffset)
  : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset)
{
    // MS_DBG(F("ThingSpeakPublisher object created"));
}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger,
                                   const char *thingSpeakMQTTKey,
                                   const char *thingSpeakChannelID,
                                   const char *thingSpeakChannelKey,
                                   uint8_t sendEveryX, uint8_t sendOffset)
  : dataPublisher(baseLogger, sendEveryX, sendOffset)
{
   setMQTTKey(thingSpeakMQTTKey);
   setChannelID(thingSpeakChannelID);
   setChannelKey(thingSpeakChannelKey);
   // MS_DBG(F("ThingSpeakPublisher object created"));
}
ThingSpeakPublisher::ThingSpeakPublisher(Logger& baseLogger, Client *inClient,
                                   const char *thingSpeakMQTTKey,
                                   const char *thingSpeakChannelID,
                                   const char *thingSpeakChannelKey,
                                   uint8_t sendEveryX, uint8_t sendOffset)
  : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset)
{
   setMQTTKey(thingSpeakMQTTKey);
   setChannelID(thingSpeakChannelID);
   setChannelKey(thingSpeakChannelKey);
   // MS_DBG(F("ThingSpeakPublisher object created"));
}
// Destructor
ThingSpeakPublisher::~ThingSpeakPublisher(){}


void ThingSpeakPublisher::setMQTTKey(const char *thingSpeakMQTTKey)
{
    _thingSpeakMQTTKey = thingSpeakMQTTKey;
    // MS_DBG(F("MQTT Key set!"));
}


void ThingSpeakPublisher::setChannelID(const char *thingSpeakChannelID)
{
    _thingSpeakChannelID = thingSpeakChannelID;
    // MS_DBG(F("Channel ID set!"));
}


void ThingSpeakPublisher::setChannelKey(const char *thingSpeakChannelKey)
{
    _thingSpeakChannelKey = thingSpeakChannelKey;
    // MS_DBG(F("Channel Key set!"));
}

// Sets all 3 ThingSpeak parameters
void ThingSpeakPublisher::setThingSpeakParams(const char *MQTTKey,
                                           const char *channelID,
                                           const char *channelKey)
{
    setMQTTKey(MQTTKey);
    setChannelID(channelID);
    setChannelKey(channelKey);
}


// A way to begin with everything already set
void ThingSpeakPublisher::begin(Logger& baseLogger, Client *inClient,
                                const char *thingSpeakMQTTKey,
                                const char *thingSpeakChannelID,
                                const char *thingSpeakChannelKey)
{
    setMQTTKey(thingSpeakMQTTKey);
    setChannelID(thingSpeakChannelID);
    setChannelKey(thingSpeakChannelKey);
    dataPublisher::begin(baseLogger, inClient);
}
void ThingSpeakPublisher::begin(Logger& baseLogger,
                                const char *thingSpeakMQTTKey,
                                const char *thingSpeakChannelID,
                                const char *thingSpeakChannelKey)
{
    setMQTTKey(thingSpeakMQTTKey);
    setChannelID(thingSpeakChannelID);
    setChannelKey(thingSpeakChannelKey);
    dataPublisher::begin(baseLogger);
}


// This sends the data to ThingSpeak
// bool ThingSpeakPublisher::mqttThingSpeak(void)
int16_t ThingSpeakPublisher::publishData(Client *_outClient)
{
    bool retVal = false;

    // Make sure we don't have too many fields
    // A channel can have a max of 8 fields
    if (_baseLogger->getArrayVarCount() > 8)
    {
        MS_DBG(F("No more than 8 fields of data can be sent to a single ThingSpeak channel!"));
        MS_DBG(F("Only the first 8 fields worth of data will be sent."));
    }
    uint8_t numChannels = min(_baseLogger->getArrayVarCount(), 8);
    MS_DBG(numChannels, F("fields will be sent to ThingSpeak"));

    // Create a buffer for the portions of the request and response
    char tempBuffer[26] = "";

    char topicBuffer[42] = "channels/";
    strcat(topicBuffer, _thingSpeakChannelID);
    strcat(topicBuffer, "/publish/");
    strcat(topicBuffer, _thingSpeakChannelKey);
    MS_DBG(F("Topic ["), strlen(topicBuffer), F("]:"), String(topicBuffer));

    emptyTxBuffer();

    _baseLogger->formatDateTime_ISO8601(Logger::markedEpochTime).toCharArray(tempBuffer, 26);
    strcat(txBuffer, "created_at=");
    strcat(txBuffer, tempBuffer);
    txBuffer[strlen(txBuffer)] = '&';

    for (uint8_t i = 0; i < numChannels; i++)
    {
        strcat(txBuffer, "field");
        itoa(i+1, tempBuffer, 10);  // BASE 10
        strcat(txBuffer, tempBuffer);
        txBuffer[strlen(txBuffer)] = '=';
        _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 26);
        strcat(txBuffer, tempBuffer);
        if (i + 1 != numChannels)
        {
            txBuffer[strlen(txBuffer)] = '&';
        }
    }
    MS_DBG(F("Message ["), strlen(txBuffer), F("]:"), String(txBuffer));

    // Set the client connection parameters
    _mqttClient.setClient(*_outClient);
    _mqttClient.setServer(mqttServer, mqttPort);

    // Make sure any previous TCP connections are closed
    // NOTE:  The PubSubClient library used for MQTT connect assumes that as
    // long as the client is connected, it must be connected to the right place.
    // Closing any stray client sockets here ensures that a new client socket
    // is opened to the right place.
    // client is connected when a different socket is open
    if (_outClient->connected())
    {
        _outClient->stop();
    }

    // Make the MQTT connection
    // Note:  the client id and the user name do not mean anything for ThingSpeak
    MS_DBG(F("Opening MQTT Connection"));
    MS_START_DEBUG_TIMER;
    if (_mqttClient.connect(mqttClient, mqttUser, _thingSpeakMQTTKey))
    {
        MS_DBG(F("MQTT connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));

        if (_mqttClient.publish(topicBuffer, txBuffer))
        {
            PRINTOUT(F("ThingSpeak topic published!  Current state:"), _mqttClient.state());
            retVal = true;
        }
        else
        {
            PRINTOUT(F("MQTT publish failed with state:"), _mqttClient.state());
            retVal = false;
        }
    }
    else
    {
        PRINTOUT(F("MQTT connection failed with state:"), _mqttClient.state());
        retVal = false;
    }

    // Disconnect from MQTT
    MS_DBG(F("Disconnecting from MQTT"));
    MS_RESET_DEBUG_TIMER
    _mqttClient.disconnect();
    MS_DBG(F("Disconnected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    return retVal;
}

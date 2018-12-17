/*
 *ThingSpeakSender.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include "senders/ThingSpeakSender.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constructor
ThingSpeakSender::ThingSpeakSender(Logger& baseLogger, Client& inClient,
                                   uint8_t sendEveryX, uint8_t sendOffset)
  : dataSender(baseLogger, inClient, sendEveryX, sendOffset)
{}
ThingSpeakSender::ThingSpeakSender(Logger& baseLogger, Client& inClient,
                                   const char *thingSpeakMQTTKey,
                                   const char *thingSpeakChannelID,
                                   const char *thingSpeakChannelKey,
                                   uint8_t sendEveryX, uint8_t sendOffset)
  : dataSender(baseLogger, inClient, sendEveryX, sendOffset)
{
   setMQTTKey(thingSpeakMQTTKey);
   setChannelID(thingSpeakChannelID);
   setChannelKey(thingSpeakChannelKey);
}
// Destructor
ThingSpeakSender::~ThingSpeakSender(){}


// Constant values for MQTT publish
// I want to refer to these more than once while ensuring there is only one copy in memory
const char *ThingSpeakSender::mqttServer = "mqtt.thingspeak.com";
const char *ThingSpeakSender::mqttClient = THING_SPEAK_CLIENT_NAME;
const char *ThingSpeakSender::mqttUser = THING_SPEAK_USER_NAME;


void ThingSpeakSender::setMQTTKey(const char *thingSpeakMQTTKey)
{
    _thingSpeakMQTTKey = thingSpeakMQTTKey;
    MS_DBG(F("MQTT Key set!"));
}


void ThingSpeakSender::setChannelID(const char *thingSpeakChannelID)
{
    _thingSpeakChannelID = thingSpeakChannelID;
    MS_DBG(F("Channel ID set!"));
}


void ThingSpeakSender::setChannelKey(const char *thingSpeakChannelKey)
{
    _thingSpeakChannelKey = thingSpeakChannelKey;
    MS_DBG(F("Channel Key set!"));
}

// Sets all 3 ThingSpeak parameters
void ThingSpeakSender::setThingSpeakParams(const char *MQTTKey,
                                           const char *channelID,
                                           const char *channelKey)
{
    setMQTTKey(MQTTKey);
    setChannelID(channelID);
    setChannelKey(channelKey);
}


// This sends the data to ThingSpeak
// bool ThingSpeakSender::mqttThingSpeak(void)
int16_t ThingSpeakSender::sendData(void)
{
    bool retVal = false;

    // Make sure we don't have too many fields
    // A channel can have a max of 8 fields
    if (_baseLogger->getArrayVarCount() > 8)
    {
        MS_DBG(F("No more than 8 fields of data can be sent to a single ThingSpeak channel!"));
        MS_DBG(F("Only the first 8 channels worth of data will be sent."));
    }
    uint8_t numChannels = min(_baseLogger->getArrayVarCount(), 8);
    MS_DBG(numChannels, F(" channels will be sent to ThingSpeak"));

    // Create a buffer for the portions of the request and response
    char tempBuffer[26] = "";

    char topicBuffer[42] = "channels/";
    strcat(topicBuffer, _thingSpeakChannelID);
    strcat(topicBuffer, "/publish/");
    strcat(topicBuffer, _thingSpeakChannelKey);
    MS_DBG(F("Topic: "), String(topicBuffer));

    emptyTxBuffer();

    _baseLogger->formatDateTime_ISO8601(_baseLogger->markedEpochTime).toCharArray(tempBuffer, 26);
    strcat(txBuffer, "created_at=");
    strcat(txBuffer, tempBuffer);
    txBuffer[strlen(txBuffer)] = '&';

    for (uint8_t i = 0; i < numChannels; i++)
    {
        strcat(txBuffer, "field");
        itoa(i+1, tempBuffer, 12);
        strcat(txBuffer, tempBuffer);
        txBuffer[strlen(txBuffer)] = '=';
        _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 26);
        strcat(txBuffer, tempBuffer);
        if (i + 1 != numChannels)
        {
            txBuffer[strlen(txBuffer)] = '&';
        }
    }
    MS_DBG(F("Message: "), String(txBuffer));

    // Set the client connection parameters
    _mqttClient.setClient(*_tinyClient);
    _mqttClient.setServer(mqttServer, 1883);

    // Make the MQTT connection
    // Note:  the client id and the user name do not mean anything for ThingSpeak
    if(_mqttClient.connect(mqttClient, mqttUser, _thingSpeakMQTTKey))
    {
        if (_mqttClient.publish(topicBuffer, txBuffer))
        {
            MS_DBG(F("Topic published!  Current state: "), _mqttClient.state());
            retVal = true;
        }
        else
        {
            MS_DBG(F("MQTT publish failed with state: "), _mqttClient.state());
            retVal = false;
        }
    }
    else
    {
        MS_DBG(F("MQTT connection failed with state: "), _mqttClient.state());
        retVal = false;
    }

    // Disconnect from MQTT
    _mqttClient.disconnect();
    return retVal;
}

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
const char* ThingSpeakPublisher::apiHost             = "api.thingspeak.com";
const int   ThingSpeakPublisher::apiPort             = 80;
const char* ThingSpeakPublisher::channelMetaResource = "/channels/";
const char* ThingSpeakPublisher::mqttServer          = "mqtt3.thingspeak.com";
const int   ThingSpeakPublisher::mqttPort            = 1883;


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


void ThingSpeakPublisher::setRESTAPIKey(const char* thingSpeakAPIKey) {
    _thingSpeakAPIKey = thingSpeakAPIKey;
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
    uint8_t numFields = min(_baseLogger->getArrayVarCount(), 8);
    MS_DBG(numFields, F("fields will be sent to ThingSpeak"));

    // Create a new buffer for the **topic**
    char topicBuffer[42] = "channels/";
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s",
             _thingSpeakChannelID);
    snprintf(topicBuffer + strlen(topicBuffer),
             sizeof(topicBuffer) - strlen(topicBuffer), "%s", "/publish");
    MS_DBG(F("Topic ["), strlen(topicBuffer), F("]:"), topicBuffer);

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

        // The txBuffer is used for the **payload** only
        txBufferInit(outClient);
        txBufferAppend("created_at=");
        txBufferAppend(
            Logger::formatDateTime_ISO8601(Logger::markedLocalUnixTime)
                .c_str());

        char tempBuffer[2] = "";  // for the field number
        for (uint8_t i = 0; i < numFields; i++) {
            txBufferAppend("&field");
            itoa(i + 1, tempBuffer, 10);  // BASE 10
            txBufferAppend(tempBuffer);
            txBufferAppend('=');
            txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
        }
        txBufferAppend("\0");  // null terminate!
        MS_DBG(F("Message length:"), txBufferLen);

        // Do the whole publish at once
        MS_DBG(F("Publishing to ThingSpeak"));
        PRINTOUT(F("\nTopic ["), strlen(topicBuffer), F("]:"), topicBuffer);
        PRINTOUT(F("Message ["), strlen(txBuffer), F("]:"), txBuffer);
        retVal = _mqttClient.publish(topicBuffer, txBuffer, false);

        PRINTOUT(F("ThingSpeak topic published!  Current state:"),
                 parseMQTTState(_mqttClient.state()));
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

// This upddates your channel field names on ThingSpeak
int16_t ThingSpeakPublisher::publishMetadata(Client* outClient) {
    if (!_thingSpeakAPIKey) {
        PRINTOUT(F("No ThingSpeak REST API key set!"));
        return -2;
    }
    // Create a buffer for the portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;
    uint8_t  numFields      = min(_baseLogger->getArrayVarCount(), 8);

    // Open a TCP/IP connection to ThingSpeak
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(apiHost, apiPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
        txBufferInit(outClient);

        // copy the initial put header into the tx buffer
        txBufferAppend(putHeader);

        // add in rest of the URL for the channel metadata
        txBufferAppend(channelMetaResource);
        txBufferAppend(_thingSpeakChannelID);
        txBufferAppend(".json?api_key=");
        txBufferAppend(_thingSpeakAPIKey);
        txBufferAppend("&name=");
        txBufferAppend(_baseLogger->getLoggerID());

        char tempBuffer[2] = "";  // for the field number
        for (uint8_t i = 0; i < numFields; i++) {
            txBufferAppend("&field");
            itoa(i + 1, tempBuffer, 10);  // BASE 10
            txBufferAppend(tempBuffer);
            txBufferAppend('=');
            txBufferAppend(_baseLogger->getVarCodeAtI(i).c_str());
        }

        // add the rest of the HTTP GET headers to the outgoing buffer
        txBufferAppend(HTTPtag);
        txBufferAppend(hostHeader);
        txBufferAppend(apiHost);
        txBufferAppend('\r');
        txBufferAppend('\n');
        txBufferAppend('\r');
        txBufferAppend('\n');

        // Flush the complete request
        txBufferFlush();

        // Wait 10 seconds for a response from the server
        uint32_t start = millis();
        while ((millis() - start) < 10000L && outClient->connected() &&
               outClient->available() < 12) {
            delay(10);
        }

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = outClient->readBytes(tempBuffer, 12);
#if defined(MS_OUTPUT) || defined(MS_2ND_OUTPUT)
        // throw the rest of the response into the tx buffer so we can debug it
        txBufferInit(nullptr);
        txBufferAppend(tempBuffer, 12, true);
        while (outClient->available()) {
            char c = outClient->read();
            txBufferAppend(c);
        }
        txBufferFlush();
#endif

        // Close the TCP/IP connection
        MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        outClient->stop();
        MS_DBG(F("Client stopped after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("\n -- Unable to Establish Connection to ThingSpeak --"));
    }

    // Process the HTTP response
    int16_t responseCode = 0;
    if (did_respond > 0) {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++) {
            responseCode_char[i] = tempBuffer[i + 9];
        }
        responseCode_char[3] = '\0';
        responseCode = atoi(responseCode_char);
        PRINTOUT(F("\n-- Response Code --"));
        PRINTOUT(responseCode);
    } else {
        responseCode = 504;
        PRINTOUT(F("\n-- NO RESPONSE FROM SERVER --"));
    }

    return responseCode;
}

/**
 * @file AWS_IoT_Publisher.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the AWS_IoT_Publisher class.
 */

#include "AWS_IoT_Publisher.h"


// ============================================================================
//  Functions for AWS IoT Core Over MQTT.
// ============================================================================

// Constant values for MQTT publish
// I want to refer to these more than once while ensuring there is only one copy
// in memory
const int   AWS_IoT_Publisher::mqttPort           = 8883;
const char* AWS_IoT_Publisher::samplingFeatureTag = "{\"sampling_feature\":\"";
const char* AWS_IoT_Publisher::timestampTag       = "\",\"timestamp\":\"";


// Constructors
AWS_IoT_Publisher::AWS_IoT_Publisher() : dataPublisher() {}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, true, sendEveryX) {}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, Client* inClient,
                                     int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}
AWS_IoT_Publisher::AWS_IoT_Publisher(
    Logger& baseLogger, const char* awsIoTEndpoint, const char* caCertName,
    const char* clientCertName, const char* clientKeyName,
    const char* samplingFeatureUUID, int sendEveryX)
    : dataPublisher(baseLogger, true, sendEveryX) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
AWS_IoT_Publisher::AWS_IoT_Publisher(
    Logger& baseLogger, Client* inClient, const char* awsIoTEndpoint,
    const char* caCertName, const char* clientCertName,
    const char* clientKeyName, const char* samplingFeatureUUID, int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
// Destructor
AWS_IoT_Publisher::~AWS_IoT_Publisher() {}


void AWS_IoT_Publisher::setEndpoint(const char* awsIoTEndpoint) {
    _awsIoTEndpoint = awsIoTEndpoint;
}


void AWS_IoT_Publisher::setCACertName(const char* caCertName) {
    _caCertName = caCertName;
}


void AWS_IoT_Publisher::setClientCertName(const char* clientCertName) {
    _clientCertName = clientCertName;
}


void AWS_IoT_Publisher::setClientKeyName(const char* clientKeyName) {
    _clientKeyName = clientKeyName;
}

// Sets all AWS IoT Core parameters
void AWS_IoT_Publisher::setAWSIoTParams(const char* awsIoTEndpoint,
                                        const char* caCertName,
                                        const char* clientCertName,
                                        const char* clientKeyName) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
}


// A way to begin with everything already set
void AWS_IoT_Publisher::begin(Logger& baseLogger, Client* inClient,
                              const char* awsIoTEndpoint,
                              const char* caCertName,
                              const char* clientCertName,
                              const char* clientKeyName,
                              const char* samplingFeatureUUID) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
    dataPublisher::begin(baseLogger, inClient);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
void AWS_IoT_Publisher::begin(Logger& baseLogger, const char* awsIoTEndpoint,
                              const char* caCertName,
                              const char* clientCertName,
                              const char* clientKeyName,
                              const char* samplingFeatureUUID) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
    dataPublisher::begin(baseLogger);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}


// This sends the data to AWS IoT Core
int16_t AWS_IoT_Publisher::publishData(Client* outClient, bool) {
    bool retVal = false;

    // Create a new buffer for the **topic**
    char topicBuffer[strlen(_baseLogger->getLoggerID()) + 38] = "";
    snprintf(topicBuffer, sizeof(topicBuffer), "%s/%s",
             _baseLogger->getLoggerID(), _baseLogger->getSamplingFeatureUUID());
    MS_DBG(F("Topic ["), strlen(topicBuffer), F("]:"), String(topicBuffer));

    // The txBuffer is used for the **payload** only
    txBufferInit(outClient);

    // put the start of the JSON into the outgoing response_buffer
    txBufferAppend(samplingFeatureTag);
    txBufferAppend(_baseLogger->getSamplingFeatureUUID());

    txBufferAppend(timestampTag);
    txBufferAppend(
        Logger::formatDateTime_ISO8601(Logger::markedLocalUnixTime).c_str());
    txBufferAppend('"');
    txBufferAppend(',');

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        txBufferAppend('"');
        txBufferAppend(_baseLogger->getVarUUIDAtI(i).c_str());
        txBufferAppend('"');
        txBufferAppend(':');
        txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            txBufferAppend(',');
        } else {
            txBufferAppend('}');
        }
    }

    // Set the client connection parameters
    _mqttClient.setClient(*outClient);
    _mqttClient.setServer(_awsIoTEndpoint, mqttPort);

    // Make sure any previous TCP connections are closed
    // NOTE:  The PubSubClient library used for MQTT connect assumes that as
    // long as the client is connected, it must be connected to the right place.
    // Closing any stray client sockets here ensures that a new client socket
    // is opened to the right place.
    if (outClient->connected()) { outClient->stop(); }

    // Make the MQTT connection
    MS_DBG(F("Opening MQTT Connection to IoT Core"));
    MS_START_DEBUG_TIMER;
    if (_mqttClient.connect(_baseLogger->getLoggerID())) {
        MS_DBG(F("MQTT connected after"), MS_PRINT_DEBUG_TIMER, F("ms"));

        if (_mqttClient.beginPublish(topicBuffer, txBufferLen, false)) {
            MS_DBG(F("Successfully started publish to topic"),
                   String(topicBuffer));
            txBufferFlush();  // NOTE: the PubSubClient library has a write
                              // method, which is call to the underlying
                              // client's write method. This flush does the same
                              // thing.
            _mqttClient.endPublish();
            PRINTOUT(F("AWS IoT Core topic published!  Current state:"),
                     parseMQTTState(_mqttClient.state()));
            retVal = true;
        } else {
            PRINTOUT(F("AWS IoT Core MQTT publish failed with state:"),
                     parseMQTTState(_mqttClient.state()));
            retVal = false;
        }
    } else {
        PRINTOUT(F("AWS IoT Core MQTT connection failed with state:"),
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

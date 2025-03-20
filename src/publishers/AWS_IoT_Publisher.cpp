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
#include <TinyGsmEnums.h>


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
    : dataPublisher(baseLogger, sendEveryX) {}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, Client* inClient,
                                     int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {}
AWS_IoT_Publisher::AWS_IoT_Publisher(
    Logger& baseLogger, const char* awsIoTEndpoint, const char* caCertName,
    const char* clientCertName, const char* clientKeyName,
    const char* samplingFeatureUUID, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger&     baseLogger,
                                     const char* awsIoTEndpoint,
                                     const char* caCertName,
                                     const char* clientCertName,
                                     const char* clientKeyName, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    setEndpoint(awsIoTEndpoint);
    setCACertName(caCertName);
    setClientCertName(clientCertName);
    setClientKeyName(clientKeyName);
}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, Client* inClient,
                                     const char* awsIoTEndpoint,
                                     const char* samplingFeatureUUID,
                                     int         sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setEndpoint(awsIoTEndpoint);
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


// A way to set members in the begin to use with a bare constructor
void AWS_IoT_Publisher::begin(Logger& baseLogger, Client* inClient,
                              const char* awsIoTEndpoint,
                              const char* samplingFeatureUUID) {
    setEndpoint(awsIoTEndpoint);
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

Client* AWS_IoT_Publisher::createClient() {
    if (_baseModem == nullptr) {
        PRINTOUT(F("ERROR! No web client assigned and cannot access a "
                   "logger modem to create one!"));
        return nullptr;
    }
    if (_caCertName == nullptr || _clientCertName == nullptr ||
        _clientKeyName == nullptr) {
        PRINTOUT(F("Cannot create a new AWS secure client without certificate "
                   "names!"));
        return nullptr;
    }
    MS_DBG(F("Creating new secure client with default socket number."));
    Client* newClient = _baseModem->createSecureClient(
        SSLAuthMode::MUTUAL_AUTHENTICATION, SSLVersion::TLS1_3, _caCertName,
        _clientCertName, _clientKeyName);
    if (newClient == nullptr) {
        PRINTOUT(F("Failed to create a new secure client!"));
        return nullptr;
    }
    return newClient;
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

            // put the start of the JSON into the outgoing response_buffer
            txBufferAppend(samplingFeatureTag);
            txBufferAppend(_baseLogger->getSamplingFeatureUUID());

            txBufferAppend(timestampTag);
            txBufferAppend(
                Logger::formatDateTime_ISO8601(Logger::markedLocalUnixTime)
                    .c_str());
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


// This sends the data to AWS IoT Core
int16_t AWS_IoT_Publisher::publishMetadata(Client* outClient) {
    bool retVal = false;

    // Create a new buffer for the **topic**
    char topicBuffer[strlen(_baseLogger->getLoggerID()) + 38 + 9] = "";
    snprintf(topicBuffer, sizeof(topicBuffer), "%s/%s/%s",
             _baseLogger->getLoggerID(), _baseLogger->getSamplingFeatureUUID(),
             '/metadata');
    MS_DBG(F("Topic ["), strlen(topicBuffer), F("]:"), String(topicBuffer));

    // The txBuffer is used for the **payload** only
    txBufferInit(outClient);

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

            // put the start of the JSON into the outgoing response_buffer
            txBufferAppend("{\"logger_id\":\"");
            txBufferAppend(_baseLogger->getLoggerID());
            txBufferAppend("\",\"sampling_feature\":\"");
            txBufferAppend(_baseLogger->getSamplingFeatureUUID());
            txBufferAppend("\",\"logging_interval\":");
            txBufferAppend(_baseLogger->getLoggingInterval());
            txBufferAppend(",\"current_file_name\":\"");
            txBufferAppend(_baseLogger->getFileName().c_str());
            txBufferAppend("\",\"time_zone\":\"");
            txBufferAppend(_baseLogger->getLoggerTimeZone());
            txBufferAppend(",\"number_variables\":");
            txBufferAppend(_baseLogger->getArrayVarCount());
            txBufferAppend(",\"variables\":{");

            for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
                txBufferAppend("\"variable_number\":");
                txBufferAppend(i);
                txBufferAppend(",\"variable_name\":\"");
                txBufferAppend(_baseLogger->getVarNameAtI(i).c_str());
                txBufferAppend("\",\"variable_unit\":\"");
                txBufferAppend(_baseLogger->getVarUnitAtI(i).c_str());
                txBufferAppend("\",\"variable_resolution\":\"");
                txBufferAppend(_baseLogger->getVarResolutionAtI(i));
                txBufferAppend("\",\"variable_code\":\"");
                txBufferAppend(_baseLogger->getVarCodeAtI(i).c_str());
                txBufferAppend("\",\"variable_uuid\":\"");
                txBufferAppend(_baseLogger->getVarUUIDAtI(i).c_str());
                if (i + 1 != _baseLogger->getArrayVarCount()) {
                    txBufferAppend("},");
                } else {
                    txBufferAppend("}}");
                }
            }
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

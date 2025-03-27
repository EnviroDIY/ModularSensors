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
AWS_IoT_Publisher::AWS_IoT_Publisher() : dataPublisher() {
    init();
}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, int sendEveryX)
    : dataPublisher(baseLogger, sendEveryX) {
    init();
}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, Client* inClient,
                                     int sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    init();
}
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
    init();
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
    init();
}
AWS_IoT_Publisher::AWS_IoT_Publisher(Logger& baseLogger, Client* inClient,
                                     const char* awsIoTEndpoint,
                                     const char* samplingFeatureUUID,
                                     int         sendEveryX)
    : dataPublisher(baseLogger, inClient, sendEveryX) {
    setEndpoint(awsIoTEndpoint);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
    init();
}
void AWS_IoT_Publisher::init() {
    // Initialize the sub topics as null pointers
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_SUB_COUNT; i++) {
        sub_topics[i] = nullptr;
    }
    // Initialize the pub topics as null pointers
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_PUB_COUNT; i++) {
        pub_topics[i] = nullptr;
    }
    // Initialize the pub functions as null pointers
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_PUB_COUNT; i++) {
        contentGetrFxns[i] = nullptr;
    }
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

void AWS_IoT_Publisher::setDataPublishTopic(const char* topic) {
    _dataTopic = topic;
}
void AWS_IoT_Publisher::setMetadataPublishTopic(const char* topic) {
    _metadataTopic = topic;
}

void AWS_IoT_Publisher::addSubTopic(const char* topic) {
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_SUB_COUNT; i++) {
        if (sub_topics[i] == nullptr) {
            sub_topics[i] = topic;
            break;
        }
    }
}

void AWS_IoT_Publisher::removeSubTopic(const char* topic) {
    // find and remove
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_SUB_COUNT; i++) {
        if (sub_topics[i] != nullptr and strcmp(sub_topics[i], topic) == 0) {
            sub_topics[i] = nullptr;
        }
    }
}

void AWS_IoT_Publisher::addPublishRequest(const char* topic,
                                          String (*contentGetrFxn)(void)) {
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_PUB_COUNT; i++) {
        if (pub_topics[i] == nullptr) {
            pub_topics[i]      = topic;
            contentGetrFxns[i] = contentGetrFxn;
            break;
        }
    }
}

void AWS_IoT_Publisher::removePublishRequest(const char* topic) {
    // find and remove
    for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_PUB_COUNT; i++) {
        if (pub_topics[i] != nullptr and strcmp(pub_topics[i], topic) == 0) {
            pub_topics[i]      = nullptr;
            contentGetrFxns[i] = nullptr;
        }
    }
}

PubSubClient& AWS_IoT_Publisher::setCallback(MQTT_CALLBACK_SIGNATURE) {
    return _mqttClient.setCallback(callback);
}

void AWS_IoT_Publisher::closeConnection() {
    _waitForSubs = false;
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
    MS_DBG(F("Creating a new TinyGsmSecureClient with default socket number."));
    Client* newClient = _baseModem->createSecureClient(
        SSLAuthMode::MUTUAL_AUTHENTICATION, SSLVersion::TLS1_3, _caCertName,
        _clientCertName, _clientKeyName);
    if (newClient == nullptr) {
        PRINTOUT(F("Failed to create a new secure client!"));
        return nullptr;
    }
    return newClient;
}
void AWS_IoT_Publisher::deleteClient(Client* client) {
    if (_baseModem != nullptr) {
        MS_DBG(F("Attempting to delete the client"));
        return _baseModem->deleteSecureClient(client);
    }
}


// This sends the data to AWS IoT Core
int16_t AWS_IoT_Publisher::publishData(Client* outClient, bool) {
    bool retVal = false;

    MS_DBG(F("Preparing to publish to AWS IoT endpoint"), getEndpoint());
    const char* use_topic = _dataTopic;
    if (_dataTopic == nullptr) {
        // Create a new data topic
        size_t topic_len = strlen(_baseLogger->getLoggerID()) +
            strlen(_baseLogger->getSamplingFeatureUUID()) + 2;
        char* topicBuffer = new char[topic_len]();
        snprintf(topicBuffer, topic_len, "%s/%s", _baseLogger->getLoggerID(),
                 _baseLogger->getSamplingFeatureUUID());
        use_topic = topicBuffer;
    }
    MS_DBG(F("Topic ["), strlen(use_topic), F("]:"), use_topic);

    // We're going to use the existing txBuffer to build the JSON, but then
    // we're going to give PubSubClient the address of the txBuffer to send it
    // as a char*.
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


    char num_buf[6];
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        itoa(i, num_buf, 10);
        txBufferAppend('"');
        txBufferAppend(num_buf);
        txBufferAppend('"');
        txBufferAppend(':');
        txBufferAppend(_baseLogger->getValueStringAtI(i).c_str());
        if (i + 1 != _baseLogger->getArrayVarCount()) {
            txBufferAppend(',');
        } else {
            txBufferAppend('}');
        }
    }
    // null terminate the buffer!
    txBufferAppend('\0');
    MS_DBG(F("Message length:"), txBufferLen);
    MS_DBG(F("strlen on buffer:"), strnlen(txBuffer, MS_SEND_BUFFER_SIZE));

    // Set the client connection parameters
    _mqttClient.setClient(*outClient);
    _mqttClient.setServer(_awsIoTEndpoint, mqttPort);
    // NOTE: The MS_MQTT_MAX_PACKET_SIZE must be bigger than the maximum
    // expected incoming *or* outgoing message size. Incoming pre-signed S3 URLs
    // are >1200 bytes.
    _mqttClient.setBufferSize(MS_MQTT_MAX_PACKET_SIZE);

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

        // immediately subscribe to any requested topics
        // NOTE: Subscribe to topics before publishing data so we don't miss
        // anything
        PRINTOUT(F("\nSubscribing to requested topics"));
        uint8_t subs_added = 0;
        for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_SUB_COUNT; i++) {
            if (sub_topics[i] != nullptr) {
                PRINTOUT(F("Subscribing to topic:"), sub_topics[i]);
                _mqttClient.subscribe(sub_topics[i]);
                subs_added++;
            }
        }
        if (subs_added > 0) {
            _waitForSubs = true;
        } else {
            _waitForSubs = false;
        }
        MS_DBG(F("Subscribed to"), subs_added, F("topics"));

        // Publish the data
        MS_DBG(F("Publishing primary data to AWS IoT Core"));
        PRINTOUT(F("\nTopic ["), strlen(use_topic), F("]:"), use_topic);
        PRINTOUT(F("Message ["), strlen(txBuffer), F("]:"), txBuffer);
        if (_mqttClient.publish(use_topic, txBuffer, false)) {
            PRINTOUT(F("AWS IoT Core topic published!  Current state:"),
                     parseMQTTState(_mqttClient.state()));
            retVal = true;
        } else {
            PRINTOUT(F("AWS IoT Core MQTT publish failed with state:"),
                     parseMQTTState(_mqttClient.state()));
            retVal = false;
        }

        // publish any other messages
        uint8_t pubs_done = 0;
        PRINTOUT(F("\nPublishing to other requested topics"));
        for (uint8_t i = 0; i < MS_AWS_IOT_PUBLISHER_PUB_COUNT; i++) {
            if (pub_topics[i] != nullptr) {
                String pub_content = contentGetrFxns[i]();
                PRINTOUT(F("Topic ["), strlen(pub_topics[i]), F("]:"),
                         pub_topics[i]);
                PRINTOUT(F("Message ["), pub_content.length(), F("]:"),
                         pub_content);
                _mqttClient.publish(pub_topics[i], pub_content.c_str());
                pubs_done++;
            }
        }
        MS_DBG(F("Published to"), pubs_done, F("topics"));

        if (_waitForSubs) {
            MS_DBG(F("Waiting"), MS_AWS_IOT_MAX_CONNECTION_TIME,
                   F("for incoming messages"));
        } else {
            MS_DBG(F("Not waiting for incoming messages"));
        }
        uint32_t start_wait = millis();
        while (_mqttClient.connected() && _waitForSubs &&
               (millis() - start_wait) < MS_AWS_IOT_MAX_CONNECTION_TIME) {
            _mqttClient.loop();
        }

        // Disconnect from MQTT
        MS_DBG(F("Disconnecting from MQTT"));
        MS_RESET_DEBUG_TIMER
        _mqttClient.disconnect();
        MS_DBG(F("Disconnected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("AWS IoT Core MQTT connection failed with state:"),
                 parseMQTTState(_mqttClient.state()));
        delay(1000);
        retVal = false;
    }
    return retVal;
}


// This sends the data to AWS IoT Core
int16_t AWS_IoT_Publisher::publishMetadata(Client* outClient) {
    bool retVal = false;

    const char* use_topic = _metadataTopic;
    if (_metadataTopic == nullptr) {
        // Create a default metadata topic
        size_t topic_len   = strlen(_baseLogger->getLoggerID()) + 10;
        char*  topicBuffer = new char[topic_len]();
        snprintf(topicBuffer, topic_len, "%s/metadata",
                 _baseLogger->getLoggerID());
        use_topic = topicBuffer;
    }
    MS_DBG(F("Topic ["), strlen(use_topic), F("]:"), use_topic);

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

        char num_buf[6];

        // put the start of the JSON into the outgoing response_buffer
        txBufferAppend("{\"logger_id\":\"");
        txBufferAppend(_baseLogger->getLoggerID());
        txBufferAppend("\",\"sampling_feature\":\"");
        txBufferAppend(_baseLogger->getSamplingFeatureUUID());
        txBufferAppend("\",\"logging_interval\":");
        itoa(_baseLogger->getLoggingInterval(), num_buf, 10);
        txBufferAppend(num_buf);
        txBufferAppend(",\"current_file_name\":\"");
        txBufferAppend(_baseLogger->getFileName().c_str());
        txBufferAppend("\",\"time_zone\":\"");
        itoa(_baseLogger->getLoggerTimeZone(), num_buf, 10);
        txBufferAppend(num_buf);
        txBufferAppend(",\"number_variables\":");
        itoa(_baseLogger->getArrayVarCount(), num_buf, 10);
        txBufferAppend(num_buf);
        txBufferAppend("}");
        txBufferAppend('\0');  // null terminate!
        MS_DBG(F("Logger metadata message length:"), txBufferLen);
        PRINTOUT(F("\nTopic ["), strlen(use_topic), F("]:"), use_topic);
        PRINTOUT(F("Message ["), strlen(txBuffer), F("]:"), txBuffer);
        retVal = _mqttClient.publish(use_topic, txBuffer, false);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
            // Create a default metadata topic
            char varTopicBuffer[strlen(use_topic) + 12] = {0};
            snprintf(varTopicBuffer, strlen(use_topic) + 12, "%s/variable%.2u",
                     use_topic, i);

            txBufferInit(outClient);
            txBufferAppend("{\"variable_number\":");
            itoa(i, num_buf, 10);
            txBufferAppend(num_buf);
            txBufferAppend(",\"variable_name\":\"");
            txBufferAppend(_baseLogger->getVarNameAtI(i).c_str());
            txBufferAppend("\",\"variable_unit\":\"");
            txBufferAppend(_baseLogger->getVarUnitAtI(i).c_str());
            txBufferAppend("\",\"variable_resolution\":\"");
            itoa(_baseLogger->getVarResolutionAtI(i), num_buf, 10);
            txBufferAppend(num_buf);
            txBufferAppend("\",\"variable_code\":\"");
            txBufferAppend(_baseLogger->getVarCodeAtI(i).c_str());
            txBufferAppend("\",\"variable_uuid\":\"");
            txBufferAppend(_baseLogger->getVarUUIDAtI(i).c_str());
            txBufferAppend("}");
            txBufferAppend("\0");  // null terminate!
            MS_DBG(F("Variable"), i, F("metadata message length:"),
                   txBufferLen);
            PRINTOUT(F("\nTopic ["), strlen(use_topic), F("]:"), use_topic);
            PRINTOUT(F("Message ["), strlen(txBuffer), F("]:"), txBuffer);
            _mqttClient.publish(varTopicBuffer, txBuffer, false);
        }

        PRINTOUT(F("AWS IoT Core topic published!  Current state:"),
                 parseMQTTState(_mqttClient.state()));
        retVal = true;
    } else {
        PRINTOUT(F("AWS IoT Core MQTT connection failed with state:"),
                 parseMQTTState(_mqttClient.state()));
        delay(1000);
        retVal = _mqttClient.state();
    }

    // Disconnect from MQTT
    MS_DBG(F("Disconnecting from MQTT"));
    MS_RESET_DEBUG_TIMER
    _mqttClient.disconnect();
    MS_DBG(F("Disconnected after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    return retVal;
}

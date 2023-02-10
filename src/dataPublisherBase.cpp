/**
 * @file dataPublisherBase.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the dataPublisher class.
 *
 * This file is a skeleton for sending out remote data.
 */
#include "dataPublisherBase.h"

char    dataPublisher::txBuffer[MS_SEND_BUFFER_SIZE];
Client* dataPublisher::txBufferOutClient = nullptr;
size_t  dataPublisher::txBufferLen;

// Basic chunks of HTTP
const char* dataPublisher::getHeader  = "GET ";
const char* dataPublisher::postHeader = "POST ";
const char* dataPublisher::HTTPtag    = " HTTP/1.1";
const char* dataPublisher::hostHeader = "\r\nHost: ";

// Constructors
dataPublisher::dataPublisher() {}

dataPublisher::dataPublisher(Logger& baseLogger, uint8_t sendEveryX,
                             uint8_t sendOffset)
    : _baseLogger(&baseLogger),
      _sendEveryX(sendEveryX),
      _sendOffset(sendOffset) {
    _baseLogger->registerDataPublisher(this);  // register self with logger
}
dataPublisher::dataPublisher(Logger& baseLogger, Client* inClient,
                             uint8_t sendEveryX, uint8_t sendOffset)
    : _baseLogger(&baseLogger),
      _inClient(inClient),
      _sendEveryX(sendEveryX),
      _sendOffset(sendOffset) {
    _baseLogger->registerDataPublisher(this);  // register self with logger
}
// Destructor
dataPublisher::~dataPublisher() {}


// Sets the client
void dataPublisher::setClient(Client* inClient) {
    _inClient = inClient;
}


// Attaches to a logger
void dataPublisher::attachToLogger(Logger& baseLogger) {
    _baseLogger = &baseLogger;
    _baseLogger->registerDataPublisher(this);  // register self with logger
}


// Sets the parameters for frequency of sending and any offset, if needed
// NOTE:  These parameters are not currently used!!
void dataPublisher::setSendFrequency(uint8_t sendEveryX, uint8_t sendOffset) {
    _sendEveryX = sendEveryX;
    _sendOffset = sendOffset;
}


// "Begins" the publisher - attaches client and logger
void dataPublisher::begin(Logger& baseLogger, Client* inClient) {
    setClient(inClient);
    begin(baseLogger);
}
void dataPublisher::begin(Logger& baseLogger) {
    attachToLogger(baseLogger);
}


void dataPublisher::txBufferInit(Client* outClient) {
    // remember client we are sending to
    txBufferOutClient = outClient;

    // reset buffer length to be empty
    txBufferLen = 0;
}

void dataPublisher::txBufferAppend(const char* data, size_t length) {
    while (length > 0) {
        size_t remaining = MS_SEND_BUFFER_SIZE - txBufferLen;
        size_t amount    = remaining < length ? remaining : length;

        memcpy(&txBuffer[txBufferLen], data, amount);
        length -= amount;
        data += amount;
        txBufferLen += amount;

        if (txBufferLen == MS_SEND_BUFFER_SIZE) { txBufferFlush(); }
    }
}

void dataPublisher::txBufferAppend(const char* s) {
    txBufferAppend(s, strlen(s));
}

void dataPublisher::txBufferAppend(char c) {
    txBufferAppend(&c, 1);
}

void dataPublisher::txBufferFlush() {
#if defined(STANDARD_SERIAL_OUTPUT)
    // send to debugging stream
    STANDARD_SERIAL_OUTPUT.write((const uint8_t*)txBuffer, txBufferLen);
    STANDARD_SERIAL_OUTPUT.flush();
#endif
    txBufferOutClient->write((const uint8_t*)txBuffer, txBufferLen);
    txBufferOutClient->flush();

    txBufferLen = 0;
}

bool dataPublisher::connectionNeeded(void) {
    // connection is always needed unless publisher has special logic
    return true;
}

// This sends data on the "default" client of the modem
int16_t dataPublisher::publishData() {
    if (_inClient == nullptr) {
        PRINTOUT(F("ERROR! No web client assigned to publish data!"));
        return 0;
    } else {
        return publishData(_inClient);
    }
}
// Duplicates for backwards compatibility
int16_t dataPublisher::sendData(Client* outClient) {
    return publishData(outClient);
}
int16_t dataPublisher::sendData() {
    return publishData();
}


// This spits out a string description of the PubSubClient codes
String dataPublisher::parseMQTTState(int state) {
    // // Possible values for client.state()
    // #define MQTT_CONNECTION_TIMEOUT     -4
    // #define MQTT_CONNECTION_LOST        -3
    // #define MQTT_CONNECT_FAILED         -2
    // #define MQTT_DISCONNECTED           -1
    // #define MQTT_CONNECTED               0
    // #define MQTT_CONNECT_BAD_PROTOCOL    1
    // #define MQTT_CONNECT_BAD_CLIENT_ID   2
    // #define MQTT_CONNECT_UNAVAILABLE     3
    // #define MQTT_CONNECT_BAD_CREDENTIALS 4
    // #define MQTT_CONNECT_UNAUTHORIZED    5
    switch (state) {
        case -4: return "-4: MQTT_CONNECTION_TIMEOUT";
        case -3: return "-3: MQTT_CONNECTION_LOST";
        case -2: return "-2: MQTT_CONNECT_FAILED";
        case -1: return "-1: MQTT_DISCONNECTED";
        case 0: return "0: MQTT_CONNECTED";
        case 1: return "1: MQTT_CONNECT_BAD_PROTOCOL";
        case 2: return "2: MQTT_CONNECT_BAD_CLIENT_ID";
        case 3: return "3: MQTT_CONNECT_UNAVAILABLE";
        case 4: return "4: MQTT_CONNECT_BAD_CREDENTIALS";
        case 5: return "5: MQTT_CONNECT_UNAUTHORIZED";
        default: return String(state) + ": UNKNOWN";
    }
}

/**
 * @file dataPublisherBase.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
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

dataPublisher::dataPublisher(Logger& baseLogger, bool requiresSSL,
                             int sendEveryX)
    : _baseLogger(&baseLogger),
      _inClient(nullptr),
      _requiresSSL(requiresSSL),
      _sendEveryX(sendEveryX) {
    _baseLogger->registerDataPublisher(this);  // register self with logger
}
dataPublisher::dataPublisher(Logger& baseLogger, Client* inClient,
                             int sendEveryX)
    : _baseLogger(&baseLogger),
      _inClient(inClient),
      _requiresSSL(false),  // not relevant if we have a client
      _sendEveryX(sendEveryX) {
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


// Sets the interval (in units of the logging interval) between attempted
// data transmissions
void dataPublisher::setSendInterval(int sendEveryX) {
    _sendEveryX = sendEveryX;
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
        // space left in the buffer
        size_t remaining = MS_SEND_BUFFER_SIZE - txBufferLen;
        // the number of characters that will be added to the buffer
        // this will be the lesser of the length desired and the space left in
        // the buffer
        size_t amount = remaining < length ? remaining : length;
        MS_DEEP_DBG(F("Ready to append to txBuffer -> filled:"), txBufferLen,
                    F("available:"), remaining, F("to append:"), amount);

        // copy as much as possible into the buffer
        memcpy(&txBuffer[txBufferLen], data, amount);
        // re-count how much is left to go
        length -= amount;
        // bump forward the pointer to where we're currently adding
        data += amount;
        // bump up the current length of the buffer
        txBufferLen += amount;
        MS_DEEP_DBG(F("Size of txBuffer after append:"), txBufferLen,
                    F("remainging to append:"), length);

        // write out the buffer if it fills
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
    MS_DEEP_DBG(F("Flushing Tx buffer:"));
    if ((txBufferOutClient == nullptr) || (txBufferLen == 0)) {
        // sending into the void...
        txBufferLen = 0;
        return;
    }

#if defined(MS_OUTPUT)
    // write out to the printout stream
    MS_OUTPUT.write((const uint8_t*)txBuffer, txBufferLen);
    MS_OUTPUT.flush();
#endif
#if defined(MS_2ND_OUTPUT)
    // write out to the printout stream
    MS_2ND_OUTPUT.write((const uint8_t*)txBuffer, txBufferLen);
    MS_2ND_OUTPUT.flush();
#endif
    // write out to the client
    txBufferOutClient->write((const uint8_t*)txBuffer, txBufferLen);
    txBufferOutClient->flush();

    uint8_t        tries = 10;
    const uint8_t* ptr   = (const uint8_t*)txBuffer;
    while (true) {
        size_t sent = txBufferOutClient->write(ptr, txBufferLen);
        txBufferLen -= sent;
        ptr += sent;
        if (txBufferLen == 0) {
            // whole message is successfully sent, we are done
            txBufferOutClient->flush();
            return;
        }

#if defined(STANDARD_SERIAL_OUTPUT)
        // warn that we only partially sent the buffer
        STANDARD_SERIAL_OUTPUT.write('!');
#endif
        if (--tries == 0) {
            // can't convince the modem to send the whole message. just break
            // the connection now so it will get reset and we can try to
            // transmit the data again later
            txBufferOutClient = nullptr;
            txBufferLen       = 0;
            return;
        }

        // give the modem a chance to transmit buffered data
        delay(1000);
    }
}

bool dataPublisher::connectionNeeded(void) {
    // connection is always needed unless publisher has special logic
    return true;
}

// This sends data on the "default" client of the modem
int16_t dataPublisher::publishData(bool forceFlush) {
    if (_inClient == nullptr) {
        if (_baseLogger == nullptr) {
            PRINTOUT(F("ERROR! No web client assigned and cannot access a base "
                       "logger to create one!"));
            return -2;
        }
        if (_baseLogger->_logModem == nullptr) {
            PRINTOUT(F("ERROR! No web client assigned and cannot access a "
                       "logger modem to create one!"));
            return -2;
        }
        Client* newClient;
        int16_t retVal = -2;
        if (_requiresSSL) {
            MS_DBG(F("Creating new secure client with default socket number."));
            newClient = _baseLogger->_logModem->createSecureClient();
        } else {
            MS_DBG(F("Creating new client with default socket number."));
            newClient = _baseLogger->_logModem->createClient();
        }
        if (newClient != nullptr) {
            retVal = publishData(newClient, forceFlush);
            delete newClient;  // need to delete to free memory!
        } else {
            PRINTOUT(F("ERROR! Failed to create new client to publish data!"));
        }
        return retVal;
    } else {
        MS_DBG(F("Publishing data with provided client."));
        return publishData(_inClient, forceFlush);
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

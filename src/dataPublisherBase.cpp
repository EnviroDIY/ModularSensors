/**
 * @file dataPublisherBase.cpp
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the dataPublisher class.
 *
 * This file is a skeleton for sending out remote data.
 */
#include "dataPublisherBase.h"

char dataPublisher::txBuffer[MS_SEND_BUFFER_SIZE] = {'\0'};

// Basic chunks of HTTP
const char* dataPublisher::getHeader  = "GET ";
const char* dataPublisher::postHeader = "POST ";
const char* dataPublisher::HTTPtag    = " HTTP/1.1";
const char* dataPublisher::hostHeader = "\r\nHost: ";

// Constructors
dataPublisher::dataPublisher() {
    _baseLogger = NULL;
    _inClient   = NULL;
    _sendEveryX = 1;
    _sendOffset = 0;
    // MS_DBG(F("dataPublisher object created"));
}
dataPublisher::dataPublisher(Logger& baseLogger, uint8_t sendEveryX,
                             uint8_t sendOffset) {
    _baseLogger = &baseLogger;
    _baseLogger->registerDataPublisher(this);  // register self with logger
    _sendEveryX = sendEveryX;
    _sendOffset = sendOffset;
    _inClient   = NULL;
    // MS_DBG(F("dataPublisher object created"));
}
dataPublisher::dataPublisher(Logger& baseLogger, Client* inClient,
                             uint8_t sendEveryX, uint8_t sendOffset) {
    _baseLogger = &baseLogger;
    _baseLogger->registerDataPublisher(this);  // register self with logger
    _sendEveryX = sendEveryX;
    _sendOffset = sendOffset;
    _inClient   = inClient;
    // MS_DBG(F("dataPublisher object created"));
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


// Empties the outgoing buffer
void dataPublisher::emptyTxBuffer(void) {
    MS_DBG(F("Dumping the TX Buffer"));
    for (int i = 0; i < MS_SEND_BUFFER_SIZE; i++) { txBuffer[i] = '\0'; }
}


// Returns how much space is left in the buffer
int dataPublisher::bufferFree(void) {
    MS_DBG(F("Current TX Buffer Size:"), strlen(txBuffer));
    return MS_SEND_BUFFER_SIZE - strlen(txBuffer);
}


// Sends the tx buffer to a stream and then clears it
void dataPublisher::printTxBuffer(Stream* stream, bool addNewLine) {
// Send the out buffer so far to the serial for debugging
#if defined(STANDARD_SERIAL_OUTPUT)
    STANDARD_SERIAL_OUTPUT.write(txBuffer, strlen(txBuffer));
    if (addNewLine) { PRINTOUT('\n'); }
    STANDARD_SERIAL_OUTPUT.flush();
#endif
    stream->write(txBuffer, strlen(txBuffer));
    if (addNewLine) { stream->print("\r\n"); }
    stream->flush();

    // empty the buffer after printing it
    emptyTxBuffer();
}

int16_t
dataPublisher::storAndPublish() {  // superseded ~ put in LoggerBaseExtCpp.h
    // Determine if file already exists
    if (NULL == _baseLogger) return 0;
    MS_DBG(F("logger "), _baseLogger->_loggerID, F("+"),
           _baseLogger->_dataPubInstance);
    String queFileName = String(_baseLogger->_loggerID) +
        String('0' + _baseLogger->_dataPubInstance);

    // Write to file
    File    queFileH;
    uint8_t fileNameLength = queFileName.length() + 1;
    char    charFileName[fileNameLength];
    queFileName.toCharArray(charFileName, fileNameLength);

    if (queFileH.open(charFileName, O_WRITE | O_AT_END)) {
        MS_DBG(F("Opened existing file:"), queFileName);
        // Set access date time
        // setFileTimestamp(logFile, T_ACCESS);
    } else {
        // Create and then open the file in write mode
        if (queFileH.open(charFileName, O_CREAT | O_WRITE | O_AT_END)) {
            MS_DBG(F("Created new file:"), charFileName);
            // Set creation date time
            ;
            ;
            // setFileTimestamp(logFile, T_CREATE);
            // If internet

            // Publish 'x' records, with 'y' delay,  and power available between
            // each record
        } else {
            // error processing
        }
    }
    _baseLogger->printSensorDataCSV(&queFileH);
    queFileH.close();
    publishData();
    return 1;
}

// This sends data on the "default" client of the modem
int16_t dataPublisher::publishData() {
    if (_inClient == NULL) {
        PRINTOUT(F("ERROR! No web client assigned to publish data!"));
        return 0;
    } else {
        return publishData(_inClient);
    }
}
// Duplicates for backwards compatibility
int16_t dataPublisher::sendData(Client* _outClient) {
    return publishData(_outClient);
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

/*
 *dataPublisherBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is a skeleton for sending out remote data.
*/
#include "dataPublisherBase.h"

char dataPublisher::txBuffer[MS_SEND_BUFFER_SIZE] = {'\0'};

// Basic chunks of HTTP
const char *dataPublisher::getHeader = "GET ";
const char *dataPublisher::postHeader = "POST ";
const char *dataPublisher::HTTPtag = "  HTTP/1.1";
const char *dataPublisher::hostHeader = "\r\nHost: ";

// Constructor
dataPublisher::dataPublisher(Logger& baseLogger, uint8_t sendEveryX, uint8_t sendOffset)
{
    _baseLogger = &baseLogger;
    _baseLogger->registerDataPublisher(this);  // register self with logger
    _sendEveryX = sendEveryX;
    _sendOffset = sendOffset;
    _inClient = NULL;
}
dataPublisher::dataPublisher(Logger& baseLogger, Client *inClient, uint8_t sendEveryX, uint8_t sendOffset)
{
    _baseLogger = &baseLogger;
    _baseLogger->registerDataPublisher(this);  // register self with logger
    _sendEveryX = sendEveryX;
    _sendOffset = sendOffset;
    _inClient = inClient;
}
// Destructor
dataPublisher::~dataPublisher(){}


// Empties the outgoing buffer
void dataPublisher::emptyTxBuffer(void)
{
    MS_DBG(F("Dumping the TX Buffer"));
    for (int i = 0; i < MS_SEND_BUFFER_SIZE; i++)
    {
        txBuffer[i] = '\0';
    }
}


// Returns how much space is left in the buffer
int dataPublisher::bufferFree(void)
{
    MS_DBG(F("Current TX Buffer Size:"), strlen(txBuffer));
    return MS_SEND_BUFFER_SIZE - strlen(txBuffer);
}


// Sends the tx buffer to a stream and then clears it
void dataPublisher::printTxBuffer(Stream *stream)
{
    // Send the out buffer so far to the serial for debugging
    #if defined(STANDARD_SERIAL_OUTPUT)
        STANDARD_SERIAL_OUTPUT.write(txBuffer, strlen(txBuffer));
        PRINTOUT('\n');
        STANDARD_SERIAL_OUTPUT.flush();
    #endif
    stream->write(txBuffer, strlen(txBuffer));
    stream->flush();

    // empty the buffer after printing it
    emptyTxBuffer();
}


// This sends data on the "default" client of the modem
int16_t dataPublisher::sendData()
{
    if (_inClient == NULL && _baseLogger->_logModem != NULL)
    {
        _inClient = _baseLogger->_logModem->getClient();
    }

    if (_inClient == NULL) return 0;
    else return sendData(_inClient);
}

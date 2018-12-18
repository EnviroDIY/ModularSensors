/*
 *dataSenderBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is a skeleton for sending out remote data.
*/
#include "dataSenderBase.h"

char dataSender::txBuffer[MS_SEND_BUFFER_SIZE] = {'\0'};

// Basic chunks of HTTP
const char *dataSender::getHeader = "GET ";
const char *dataSender::postHeader = "POST ";
const char *dataSender::HTTPtag = "  HTTP/1.1";
const char *dataSender::hostHeader = "\r\nHost: ";

// Constructor
dataSender::dataSender(Logger& baseLogger, uint8_t sendEveryX, uint8_t sendOffset)
{
    _baseLogger = &baseLogger;
    _baseLogger->registerDataSender(this);  // register self with logger
    _sendEveryX = sendEveryX;
    _sendOffset = sendOffset;
}
// Destructor
dataSender::~dataSender(){}


// Empties the outgoing buffer
void dataSender::emptyTxBuffer(void)
{
    MS_DBG(F("Dumping the TX Buffer"));
    for (int i = 0; i < MS_SEND_BUFFER_SIZE; i++)
    {
        txBuffer[i] = '\0';
    }
}


// Returns how much space is left in the buffer
int dataSender::bufferFree(void)
{
    MS_DBG(F("Current TX Buffer Size: "), strlen(txBuffer));
    return MS_SEND_BUFFER_SIZE - strlen(txBuffer);
}


// Sends the tx buffer to a stream and then clears it
void dataSender::printTxBuffer(Stream *stream)
{
    // Send the out buffer so far to the serial for debugging
    #if defined(STANDARD_SERIAL_OUTPUT)
        PRINTOUT('\n');
        STANDARD_SERIAL_OUTPUT.write(txBuffer, strlen(txBuffer));
        PRINTOUT('\n');
        STANDARD_SERIAL_OUTPUT.flush();
    #endif
    stream->write(txBuffer, strlen(txBuffer));
    stream->flush();

    // empty the buffer after printing it
    emptyTxBuffer();
}

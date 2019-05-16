/*
 *dataPublisherBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is a skeleton for sending out remote data.
*/

// Header Guards
#ifndef dataPublisherBase_h
#define dataPublisherBase_h

// Debugging Statement
// #define MS_DATAPUBLISHERBASE_DEBUG

#ifdef MS_DATAPUBLISHERBASE_DEBUG
#define MS_DEBUGGING_STD "dataPublisherBase"
#endif

// Send Buffer
// This determines how many characters to set out at once over the TCP/UDP
// connection.  Increasing this may decrease data use by a loger, while
// decreasing it will save memory.  Do not make it smaller than 47 (to keep all
// variable values with their UUID's) or bigger than 1500 (a typical TCP/UDP
// Maximum Transmission Unit).
#define MS_SEND_BUFFER_SIZE 750

// Included Dependencies
//#include <Arduino.h>
#include "ModSensorDebugger.h"
#include "LoggerBase.h"
#include "Client.h"

class dataPublisher
{

public:

    // Constructors
    dataPublisher();
    dataPublisher(Logger& baseLogger,
                  uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    dataPublisher(Logger& baseLogger, Client *inClient,
                  uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~dataPublisher();

    // Sets the client
    void setClient(Client *inClient);

    // Attaches to a logger
    void attachToLogger(Logger& baseLogger);

    // Sets the parameters for frequency of sending and any offset, if needed
    // NOTE:  These parameters are not currently used!!
    void setSendFrequency(uint8_t sendEveryX, uint8_t sendOffset);

    // "Begins" the publisher - attaches client and logger
    // Not doing this in the constructor because we expect the publishers to be
    // created in the "global scope" and we cannot control the order in which
    // objects in that global scope will be created.  That is, we cannot
    // guarantee that the logger will actually be created before the publisher
    // that wants to attach to it unless we wait to attach the publisher until
    // in the setup or loop function of the main program.
    void begin(Logger& baseLogger, Client *inClient);
    void begin(Logger& baseLogger);

    // Returns the data destination
    virtual String getEndpoint(void) = 0;

    // This opens a socket to the correct receiver and sends out the formatted data
    // This depends on an internet connection already being made and a client
    // being available
    virtual int16_t sendData(Client *_outClient) = 0;
    virtual int16_t sendData();

protected:
    // The internal logger instance
    Logger *_baseLogger;
    // The internal client
    Client *_inClient;

    static char txBuffer[MS_SEND_BUFFER_SIZE];
    // This returns the number of empty spots in the buffer
    static int bufferFree(void);
    // This fills the TX buffer with nulls ('\0')
    static void emptyTxBuffer(void);
    // This writes the TX buffer to a stream and also to the debugging port
    static void printTxBuffer(Stream *stream);

    uint8_t _sendEveryX;
    uint8_t _sendOffset;

    // Basic chunks of HTTP
    static const char *getHeader;
    static const char *postHeader;
    static const char *HTTPtag;
    static const char *hostHeader;

};

#endif  // Header Guard

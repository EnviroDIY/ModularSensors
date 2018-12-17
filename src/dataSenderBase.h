/*
 *dataSenderBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is a skeleton for sending out remote data.
*/

// Header Guards
#ifndef dataSenderBase_h
#define dataSenderBase_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

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

class dataSender
{

public:

    // Constructor - requires a logger modem
    dataSender(Logger& baseLogger, Client& inClient,
               uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    // Destructor
    virtual ~dataSender();

    // A pointer to the internal client instance
    Client *_tinyClient;

    // Takes advantage of the modem to synchronize the clock
    // This will power the modem up and down
    bool syncRTC();

    // This fills the TX buffer with nulls ('\0')
    static void emptyTxBuffer(void);
    // This writes the TX buffer to a stream and also to the debugging port
    static void printTxBuffer(Stream *stream);

    // This opens a socket to the correct receiver and sends out the formatted data
    // This depends on an internet connection already being made and a client
    // being available
    virtual int16_t sendData() = 0;

protected:
    // The internal logger instance
    Logger *_baseLogger;

    static char txBuffer[MS_SEND_BUFFER_SIZE];
    static int bufferFree(void);

    uint8_t _sendEveryX;
    uint8_t _sendOffset;

};

#endif  // Header Guard

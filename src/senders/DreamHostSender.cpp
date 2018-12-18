/*
 *DreamHostSender.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

#include "DreamHostSender.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

// Constructor
DreamHostSender::DreamHostSender(Logger& baseLogger,
                                 uint8_t sendEveryX, uint8_t sendOffset)
  : dataSender(baseLogger,sendEveryX, sendOffset)
{}
DreamHostSender::DreamHostSender(Logger& baseLogger,
                                 const char *URL, uint8_t sendEveryX,
                                 uint8_t sendOffset)
  : dataSender(baseLogger,sendEveryX, sendOffset)
{
    setDreamHostPortalRX(URL);
}
// Destructor
DreamHostSender::~DreamHostSender(){}


// Constant portions of the requests
const char *DreamHostSender::dreamhostHost = "swrcsensors.dreamhosters.com";
const char *DreamHostSender::loggerTag = "?LoggerID=";
const char *DreamHostSender::timestampTagDH = "&Loggertime=";


// Functions for private SWRC server
void DreamHostSender::setDreamHostPortalRX(const char *URL)
{
    _DreamHostPortalRX = URL;
    MS_DBG(F("Dreamhost portal URL set!"));
}


// This prints the URL out to an Arduino stream
void DreamHostSender::printSensorDataDreamHost(Stream *stream)
{
    stream->print(_DreamHostPortalRX);
    stream->print(loggerTag);
    stream->print(_baseLogger->getLoggerID());
    stream->print(timestampTagDH);
    stream->print(String(Logger::markedEpochTime - 946684800));  // Correct time from epoch to y2k

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++)
    {
        stream->print('&');
        stream->print(_baseLogger->getVarCodeAtI(i));
        stream->print('=');
        stream->print(_baseLogger->getValueStringAtI(i));
    }
}


// This prints a fully structured GET request for DreamHost to the
// specified stream
void DreamHostSender::printDreamHostRequest(Stream *stream)
{
    // Start the request
    stream->print(getHeader);

    // Stream the full URL with parameters
    printSensorDataDreamHost(stream);

    // Send the rest of the HTTP header
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(dreamhostHost);
    stream->print(F("\r\n\r\n"));
}


// Post the data to dream host.
// int16_t DreamHostSender::postDataDreamHost(void)
int16_t DreamHostSender::sendData(Client *_outClient)
{
    // Create a buffer for the portions of the request and response
    char tempBuffer[37] = "";
    uint16_t did_respond = 0;

    // Open a TCP/IP connection to DreamHost
    if(_outClient->connect(dreamhostHost, 80))
    {
        // copy the initial post header into the tx buffer
        strcpy(txBuffer, getHeader);

        // add in the dreamhost receiver URL
        strcat(txBuffer, _DreamHostPortalRX);

        // start the URL parameters
        if (bufferFree() < 16) printTxBuffer(_outClient);
        strcat(txBuffer, loggerTag);
        strcat(txBuffer, _baseLogger->getLoggerID());

        if (bufferFree() < 22) printTxBuffer(_outClient);
        strcat(txBuffer, timestampTagDH);
        itoa(Logger::markedEpochTime - 946684800, tempBuffer, 10);
        strcat(txBuffer, tempBuffer);

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++)
        {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(_outClient);

            txBuffer[strlen(txBuffer)] = '&';
            _baseLogger->getVarCodeAtI(i).toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
            txBuffer[strlen(txBuffer)] = '=';
            _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
        }

        // add the rest of the HTTP GET headers to the outgoing buffer
        if (bufferFree() < 52) printTxBuffer(_outClient);
        strcat(txBuffer, HTTPtag);
        strcat(txBuffer, hostHeader);
        strcat(txBuffer, dreamhostHost);
        txBuffer[strlen(txBuffer)] = '\r';
        txBuffer[strlen(txBuffer)] = '\n';
        txBuffer[strlen(txBuffer)] = '\r';
        txBuffer[strlen(txBuffer)] = '\n';

        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(_outClient);

        uint32_t start_timer = millis();
        while ((millis() - start_timer) < 10000L && _outClient->available() < 12)
        {delay(10);}

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = _outClient->readBytes(tempBuffer, 12);

        // Close the TCP/IP connection as soon as the first 12 characters are read
        // We don't need anything else and stoping here should save data use.
        _outClient->stop();
    }
    else PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost -- "));

    // Process the HTTP response
    int16_t responseCode = 0;
    if (did_respond > 0)
    {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++)
        {
            responseCode_char[i] = tempBuffer[i+9];
        }
        responseCode = atoi(responseCode_char);
    }
    else responseCode=504;

    PRINTOUT(F(" -- Response Code -- "));
    PRINTOUT(responseCode);

    return responseCode;
}

/*
 *DreamHostPublisher.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

#include "DreamHostPublisher.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

// Constant portions of the requests
const char *DreamHostPublisher::dreamhostHost = "swrcsensors.dreamhosters.com";
const int DreamHostPublisher::dreamhostPort = 80;
const char *DreamHostPublisher::loggerTag = "?LoggerID=";
const char *DreamHostPublisher::timestampTagDH = "&Loggertime=";

// Constructor
DreamHostPublisher::DreamHostPublisher(Logger& baseLogger,
                                 uint8_t sendEveryX, uint8_t sendOffset)
  : dataPublisher(baseLogger, sendEveryX, sendOffset)
{}
DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, Client *inClient,
                                 uint8_t sendEveryX, uint8_t sendOffset)
  : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset)
{}
DreamHostPublisher::DreamHostPublisher(Logger& baseLogger,
                                 const char *URL, uint8_t sendEveryX,
                                 uint8_t sendOffset)
  : dataPublisher(baseLogger, sendEveryX, sendOffset)
{
    setDreamHostPortalRX(URL);
}
DreamHostPublisher::DreamHostPublisher(Logger& baseLogger, Client *inClient,
                                 const char *URL, uint8_t sendEveryX,
                                 uint8_t sendOffset)
  : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset)
{
    setDreamHostPortalRX(URL);
}
// Destructor
DreamHostPublisher::~DreamHostPublisher(){}


// Functions for private SWRC server
void DreamHostPublisher::setDreamHostPortalRX(const char *URL)
{
    _DreamHostPortalRX = URL;
    MS_DBG(F("Dreamhost portal URL set!"));
}


// This prints the URL out to an Arduino stream
void DreamHostPublisher::printSensorDataDreamHost(Stream *stream)
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
void DreamHostPublisher::printDreamHostRequest(Stream *stream)
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
// int16_t DreamHostPublisher::postDataDreamHost(void)
int16_t DreamHostPublisher::sendData(Client *_outClient)
{
    // Create a buffer for the portions of the request and response
    char tempBuffer[37] = "";
    uint16_t did_respond = 0;

    // Open a TCP/IP connection to DreamHost
    MS_DBG(F("Connecting client"));
    uint32_t start_timer = millis();
    if(_outClient->connect(dreamhostHost, dreamhostPort))
    {
        MS_DBG(F("Client connected after"), millis() - start_timer, F("ms\n"));

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
        ltoa((Logger::markedEpochTime - 946684800), tempBuffer, 10);  // BASE 10
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

        start_timer = millis();
        while ((millis() - start_timer) < 10000L && _outClient->available() < 12)
        {delay(10);}

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = _outClient->readBytes(tempBuffer, 12);

        // Close the TCP/IP connection
        MS_DBG(F("Stopping client"));
        start_timer = millis();
        _outClient->stop();
        MS_DBG(F("Client stopped after"), millis() - start_timer, F("ms"));
    }
    else PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost --"));

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

    PRINTOUT(F("-- Response Code --"));
    PRINTOUT(responseCode);

    return responseCode;
}

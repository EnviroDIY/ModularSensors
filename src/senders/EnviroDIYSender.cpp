/*
 *EnviroDIYSender.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include "senders/EnviroDIYSender.h"

// To prevent compiler/linker crashes with Enable interrupt
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include <EnableInterrupt.h>
// For all i2c communication, including with the real time clock
#include <Wire.h>


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constructor
EnviroDIYSender::EnviroDIYSender(Logger& baseLogger,
                                 uint8_t sendEveryX, uint8_t sendOffset)
  : dataSender(baseLogger,sendEveryX, sendOffset)
{}
EnviroDIYSender::EnviroDIYSender(Logger& baseLogger,
                                 const char *registrationToken,
                                 const char *samplingFeatureUUID,
                                 uint8_t sendEveryX, uint8_t sendOffset)
  : dataSender(baseLogger,sendEveryX, sendOffset)
{
    setToken(registrationToken);
    _baseLogger->setSamplingFeatureUUID(samplingFeatureUUID);
}
// Destructor
EnviroDIYSender::~EnviroDIYSender(){}


// Constant values for post requests
// I want to refer to these more than once while ensuring there is only one copy in memory
const char *EnviroDIYSender::postEndpoint = "/api/data-stream/";
const char *EnviroDIYSender::enviroDIYHost = "data.envirodiy.org";
const char *EnviroDIYSender::tokenHeader = "\r\nTOKEN: ";
// const unsigned char *EnviroDIYSender::cacheHeader = "\r\nCache-Control: no-cache";
// const unsigned char *EnviroDIYSender::connectionHeader = "\r\nConnection: close";
const char *EnviroDIYSender::contentLengthHeader = "\r\nContent-Length: ";
const char *EnviroDIYSender::contentTypeHeader = "\r\nContent-Type: application/json\r\n\r\n";

const char *EnviroDIYSender::samplingFeatureTag = "{\"sampling_feature\":\"";
const char *EnviroDIYSender::timestampTag = "\",\"timestamp\":\"";


void EnviroDIYSender::setToken(const char *registrationToken)
{
    _registrationToken = registrationToken;
    MS_DBG(F("Registration token set!"));
}


// Calculates how long the JSON will be
uint16_t EnviroDIYSender::calculateJsonSize()
{
    uint16_t jsonLength = 21;  // {"sampling_feature":"
    jsonLength += 36;  // sampling feature UUID
    jsonLength += 15;  // ","timestamp":"
    jsonLength += 25;  // markedISO8601Time
    jsonLength += 2;  //  ",
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++)
    {
        jsonLength += 1;  //  "
        jsonLength += 36;  // variable UUID
        jsonLength += 2;  //  ":
        jsonLength += _baseLogger->getValueStringAtI(i).length();
        if (i + 1 != _baseLogger->getArrayVarCount())
        {
            jsonLength += 1;  // ,
        }
    }
    jsonLength += 1;  // }

    return jsonLength;
}


/*
// Calculates how long the full post request will be, including headers
uint16_t EnviroDIYSender::calculatePostSize()
{
    uint16_t postLength = 31;  // "POST /api/data-stream/ HTTP/1.1"
    postLength += 28;  // "\r\nHost: data.envirodiy.org"
    postLength += 11;  // "\r\nTOKEN: "
    postLength += 36;  // registrationToken
    // postLength += 27;  // "\r\nCache-Control: no-cache"
    // postLength += 21;  // "\r\nConnection: close"
    postLength += 20;  // "\r\nContent-Length: "
    postLength += String(calculateJsonSize()).length();
    postLength += 42;  // "\r\nContent-Type: application/json\r\n\r\n"
    postLength += calculateJsonSize();
    return postLength;
}
*/


// This prints a properly formatted JSON for EnviroDIY to an Arduino stream
void EnviroDIYSender::printSensorDataJSON(Stream *stream)
{
    stream->print(samplingFeatureTag);
    stream->print(_baseLogger->getSamplingFeatureUUID());
    stream->print(timestampTag);
    stream->print(_baseLogger->formatDateTime_ISO8601(_baseLogger->markedEpochTime));
    stream->print(F("\","));

    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++)
    {
        stream->print('"');
        stream->print(_baseLogger->getVarUUIDAtI(i));
        stream->print(F("\":"));
        stream->print(_baseLogger->getValueStringAtI(i));
        if (i + 1 != _baseLogger->getArrayVarCount())
        {
            stream->print(',');
        }
    }

    stream->print('}');
}


// This prints a fully structured post request for EnviroDIY to the
// specified stream.
void EnviroDIYSender::printEnviroDIYRequest(Stream *stream)
{
    // Stream the HTTP headers for the post request
    stream->print(postHeader);
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(enviroDIYHost);
    stream->print(tokenHeader);
    stream->print(_registrationToken);
    // stream->print(cacheHeader);
    // stream->print(connectionHeader);
    stream->print(contentLengthHeader);
    stream->print(calculateJsonSize());
    stream->print(contentTypeHeader);

    // Stream the JSON itself
    printSensorDataJSON(stream);
}


// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
// int16_t EnviroDIYSender::postDataEnviroDIY(void)
int16_t EnviroDIYSender::sendData(Client *_outClient)
{
    // Create a buffer for the portions of the request and response
    char tempBuffer[37] = "";
    uint16_t did_respond = 0;

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    if(_outClient->connect(enviroDIYHost, 80))
    {
        // copy the initial post header into the tx buffer
        strcpy(txBuffer, postHeader);
        strcat(txBuffer, HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        // before adding each line/chunk to the outgoing buffer, we make sure
        // there is space for that line, sending out buffer if not
        if (bufferFree() < 28) printTxBuffer(_outClient);
        strcat(txBuffer, hostHeader);
        strcat(txBuffer, enviroDIYHost);

        if (bufferFree() < 47) printTxBuffer(_outClient);
        strcat(txBuffer, tokenHeader);
        strcat(txBuffer, _registrationToken);

        // if (bufferFree() < 27) printTxBuffer(_outClient);
        // strcat(txBuffer, cacheHeader);

        // if (bufferFree() < 21) printTxBuffer(_outClient);
        // strcat(txBuffer, connectionHeader);

        if (bufferFree() < 26) printTxBuffer(_outClient);
        strcat(txBuffer, contentLengthHeader);
        itoa(calculateJsonSize(), tempBuffer, 10);
        strcat(txBuffer, tempBuffer);

        if (bufferFree() < 42) printTxBuffer(_outClient);
        strcat(txBuffer, contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        if (bufferFree() < 21) printTxBuffer(_outClient);
        strcat(txBuffer, samplingFeatureTag);

        if (bufferFree() < 36) printTxBuffer(_outClient);
        strcat(txBuffer, _baseLogger->getSamplingFeatureUUID());

        if (bufferFree() < 42) printTxBuffer(_outClient);
        strcat(txBuffer, timestampTag);
        _baseLogger->formatDateTime_ISO8601(_baseLogger->markedEpochTime).toCharArray(tempBuffer, 37);
        strcat(txBuffer, tempBuffer);
        txBuffer[strlen(txBuffer)] = '"';
        txBuffer[strlen(txBuffer)] = ',';

        for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++)
        {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(_outClient);

            txBuffer[strlen(txBuffer)] = '"';
            _baseLogger->getVarUUIDAtI(i).toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
            txBuffer[strlen(txBuffer)] = '"';
            txBuffer[strlen(txBuffer)] = ':';
            _baseLogger->getValueStringAtI(i).toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
            if (i + 1 != _baseLogger->getArrayVarCount())
            {
                txBuffer[strlen(txBuffer)] = ',';
            }
            else
            {
                txBuffer[strlen(txBuffer)] = '}';
            }
        }

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
    else PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data Portal -- "));

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

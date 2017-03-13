/*
 *LoggerEnviroDIY.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include <GPRSbee.h>
#include "LoggerEnviroDIY.h"


// Initialization - cannot do this in constructor because it must happen
// within the setup and if using the constuctor cannot control when
// it happens
void LoggerEnviroDIY::init(int timeZone, int SDCardPin, int sensorCount,
                           SensorBase *sensorList[],
                           const char *loggerID/* = 0*/,
                           const char *samplingFeature/* = 0*/,
                           const char *UUIDs[]/* = 0*/)
{
    _timeZone = timeZone;
    _SDCardPin = SDCardPin;
    _sensorList = sensorList;
    _sensorCount = sensorCount;
    _loggerID = loggerID;
    _samplingFeature = samplingFeature;
    _UUIDs = _UUIDs;
};


void LoggerEnviroDIY::setCommunication(xbee beeType/* = "GPRS"*/,
                                       const char *registrationToken/* = "UNKNOWN"*/,
                                       const char *hostAddress/* = "data.envirodiy.org"*/,
                                       const char *APIEndpoint/* = "/api/data-stream/"*/,
                                       int serverTimeout/* = 15000*/,
                                       const char *APN/* = "apn.konekt.io"*/)
{
    _registrationToken = registrationToken;
    _hostAddress = hostAddress;
    _APIEndpoint = APIEndpoint;
    _serverTimeout = serverTimeout;
    _beeType = beeType;
    _APN = APN;
};



// ============================================================================
//  Functions for the EnviroDIY data receivers.
// ============================================================================


// Used to empty out the buffer after a post request.
// Removing this may cause communication issues. If you
// prefer to not see the std::out, remove the print statement
void LoggerEnviroDIY::printRemainingChars(int timeDelay/* = 1*/, int timeout/* = 5000*/)
{
    while (timeout-- > 0 && Serial1.available() > 0)
    {
        while (Serial1.available() > 0)
        {
            // char netChar = Serial1.read();
            // Serial.print(netChar);
            Serial1.read();
            delay(timeDelay);
        }
        delay(timeDelay);
    }
    Serial1.flush();
}


String LoggerEnviroDIY::generateSensorDataJSON(void)
{
    String jsonString = "{";
    jsonString += F("\"_samplingFeature\": \"");
    jsonString += String(_samplingFeature) + F("\", ");
    jsonString += F("\"timestamp\": \"");
    jsonString += String(currentTime) + F("\", ");

    for (int i = 0; i < _sensorCount; i++)
    {
        jsonString += F("\"");
        jsonString += String(_UUIDs[i]) + F("\": ");
        jsonString += String(_sensorList[i]->getValue());
        if (i + 1 != _sensorCount)
        {
            jsonString += F(", ");
        }
    }

    jsonString += F(" }");
    return jsonString;
}

// This function generates the full POST request that gets sent to data.envirodiy.org
// This is only needed for transparent Bee's (ie, WiFi)
void LoggerEnviroDIY::streamPostRequest(Stream & stream)
{
    stream.print(F("POST "));
    stream.print(_APIEndpoint);
    stream.print(F(" HTTP/1.1\r\nHost: "));
    stream.print(_hostAddress);
    stream.print(F("\r\nTOKEN: "));
    stream.print(_registrationToken);
    stream.print(F("\r\nCache-Control: no-cache\r\nContent-Length: "));
    stream.print(generateSensorDataJSON().length());
    stream.print(F("\r\nContent-Type: application/json\r\n\r\n"));
    stream.print(generateSensorDataJSON());
    stream.print(F("\r\n\r\n"));
}

// This function makes an HTTP connection to the server and POSTs data - for WIFI
int LoggerEnviroDIY::postDataWiFi(void)
{
    // Serial.println(F("Checking for remaining data in the buffer"));
    printRemainingChars(5, 5000);
    // Serial.println(F("\n"));

    HTTP_RESPONSE result = HTTP_OTHER;

    // Send the request to the WiFiBee (it's transparent, just goes as a stream)
    Serial1.flush();
    streamPostRequest(Serial1);
    Serial1.flush();


    // Send the request to the serial for debugging
    Serial.println(F("\n \\/\\/---- Post Request to EnviroDIY ----\\/\\/ "));
    Serial.flush();
    streamPostRequest(Serial);
    Serial.flush();

    // Add a brief delay for at least the first 12 characters of the HTTP response
    int timeout = _serverTimeout;
    while ((timeout > 0) && Serial1.available() < 12)
    {
      delay(1);
      timeout--;
    }

    // Process the HTTP response
    if (timeout > 0 && Serial1.available() >= 12)
    {
        char response[10];
        char code[4];
        memset(response, '\0', 10);
        memset(code, '\0', 4);

        int responseBytes = Serial1.readBytes(response, 9);
        int codeBytes = Serial1.readBytes(code, 3);
        Serial.println(F(" -- Response -- "));
        Serial.print(response);
        Serial.println(code);

        printRemainingChars(5, 5000);

        // Check the response to see if it was successful
        if (memcmp(response, F("HTTP/1.0 "), responseBytes) == 0
            || memcmp(response, F("HTTP/1.1 "), responseBytes) == 0)
        {
            if (memcmp(code, F("200"), codeBytes) == 0
                || memcmp(code, F("201"), codeBytes) == 0)
            {
                // The first 12 characters of the response indicate "HTTP/1.1 200" which is success
                result = HTTP_SUCCESS;
            }
            else if (memcmp(code, F("302"), codeBytes) == 0)
            {
                result = HTTP_REDIRECT;
            }
            else if (memcmp(code, F("400"), codeBytes) == 0
                || memcmp(code, F("404"), codeBytes) == 0)
            {
                result = HTTP_FAILURE;
              }
              else if (memcmp(code, F("403"), codeBytes) == 0)
              {
                  result = HTTP_FORBIDDEN;
              }
            else if (memcmp(code, F("500"), codeBytes) == 0)
            {
                result = HTTP_SERVER_ERROR;
            }
        }
    }
    else // Otherwise timeout, no response from server
    {
        result = HTTP_TIMEOUT;
    }

    return result;
}

// This function makes an HTTP connection to the server and POSTs data - for GPRS
int LoggerEnviroDIY::postDataGPRS(void)
{
    // Serial.println(F("Checking for remaining data in the buffer"));
    printRemainingChars(5, 5000);
    // Serial.println(F("\n"));

    HTTP_RESPONSE result = HTTP_OTHER;

    char url[strlen(_hostAddress) + strlen(_APIEndpoint) + 8] = "http://";
    strcat(url,  _hostAddress);
    strcat(url,  _APIEndpoint);
    char header[45] = "TOKEN: ";
    strcat(header, _registrationToken);

    Serial.flush();
    Serial.println(F("\n \\/\\/---- Post Request to EnviroDIY ----\\/\\/ "));
    Serial.println(url);
    Serial.println(header);
    Serial.println(F("Content-Type: application/json"));
    Serial.println(generateSensorDataJSON());
    Serial.flush();

    // Add the needed HTTP Headers
    gprsbee.addHTTPHeaders(header);
    gprsbee.addContentType(F("application/json"));

    // Set up the Response buffer
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));

    // Actually make the post request
    bool response = (gprsbee.doHTTPPOSTWithReply(_APN, url,
                             generateSensorDataJSON().c_str(),
                             strlen(generateSensorDataJSON().c_str()),
                             buffer, sizeof(buffer)));

    if (response)
    {
        result = HTTP_SUCCESS;
    }
    else // Otherwise timeout, no response from server
    {
        result = HTTP_TIMEOUT;
    }

    return result;
}

// Used only for debugging - can be removed
void LoggerEnviroDIY::printPostResult(int result)
{
    switch (result)
    {
        case HTTP_SUCCESS:
        {
            Serial.print(F("\nSucessfully sent data to "));
            Serial.println(_hostAddress);
        }
        break;

        case HTTP_FAILURE:
        {
            Serial.print(F("\nFailed to send data to "));
            Serial.println(_hostAddress);
        }
        break;

        case HTTP_FORBIDDEN:
        {
            Serial.print(F("\nAccess to "));
            Serial.print(_hostAddress);
            Serial.println(F(" forbidden - Check your reguistration token and _UUIDs."));
        }
        break;

        case HTTP_TIMEOUT:
        {
            Serial.print(F("\nRequest to "));
            Serial.print(_hostAddress);
            Serial.println(F(" timed out, no response from server or insufficient signal to send message."));
        }
        break;

        case HTTP_REDIRECT:
        {
            Serial.print(F("\nRequest to "));
            Serial.print(_hostAddress);
            Serial.println(F(" was redirected."));
        }
        break;

        case HTTP_SERVER_ERROR:
        {
            Serial.print(F("\nRequest to "));
            Serial.print(_hostAddress);
            Serial.println(F(" aused an internal server error."));
        }
        break;

        default:
        {
            Serial.print(F("\nAn unknown error has occured, and we're pretty confused\n"));
        }
    }
}



#ifdef DreamHostURL
String LoggerEnviroDIY::generateSensorDataDreamHost(void)
{
    String dhString = DreamHostURL;
    dhString += F("_loggerID=");
    dhString += String(_loggerID);
    dhString += F("&Loggertime=");
    dhString += String(getNow());

    for (int i = 0; i < _sensorCount; i++)
    {
        dhString += F("&");
        dhString += String(_sensorList[i]->getDreamHost());
        dhString += F("=");
        dhString += String(_sensorList[i]->getValue());
    }
    return dhString;

}

// Post the data to dream host.  Do IF AND ONLY IF using GPRSBee
int LoggerEnviroDIY::postDataDreamHost(void)
{
    HTTP_RESPONSE result = HTTP_OTHER;
    printRemainingChars(5, 5000);

    Serial.flush();
    Serial.println(F("\n \\/\\/------ Data to DreamHost ------\\/\\/ "));
    Serial.println(generateSensorDataDreamHost());
    Serial.flush();
    char buffer[10];
    bool response = (gprsbee.doHTTPGET(_APN, generateSensorDataDreamHost(),
                buffer, sizeof(buffer)));

    if (response)
    {
    result = HTTP_SUCCESS;
    }
    else // Otherwise timeout, no response from server
    {
    result = HTTP_TIMEOUT;
    }

    return result;
}
#endif


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================

void LoggerEnviroDIY::log(int loggingIntervalMinutes, int ledPin/* = -1*/)
{
    // Update the timer
    timer.update();

    // Check of the current time is an even interval of the logging interval
    if (currentepochtime % loggingIntervalMinutes*60 == 0)
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(ledPin, HIGH);

        // Update the values from all attached sensors
        updateAllSensors();

        //Save the data record to the log file
        logToSD(generateSensorDataCSV());
        Serial.println(generateSensorDataCSV());  // for debugging

        // Post the data to the WebSDL
        int result;
        switch (_beeType)
        {
            case GPRS:
            {
                result = postDataGPRS();
            };
            case WIFI:
            {
                result = postDataWiFi();
            };
        }
        // Print the response from the WebSDL
        printPostResult(result);

        #ifdef DreamHostURL
            postDataDreamHost(void);
        #endif

        // Turn off the LED
        digitalWrite(ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    //Sleep
    if(sleep){systemSleep();}
}

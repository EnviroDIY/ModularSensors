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


// Communication set up
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
void LoggerEnviroDIY::dumpBuffer(Stream & stream, int timeDelay/* = 1*/, int timeout/* = 5000*/)
{
    while (timeout-- > 0 && stream.available() > 0)
    {
        while (stream.available() > 0)
        {
            // Serial.print(stream.read());
            stream.read();
            delay(timeDelay);
        }
        delay(timeDelay);
    }
    stream.flush();
}


String LoggerEnviroDIY::generateSensorDataJSON(void)
{
    String jsonString = F("{");
    jsonString += F("\"sampling_feature\": \"");
    jsonString += String(LoggerBase::_samplingFeature) + F("\", ");
    jsonString += F("\"timestamp\": \"");
    jsonString += String(LoggerBase::currentTime) + F("\", ");

    for (int i = 0; i < LoggerBase::_sensorCount; i++)
    {
        jsonString += F("\"");
        jsonString += String(LoggerBase::_UUIDs[i]) + F("\": ");
        jsonString += String(LoggerBase::_sensorList[i]->getValue());
        if (i + 1 != LoggerBase::_sensorCount)
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
    dumpBuffer(Serial1);
    int responseCode = 0;

    // Send the request to the WiFiBee (it's transparent, just goes as a stream)
    streamPostRequest(Serial1);
    Serial1.flush();

    // Send the request to the serial for debugging
    Serial.println(F("\n \\/---- Post Request to EnviroDIY ----\\/ "));  // for debugging
    streamPostRequest(Serial);  // for debugging
    Serial.flush();  // for debugging

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
        Serial1.readStringUntil(' ');
        responseCode = Serial1.parseInt();
        Serial.println(F(" -- Response Code -- "));  // for debugging
        Serial.println(responseCode);  // for debugging

        dumpBuffer(Serial1);
    }
    else responseCode=504;

    return responseCode;
}

// This function makes an HTTP connection to the server and POSTs data - for GPRS
int LoggerEnviroDIY::postDataGPRS(void)
{
    dumpBuffer(Serial1);
    int responseCode = 0;

    char url[strlen(_hostAddress) + strlen(_APIEndpoint) + 8] = "http://";
    strcat(url,  _hostAddress);
    strcat(url,  _APIEndpoint);
    char header[45] = "TOKEN: ";
    strcat(header, _registrationToken);

    Serial.println(F("\n \\/---- Post Request to EnviroDIY ----\\/ "));  // for debugging
    Serial.println(url);  // for debugging
    Serial.println(header);  // for debugging
    Serial.println(F("Content-Type: application/json"));  // for debugging
    Serial.println(generateSensorDataJSON());  // for debugging

    // Add the needed HTTP Headers
    gprsbee.addHTTPHeaders(header);
    gprsbee.addContentType(F("application/json"));

    // Actually make the post request
    bool response = (gprsbee.doHTTPPOST(_APN, url,
                             generateSensorDataJSON().c_str(),
                             strlen(generateSensorDataJSON().c_str())));


    // TODO:  Actually read the response
    if (response)
    {
        responseCode = 201;
    }
    else // Otherwise timeout, no response from server
    {
        responseCode = 504;
    }

    return responseCode;
}

// Used only for debugging - can be removed
void LoggerEnviroDIY::printPostResult(int HTTPcode)
{
    switch (HTTPcode)
    {
        case 200:
        case 201:
        case 202:
        {
            Serial.print(F("\nSucessfully sent data to "));
            Serial.println(_hostAddress);
        }
        break;

        case 301:
        case 302:
        {
            Serial.print(F("\nRequest to "));
            Serial.print(_hostAddress);
            Serial.println(F(" was redirected."));
        }
        break;

        case 400:
        case 404:
        {
            Serial.print(F("\nFailed to send data to "));
            Serial.println(_hostAddress);
        }
        break;

        case 403:
        case 405:
        {
            Serial.print(F("\nAccess to "));
            Serial.print(_hostAddress);
            Serial.println(F(" forbidden - Check your reguistration token and _UUIDs."));
        }
        break;

        case 500:
        case 503:
        {
            Serial.print(F("\nRequest to "));
            Serial.print(_hostAddress);
            Serial.println(F(" aused an internal server error."));
        }
        break;

        case 504:
        {
            Serial.print(F("\nRequest to "));
            Serial.print(_hostAddress);
            Serial.println(F(" timed out, no response from server or insufficient signal to send message."));
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
    dhString += String(LoggerBase::_loggerID);
    dhString += F("&Loggertime=");
    dhString += String(getNow());

    for (int i = 0; i < LoggerBase::_sensorCount; i++)
    {
        dhString += F("&");
        dhString += String(LoggerBase::_sensorList[i]->getDreamHost());
        dhString += F("=");
        dhString += String(LoggerBase::_sensorList[i]->getValue());
    }
    return dhString;

}

// Post the data to dream host.  Do IF AND ONLY IF using GPRSBee
int LoggerEnviroDIY::postDataDreamHost(void)
{
    int responseCode = 0;
    dumpBuffer(Serial1);

    Serial.println(F("\n \\/------ Data to DreamHost ------\\/ "));  // for debugging
    Serial.println(generateSensorDataDreamHost());  // for debugging

    // Set up buffer to recieve response and fill with \0's
    char buffer[10];
    memset(buffer, '\0', sizeof(buffer));

    bool response = (gprsbee.doHTTPGET(_APN, generateSensorDataDreamHost(),
                                       buffer, sizeof(buffer)));

    // TODO:  Actually read the response
    if (response)
    {
        Serial.println(buffer)
        responseCode = 201;
    }
    else // Otherwise timeout, no response from server
    {
        responseCode = 504;
    }

    return responseCode;
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
    if (LoggerBase::currentepochtime % loggingIntervalMinutes*60 == 0)
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(ledPin, HIGH);

        // Update the values from all attached sensors
        updateAllSensors();

        //Save the data record to the log file
        logToSD(generateSensorDataCSV());

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
                printPostResult(result);  // for debugging
            };
        }
        // Print the response from the WebSDL

        #ifdef DreamHostURL
            result = postDataDreamHost();
            printPostResult(result);  // for debugging
        #endif

        // Turn off the LED
        digitalWrite(ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    //Sleep
    if(sleep){systemSleep();}
}

/*
 *LoggerDreamHost.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include <GPRSbee.h>
#include "LoggerDreamHost.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================
void LoggerDreamHost::setDreamHostURL(const char *URL)
{
    _DreamHostURL = URL;
}

String LoggerDreamHost::generateSensorDataDreamHost(void)
{
    String dhString = String(_DreamHostURL);
    dhString += F("?LoggerID=");
    dhString += String(LoggerBase::_loggerID);
    dhString += F("&Loggertime=");
    dhString += String(LoggerBase::markedEpochTime - 946684800);  // Coorect time from epoch to y2k

    for (int i = 0; i < LoggerBase::_sensorCount; i++)
    {
        dhString += F("&");
        dhString += LoggerBase::_sensorList[i]->getDreamHost();
        dhString += F("=");
        dhString += LoggerBase::_sensorList[i]->getValueString();
    }
    return dhString;

}

// This function generates the full GET request that gets sent to DreamHost
// This is only needed for transparent Bee's (ie, WiFi)
void LoggerDreamHost::streamDreamHostRequest(Stream *stream)
{
    stream->print(F("GET /portalRX_EST_universal.php"));
    stream->print(generateSensorDataDreamHost());
    stream->print(F("  HTTP/1.1"));
    stream->print(F("\r\nHost: swrcsensors.dreamhosters.com"));
    stream->print(F("\r\nConnection: close"));
    stream->print(F("\r\n\r\n"));
}

// Post the data to dream host.  Do IF AND ONLY IF using GPRSBee
int LoggerDreamHost::postDataDreamHost(void)
{
    // Turn on the modem and connect to the network
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            _modemOnOff->on();
            // _modem->waitForNetwork();
            // _modem->gprsConnect(_APN, "", "");
            // _client->connect("data.envirodiy.org", 80);
            break;
        }
        case WIFIBee:
            {break;}
    }

    // Send the request to the serial for debugging
    Serial.println(F("\n \\/------ Data to DreamHost ------\\/ "));  // for debugging
    streamDreamHostRequest(&Serial);  // for debugging
    Serial.flush();  // for debugging

    // Send the request to the modem stream
    dumpBuffer(LoggerEnviroDIY::_modemStream);
    streamEnviroDIYRequest(LoggerEnviroDIY::_modemStream);
    LoggerEnviroDIY::_modemStream->flush();  // wait for sending to finish

    // Add a brief delay for at least the first 12 characters of the HTTP response
    int timeout = 1500;
    while ((timeout > 0) && _modemStream->available() < 12)
    {
      delay(1);
      timeout--;
    }

    // Process the HTTP response
    int responseCode = 0;
    if (timeout > 0 && _modemStream->available() >= 12)
    {
        _modemStream->readStringUntil(' ');
        responseCode = _modemStream->parseInt();
        Serial.println(F(" -- Response Code -- "));  // for debugging
        Serial.println(responseCode);  // for debugging

        dumpBuffer(_modemStream);
    }
    else responseCode=504;

    // Disconnect and turn off the modem
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            // _client->stop();
            // _modem->gprsDisconnect();
            _modemOnOff->off();
            break;
        }
        case WIFIBee:
            {break;}
    }

    return responseCode;
}


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================

void LoggerDreamHost::log(void)
{
    // Update the timer
    // loggerTimer.update();

    // Check of the current time is an even interval of the logging interval
    if (checkInterval())
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(LoggerBase::_ledPin, HIGH);

        // Update the static time variables with the current time
        markTime();
        // Update the values from all attached sensors
        updateAllSensors();
        // Immediately put sensors to sleep to save power
        sensorsSleep();

        //Save the data record to the log file
        logToSD(generateSensorDataCSV());

        // Post the data to the WebSDL
        int result = postDataEnviroDIY();
        // Print the response from the WebSDL
        printPostResult(result);  // for debugging

        // Post the data to the WebSDL
        int result2 = postDataDreamHost();
        // Print the response from the WebSDL
        printPostResult(result2);  // for debugging

        // Turn off the LED
        digitalWrite(LoggerBase::_ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    //Sleep
    if(LoggerBase::sleep){systemSleep();}
}

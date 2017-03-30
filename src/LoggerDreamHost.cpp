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
    dhString += String(LoggerBase::markedEpochTime);

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
int LoggerDreamHost::postDataDreamHost(void)
{
    int responseCode = 0;
    dumpBuffer(LoggerEnviroDIY::_beeStream);

    Serial.println(F("\n \\/------ Data to DreamHost ------\\/ "));  // for debugging
    Serial.println(generateSensorDataDreamHost());  // for debugging

    // Set up buffer to recieve response and fill with \0's
    char buffer[10];
    memset(buffer, '\0', sizeof(buffer));

    bool response = (gprsbee.doHTTPGET(LoggerEnviroDIY::_APN, generateSensorDataDreamHost(),
                                       buffer, sizeof(buffer)));

    // TODO:  Actually read the response
    if (response)
    {
        Serial.println(buffer);
        responseCode = 201;
    }
    else // Otherwise timeout, no response from server
    {
        responseCode = 504;
    }

    return responseCode;
}


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================

void LoggerDreamHost::log(void)
{
    // Update the timer
    // timer.update();

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
        int result;
        switch (LoggerEnviroDIY::_beeType)
        {
            case GPRSv4:
            case GPRSv6:
            {
                result = postDataGPRS();
                // Print the response from the WebSDL
                printPostResult(result);  // for debugging
                result = postDataDreamHost();
                printPostResult(result);  // for debugging
                break;
            };
            case WIFI:
            {
                result = postDataWiFi();
                // Print the response from the WebSDL
                printPostResult(result);  // for debugging
                Serial.println(F("The DreamHost module cannot be used with WiFi at this time."));  // for debugging
                break;
            };
        }

        // Turn off the LED
        digitalWrite(LoggerBase::_ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    //Sleep
    if(LoggerBase::sleep){systemSleep();}
}

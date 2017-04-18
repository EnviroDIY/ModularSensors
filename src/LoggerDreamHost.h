/*
 *LoggerDreamHost.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#ifndef LoggerDreamHost_h
#define LoggerDreamHost_h


#include <Arduino.h>
#include "LoggerEnviroDIY.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================
class LoggerDreamHost : public virtual LoggerEnviroDIY
{
public:
    // Functions for private SWRC server
    void setDreamHostPortalRX(const char *URL)
    {
        _DreamHostPortalRX = URL;
    }

    String generateSensorDataDreamHost(void)
    {
        String dhString = String(_DreamHostPortalRX);
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

#if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_ESP8266) || defined(TINY_GSM_MODEM_XBEE)

    // Post the data to dream host.
    int postDataDreamHost(void)
    {
        modem.connect("swrcsensors.dreamhosters.com", 80);

        // Send the request to the serial for debugging
        Serial.println(F("\n \\/------ Data to DreamHost ------\\/ "));  // for debugging
        streamDreamHostRequest(&Serial);  // for debugging
        Serial.flush();  // for debugging

        // Send the request to the modem stream
        modem.dumpBuffer(modem._modemStream);
        streamDreamHostRequest(modem._modemStream);
        modem._modemStream->flush();  // wait for sending to finish

        // Add a brief delay for at least the first 12 characters of the HTTP response
        int timeout = 1500;
        while ((timeout > 0) && modem._modemStream->available() < 12)
        {
          delay(1);
          timeout--;
        }

        // Process the HTTP response
        int responseCode = 0;
        if (timeout > 0 && modem._modemStream->available() >= 12)
        {
            // Serial.println("****" + modem._modemStream->readStringUntil(' ') + "****");  // for debugging
            // Serial.println("****" + modem._modemStream->readStringUntil(' ') + "****");  // for debugging
            modem._modemStream->readStringUntil(' ');
            responseCode = modem._modemStream->readStringUntil(' ').toInt();
            modem.dumpBuffer(modem._modemStream);
        }
        else responseCode=504;

        Serial.println(F(" -- Response Code -- "));  // for debugging
        Serial.println(responseCode);  // for debugging

        modem.stop();

        return responseCode;
    }

    // Convience functions to do it all
    void log(void) override
    {
        // Update the timer
        // This runs the timer's "now" function [in our case getNow()] and then
        // checks all of the registered timer events to see if they should run
        // loggerTimer.update();

        // Check of the current time is an even interval of the logging interval
        if (checkInterval())
        {
            // Print a line to show new reading
            Serial.println(F("------------------------------------------"));  // for debugging
            // Turn on the LED to show we're taking a reading
            digitalWrite(LoggerBase::_ledPin, HIGH);

            // Turn on the modem to let it start searching for the network
            modem.modemOnOff->on();

            // Update the time variables with the current time
            markTime();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();

            //Save the data record to the log file
            logToSD(generateSensorDataCSV());

            // Connect to the network
            if (modem.connectNetwork())
            {
                // Post the data to the WebSDL
                postDataEnviroDIY();

                // Print the response from the WebSDL
                // int result = postDataEnviroDIY();
                // modem.printHTTPResult(result);  // for debugging

                // Post the data to DreamHost
                postDataDreamHost();
                // Print the response from DreamHost
                // int result2 = postDataDreamHost();
                // modem.printHTTPResult(result2);  // for debugging
            }
            // Disconnect from the network
            modem.disconnectNetwork();

            // Turn on the modem off
            modem.modemOnOff->off();

            // Turn off the LED
            digitalWrite(LoggerBase::_ledPin, LOW);
            // Print a line to show reading ended
            Serial.println(F("------------------------------------------\n"));  // for debugging
        }

        //Sleep
        if(_sleep){systemSleep();}
    }

#endif

private:
    const char *_DreamHostPortalRX;

    void streamDreamHostRequest(Stream *stream)
    {
        stream->print(String(F("GET ")));
        stream->print(generateSensorDataDreamHost());
        stream->print(String(F("  HTTP/1.1")));
        stream->print(String(F("\r\nHost: swrcsensors.dreamhosters.com")));
        stream->print(String(F("\r\n\r\n")));
    }
};

#endif

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
class LoggerDreamHost : public LoggerEnviroDIY
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
        dhString += String(Logger::_loggerID);
        dhString += F("&Loggertime=");
        dhString += String(Logger::markedEpochTime - 946684800);  // Coorect time from epoch to y2k

        for (int i = 0; i < Logger::_variableCount; i++)
        {
            dhString += F("&");
            dhString += Logger::_variableList[i]->getVarCode();
            dhString += F("=");
            dhString += Logger::_variableList[i]->getValueString();
        }
        return dhString;
    }

#if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || \
    defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
    defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_ESP8266) || \
    defined(TINY_GSM_MODEM_XBEE)

    // Post the data to dream host.
    int postDataDreamHost(void)
    {
        modem.connect("swrcsensors.dreamhosters.com", 80);

        // Send the request to the serial for debugging
        PRINTOUT(F("\n \\/------ Data to DreamHost ------\\/ \n"));
        streamDreamHostRequest(&Serial);  // for debugging
        Serial.flush();  // for debugging

        // Send the request to the modem stream
        modem.dumpBuffer(modem.stream);
        streamDreamHostRequest(modem.stream);
        modem.stream->flush();  // wait for sending to finish

        // Wait for at least the first 12 characters to make it across
        unsigned long timeout = 1500;
        for (unsigned long start = millis(); millis() - start < timeout; )
        {
            if (modem.stream->available() >= 12) break;
        }

        // Process the HTTP response
        int responseCode = 0;
        if (modem.stream->available() >= 12)
        {
            modem.stream->readStringUntil(' ');  // Throw away the "HTTP/1.1"
            responseCode = modem.stream->readStringUntil(' ').toInt();
            modem.dumpBuffer(modem.stream);
        }
        else responseCode=504;

        PRINTOUT(F(" -- Response Code -- \n"));
        PRINTOUT(responseCode, F("\n"));

        modem.stop();

        return responseCode;
    }

    // Convience functions to do it all
    virtual void log(void) override
    {
        // Check of the current time is an even interval of the logging interval
        if (checkInterval())
        {
            // Print a line to show new reading
            PRINTOUT(F("------------------------------------------\n"));
            // Turn on the LED to show we're taking a reading
            digitalWrite(_ledPin, HIGH);

            // Turn on the modem to let it start searching for the network
            LoggerEnviroDIY::modem.modemOnOff->on();

            // Wake up all of the sensors
            // I'm not doing as part of sleep b/c it may take up to a second or
            // two for them all to wake which throws off the checkInterval()
            sensorsWake();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();

            // Create a csv data record and save it to the log file
            logToSD(generateSensorDataCSV());

            // Connect to the network
            if (modem.connectNetwork())
            {
                // Post the data to the WebSDL
                // postDataEnviroDIY();

                // Post the data to DreamHost
                // postDataDreamHost();

                // Get the unix timestamp from Sodaq's website
                modem.getUnixTime();
            }
            // Disconnect from the network
            modem.disconnectNetwork();

            // Turn on the modem off
            modem.modemOnOff->off();

            // Turn off the LED
            digitalWrite(_ledPin, LOW);
            // Print a line to show reading ended
            PRINTOUT(F("------------------------------------------\n\n"));
        }

        // Sleep
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

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
        DBGVA(F("Dreamhost portal URL set!\n"));
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

    void streamDreamHostRequest(Stream *stream)
    {
        stream->print(String(F("GET ")));
        stream->print(generateSensorDataDreamHost());
        stream->print(String(F("  HTTP/1.1")));
        stream->print(String(F("\r\nHost: swrcsensors.dreamhosters.com")));
        stream->print(String(F("\r\n\r\n")));
    }

#if defined(USE_TINY_GSM)

    // Post the data to dream host.
    int postDataDreamHost(void)
    {
        // Create a buffer for the response
        char response_buffer[12] = "";
        bool did_respond = false;

        // Open a TCP/IP connection to DreamHost
        if(modem.connect("swrcsensors.dreamhosters.com", 80))
        {
            // Send the request to the serial for debugging
            PRINTOUT(F("\n \\/------ Data to DreamHost ------\\/ \n"));
            streamDreamHostRequest(&Serial);  // for debugging
            Serial.flush();  // for debugging

            // Send the request to the modem stream
            modem.dumpBuffer(modem.stream);
            streamDreamHostRequest(modem.stream);
            modem.stream->flush();  // wait for sending to finish

            // Read only the first 12 characters of the response
            // We're only reading as far as the http code, anything beyond that
            // we don't care about so we're not reading to save on total
            // data used for transmission.
            did_respond = modem.stream->read(response_buffer, 12)

            // Close the TCP/IP connection as soon as the first 12 characters are read
            // We don't need anything else and stoping here should save data use.
            modem.stop();
        }
        else PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost -- \n"));


        // Process the HTTP response
        int responseCode = 0;
        if (did_respond)
        {
            char responseCode_char[4];
            for (int i = 0; i < 3; i++)
            {
                responseCode_char[i] = response_buffer[i+9]
            }
            responseCode = atoi(responseCode_char);
            modem.dumpBuffer(modem.stream);
        }
        else responseCode=504;

        PRINTOUT(F(" -- Response Code -- \n"));
        PRINTOUT(responseCode, F("\n"));

        return responseCode;
    }

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //
    // This is a one-and-done to log data
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
            LoggerEnviroDIY::modem.on();

            // Wake up all of the sensors
            // I'm not doing as part of sleep b/c it may take up to a second or
            // two for them all to wake which throws off the checkInterval()
            sensorsWake();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();

            // Connect to the network
            if (modem.connectNetwork())
            {
                // Post the data to the WebSDL
                postDataEnviroDIY();

                // Post the data to DreamHost
                postDataDreamHost();

                // Sync the clock every 288 readings (1/day at 5 min intervals)
                // if (_numReadings % 288 == 0)
                // {
                //     modem.syncDS3231();
                // }

                // Disconnect from the network
                modem.disconnectNetwork();
            }

            // Turn on the modem off
            modem.off();

            // Create a csv data record and save it to the log file
            logToSD(generateSensorDataCSV());

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
};

#endif
